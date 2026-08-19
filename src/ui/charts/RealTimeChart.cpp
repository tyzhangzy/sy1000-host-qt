#include "RealTimeChart.h"
#include <QPainterPath>
#include <QtMath>
#include <QDebug>
#include <QtAlgorithms>

RealTimeChart::RealTimeChart(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAntialiasing(true);
    setRenderTarget(QQuickPaintedItem::RenderTarget::FramebufferObject);
    setImplicitWidth(800);
    setImplicitHeight(400);
}

RealTimeChart::~RealTimeChart() = default;

void RealTimeChart::paint(QPainter *painter)
{
    if (!painter) return;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    painter->fillRect(boundingRect(), Qt::white);

    QRectF plotRect(
        MARGIN_LEFT,
        MARGIN_TOP,
        width() - MARGIN_LEFT - MARGIN_RIGHT,
        height() - MARGIN_TOP - MARGIN_BOTTOM
    );

    if (m_showGrid)
        drawGrid(painter, plotRect);

    drawAxes(painter, plotRect);

    int total = 0;
    {
        QMutexLocker locker(&m_dataMutex);
        for (const auto &s : m_series)
            total += s.points.size();
    }
    if (total > 200) {
        drawCurveOptimized(painter, plotRect);
    } else {
        drawCurve(painter, plotRect);
    }

    if (!m_title.isEmpty()) {
        painter->setPen(Qt::black);
        painter->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
        painter->drawText(QRectF(0, 5, width(), 25), Qt::AlignCenter, m_title);
    }
}

void RealTimeChart::drawGrid(QPainter* painter, const QRectF& rect)
{
    painter->setPen(QPen(m_gridColor, 1, Qt::DotLine));
    
    // 垂直网格线（时间轴）- 10等分
    for (int i = 0; i <= 10; ++i) {
        double x = rect.left() + rect.width() * i / 10.0;
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }
    
    // 水平网格线（数值轴）- 8等分
    for (int i = 0; i <= 8; ++i) {
        double y = rect.top() + rect.height() * i / 8.0;
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}

void RealTimeChart::drawAxes(QPainter* painter, const QRectF& rect)
{
    painter->setPen(QPen(Qt::black, 1.5));
    painter->setFont(QFont("Microsoft YaHei", 9));

    // X轴
    painter->drawLine(QPointF(rect.left(), rect.bottom()), QPointF(rect.right(), rect.bottom()));
    // 左Y轴
    painter->drawLine(QPointF(rect.left(), rect.top()), QPointF(rect.left(), rect.bottom()));
    // 右Y轴
    painter->drawLine(QPointF(rect.right(), rect.top()), QPointF(rect.right(), rect.bottom()));

    painter->setPen(QPen(Qt::black, 1));

    // 左Y轴刻度和标签
    for (int i = 0; i <= 8; ++i) {
        double ratio = 1.0 - i / 8.0;  // 从上到下
        double value = m_yMin + (m_yMax - m_yMin) * ratio;
        double y = rect.top() + rect.height() * i / 8.0;
        painter->drawLine(QPointF(rect.left() - 5, y), QPointF(rect.left(), y));
        QString label = QString::number(value, 'f', 1);
        QRectF textRect(rect.left() - 55, y - 10, 50, 20);
        painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }

    // 右Y轴刻度和标签
    for (int i = 0; i <= 8; ++i) {
        double ratio = 1.0 - i / 8.0;
        double value = m_rightYMin + (m_rightYMax - m_rightYMin) * ratio;
        double y = rect.top() + rect.height() * i / 8.0;
        painter->drawLine(QPointF(rect.right(), y), QPointF(rect.right() + 5, y));
        QString label = QString::number(value, 'f', 0);
        QRectF textRect(rect.right() + 8, y - 10, 50, 20);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, label);
    }

    // 左Y轴单位标签
    if (!m_yAxisLabel.isEmpty()) {
        painter->save();
        painter->translate(15, rect.center().y());
        painter->rotate(-90);
        painter->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        painter->drawText(QRectF(-50, -10, 100, 20), Qt::AlignCenter, m_yAxisLabel);
        painter->restore();
    }

    // 右Y轴单位标签
    if (!m_rightYAxisLabel.isEmpty()) {
        painter->save();
        painter->translate(width() - 15, rect.center().y());
        painter->rotate(-90);
        painter->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        painter->drawText(QRectF(-50, -10, 100, 20), Qt::AlignCenter, m_rightYAxisLabel);
        painter->restore();
    }

    // X轴时间标签
    qint64 startTime = 0, endTime = 0;
    {
        QMutexLocker locker(&m_dataMutex);
        if (m_series.isEmpty() || m_series[0].points.isEmpty())
            return;
        startTime = m_series[0].points.first().timestamp;
        endTime = m_series[0].points.last().timestamp;
    }
    qint64 timeRange = qMax(m_timeRangeMs, endTime - startTime);

    painter->setFont(QFont("Microsoft YaHei", 8));
    int divisions = 5;
    for (int i = 0; i <= divisions; ++i) {
        double x = rect.left() + rect.width() * i / divisions;
        qint64 timeOffset = timeRange * i / divisions;
        QString timeStr = formatElapsedTime(timeOffset);
        QRectF textRect(x - 40, rect.bottom() + 5, 80, 20);
        painter->drawText(textRect, Qt::AlignCenter, timeStr);
    }
}

void RealTimeChart::drawCurve(QPainter* painter, const QRectF& rect)
{
    QMutexLocker locker(&m_dataMutex);

    if (m_autoScale)
        calculateYRange();

    qint64 maxEnd = 0;
    for (const auto &s : m_series)
        if (!s.points.isEmpty() && s.points.last().timestamp > maxEnd)
            maxEnd = s.points.last().timestamp;
    if (maxEnd == 0)
        return;

    qint64 timeRange = qMax(m_timeRangeMs, maxEnd);
    qint64 minVisibleTime = qMax(qint64(0), maxEnd - timeRange);

    for (const auto &series : m_series) {
        if (series.points.size() < 2)
            continue;
        drawSeriesInto(painter, series, rect, timeRange, minVisibleTime, true);
    }
}

void RealTimeChart::drawSeriesInto(QPainter* painter, const Series& series, const QRectF& rect,
                                   qint64 timeRange, qint64 minVisibleTime, bool decorate)
{
    QPainterPath path;
    bool first = true;

    for (const auto& point : series.points) {
        if (point.timestamp < minVisibleTime) continue;
        qint64 relativeTime = point.timestamp - minVisibleTime;
        QPointF screenPos = dataToScreen(rect, relativeTime, point.value, timeRange, series.rightAxis);
        if (first) {
            path.moveTo(screenPos);
            first = false;
        } else {
            path.lineTo(screenPos);
        }
    }

    if (first)
        return;

    // 填充区域
    QPainterPath fillPath = path;
    QPointF lastPoint = fillPath.currentPosition();
    fillPath.lineTo(lastPoint.x(), rect.bottom());
    fillPath.lineTo(rect.left(), rect.bottom());
    fillPath.closeSubpath();
    QColor fillColor = series.color;
    fillColor.setAlpha(30);
    painter->fillPath(fillPath, fillColor);

    // 曲线
    painter->setPen(QPen(series.color, m_lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(path);

    if (!decorate)
        return;

    // 最新点高亮 + 当前值
    const auto& lp = series.points.last();
    qint64 rel = lp.timestamp - minVisibleTime;
    QPointF sp = dataToScreen(rect, rel, lp.value, timeRange, series.rightAxis);

    painter->setPen(Qt::NoPen);
    painter->setBrush(series.color);
    painter->drawEllipse(sp, 5, 5);
    painter->setBrush(Qt::white);
    painter->drawEllipse(sp, 3, 3);

    painter->setPen(Qt::black);
    painter->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    QString valueStr = QString::number(lp.value, 'f', 2);
    QRectF textRect(sp.x() + 10, sp.y() - 20, 100, 20);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, valueStr);
}

RealTimeChart::Series& RealTimeChart::ensureSeries(int index)
{
    while (m_series.size() <= index) {
        Series s;
        s.name = QStringLiteral("Series %1").arg(m_series.size());
        s.color = m_lineColor;
        s.rightAxis = false;
        m_series.append(s);
    }
    return m_series[index];
}

QPointF RealTimeChart::dataToScreen(const QRectF& rect, qint64 timestamp, double value,
                                    qint64 timeRange, bool rightAxis)
{
    if (timeRange <= 0) timeRange = 60000;  // 默认60秒

    // X坐标：基于相对时间（0 ~ timeRange）
    double x = rect.left() + timestamp * rect.width() / timeRange;
    x = qBound(rect.left(), x, rect.right());

    // Y坐标：基于数值（按左右轴分别映射）
    double lo = rightAxis ? m_rightYMin : m_yMin;
    double hi = rightAxis ? m_rightYMax : m_yMax;
    double yRange = hi - lo;
    if (yRange <= 0) yRange = 1;
    double y = rect.bottom() - (value - lo) * rect.height() / yRange;

    return QPointF(x, y);
}

void RealTimeChart::calculateYRange()
{
    bool leftHas = false, rightHas = false;
    double leftMin = 0, leftMax = 0, rightMin = 0, rightMax = 0;

    for (const auto &s : m_series) {
        if (s.points.isEmpty()) continue;
        double mn = s.points.first().value, mx = s.points.first().value;
        for (const auto &p : s.points) {
            if (p.value < mn) mn = p.value;
            if (p.value > mx) mx = p.value;
        }
        if (s.rightAxis) {
            if (!rightHas) { rightMin = mn; rightMax = mx; rightHas = true; }
            else { if (mn < rightMin) rightMin = mn; if (mx > rightMax) rightMax = mx; }
        } else {
            if (!leftHas) { leftMin = mn; leftMax = mx; leftHas = true; }
            else { if (mn < leftMin) leftMin = mn; if (mx > leftMax) leftMax = mx; }
        }
    }

    // 添加边距并更新左轴范围
    if (leftHas) {
        double range = leftMax - leftMin;
        if (range < 0.001) range = 1.0;
        double newMin = leftMin - range * 0.1;
        double newMax = leftMax + range * 0.1;
        if (newMin != m_yMin || newMax != m_yMax) {
            m_yMin = newMin;
            m_yMax = newMax;
            emit yRangeChanged();
        }
    }

    // 右轴范围
    if (rightHas) {
        double range = rightMax - rightMin;
        if (range < 0.001) range = 1.0;
        double newMin = rightMin - range * 0.1;
        double newMax = rightMax + range * 0.1;
        if (newMin != m_rightYMin || newMax != m_rightYMax) {
            m_rightYMin = newMin;
            m_rightYMax = newMax;
            emit rightYRangeChanged();
        }
    }
}

QString RealTimeChart::formatTime(qint64 timestamp)
{
    // 将毫秒转换为 mm:ss.ms 格式
    return formatElapsedTime(timestamp);
}

QString RealTimeChart::formatElapsedTime(qint64 ms)
{
    int totalSeconds = ms / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    int milliseconds = (ms % 1000) / 100;  // 只保留一位小数
    
    return QString("%1:%2.%3")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(milliseconds);
}

void RealTimeChart::addValue(double value)
{
    addSeriesValueAt(0, value, QDateTime::currentMSecsSinceEpoch());
}

void RealTimeChart::addValueWithTimestamp(double value, qint64 timestamp)
{
    addSeriesValueAt(0, value, timestamp);
}

void RealTimeChart::addSeriesValue(int series, double value)
{
    addSeriesValueAt(series, value, QDateTime::currentMSecsSinceEpoch());
}

void RealTimeChart::addSeriesValueAt(int series, double value, qint64 timestamp)
{
    if (m_isPaused) return;

    {
        QMutexLocker locker(&m_dataMutex);
        ensureSeries(series);
        m_series[series].points.enqueue({timestamp, value});
        while (m_series[series].points.size() > m_maxPoints)
            m_series[series].points.dequeue();
    }

    m_cacheValid = false;
    emit dataAdded(value);
    update();
}

int RealTimeChart::addSeries(const QString &name, const QColor &color, bool rightAxis)
{
    QMutexLocker locker(&m_dataMutex);

    // Ensure the left-axis pressure placeholder (series 0) exists first, so the
    // added series land at index 1..N regardless of sampling order.
    if (m_series.isEmpty()) {
        Series pressure;
        pressure.name = QStringLiteral("Pressure");
        pressure.color = m_lineColor;
        pressure.rightAxis = false;
        m_series.append(pressure);
    }

    Series s;
    s.name = name;
    s.color = color;
    s.rightAxis = rightAxis;
    m_series.append(s);
    int idx = m_series.size() - 1;
    locker.unlock();
    emit seriesChanged();
    update();
    return idx;
}

void RealTimeChart::clearSeries(int series)
{
    {
        QMutexLocker locker(&m_dataMutex);
        if (series < 0 || series >= m_series.size())
            return;
        m_series[series].points.clear();
    }
    m_cacheValid = false;
    update();
}

void RealTimeChart::clear()
{
    {
        QMutexLocker locker(&m_dataMutex);
        for (auto &s : m_series)
            s.points.clear();
    }

    // 重置缓存
    m_cacheValid = false;
    m_lastDrawnIndex = -1;
    m_cachedImage = QImage();

    update();
}

void RealTimeChart::setMaxPoints(int maxPoints)
{
    if (maxPoints < 10) maxPoints = 10;
    if (maxPoints > 10000) maxPoints = 10000;
    
    if (m_maxPoints != maxPoints) {
        m_maxPoints = maxPoints;
        emit maxPointsChanged();

        // 清理多余数据（每序列）
        QMutexLocker locker(&m_dataMutex);
        for (auto &s : m_series)
            while (s.points.size() > m_maxPoints)
                s.points.dequeue();
        locker.unlock();

        update();
    }
}

void RealTimeChart::setYMin(double yMin)
{
    if (!qFuzzyCompare(m_yMin, yMin)) {
        m_yMin = yMin;
        m_cacheValid = false; // 缓存无效
        emit yRangeChanged();
        update();
    }
}

void RealTimeChart::setYMax(double yMax)
{
    if (!qFuzzyCompare(m_yMax, yMax)) {
        m_yMax = yMax;
        m_cacheValid = false; // 缓存无效
        emit yRangeChanged();
        update();
    }
}

void RealTimeChart::setRightYMin(double yMin)
{
    if (!qFuzzyCompare(m_rightYMin, yMin)) {
        m_rightYMin = yMin;
        m_cacheValid = false;
        emit rightYRangeChanged();
        update();
    }
}

void RealTimeChart::setRightYMax(double yMax)
{
    if (!qFuzzyCompare(m_rightYMax, yMax)) {
        m_rightYMax = yMax;
        m_cacheValid = false;
        emit rightYRangeChanged();
        update();
    }
}

void RealTimeChart::setAutoScale(bool autoScale)
{
    if (m_autoScale != autoScale) {
        m_autoScale = autoScale;
        m_cacheValid = false; // 缓存无效
        emit autoScaleChanged();
        update();
    }
}

void RealTimeChart::setTitle(const QString& title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
        update();
    }
}

void RealTimeChart::setYAxisLabel(const QString& label)
{
    if (m_yAxisLabel != label) {
        m_yAxisLabel = label;
        emit yAxisLabelChanged();
        update();
    }
}

void RealTimeChart::setRightYAxisLabel(const QString& label)
{
    if (m_rightYAxisLabel != label) {
        m_rightYAxisLabel = label;
        emit rightYAxisLabelChanged();
        update();
    }
}

void RealTimeChart::setShowGrid(bool show)
{
    if (m_showGrid != show) {
        m_showGrid = show;
        emit showGridChanged();
        update();
    }
}

void RealTimeChart::setLineWidth(int width)
{
    if (width < 1) width = 1;
    if (width > 10) width = 10;
    
    if (m_lineWidth != width) {
        m_lineWidth = width;
        emit lineWidthChanged();
        update();
    }
}

void RealTimeChart::setLineColor(const QColor& color)
{
    if (m_lineColor != color) {
        m_lineColor = color;
        emit lineColorChanged();
        update();
    }
}

void RealTimeChart::setGridColor(const QColor& color)
{
    if (m_gridColor != color) {
        m_gridColor = color;
        emit gridColorChanged();
        update();
    }
}

void RealTimeChart::setIsPaused(bool paused)
{
    if (m_isPaused != paused) {
        m_isPaused = paused;
        emit isPausedChanged();
    }
}

void RealTimeChart::setTimeRange(qint64 rangeMs)
{
    if (rangeMs > 0) {
        m_timeRangeMs = rangeMs;
    } else {
        m_timeRangeMs = 60000;  // 默认60秒
    }
    m_cacheValid = false; // 时间范围变化，缓存无效
    update();
}

void RealTimeChart::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    m_cacheValid = false; // 尺寸变化，缓存无效
    update();
}

// 优化的绘制函数 - 使用缓存减少重绘开销
void RealTimeChart::drawCurveOptimized(QPainter* painter, const QRectF& rect)
{
    QMutexLocker locker(&m_dataMutex);

    if (m_autoScale)
        calculateYRange();

    // 多序列：始终全量重绘（简单且正确）。
    if (m_series.size() != 1) {
        updateCachedImage(rect);
        painter->drawImage(0, 0, m_cachedImage);

        qint64 maxEnd = 0;
        for (const auto &s : m_series)
            if (!s.points.isEmpty() && s.points.last().timestamp > maxEnd)
                maxEnd = s.points.last().timestamp;
        if (maxEnd == 0)
            return;
        qint64 timeRange = qMax(m_timeRangeMs, maxEnd);
        qint64 minVisibleTime = qMax(qint64(0), maxEnd - timeRange);
        for (const auto &series : m_series)
            if (series.points.size() >= 2)
                drawSeriesInto(painter, series, rect, timeRange, minVisibleTime, true);
        return;
    }

    // 单序列增量路径（序列 0）。
    if (m_series.isEmpty() || m_series[0].points.size() < 2)
        return;

    const auto &pts = m_series[0].points;
    bool needFullRedraw = !m_cacheValid ||
                          m_cachedImage.isNull() ||
                          m_cachedImage.size() != QSize(static_cast<int>(width()), static_cast<int>(height())) ||
                          qFuzzyCompare(m_cachedYMin, m_yMin) == false ||
                          qFuzzyCompare(m_cachedYMax, m_yMax) == false ||
                          m_cachedTimeRange != m_timeRangeMs ||
                          m_lastDrawnIndex < 0 ||
                          m_lastDrawnIndex >= pts.size() - 1;

    if (needFullRedraw) {
        updateCachedImage(rect);
    } else {
        // 增量绘制：只绘制新增的点
        QPainter cachePainter(&m_cachedImage);
        cachePainter.setRenderHint(QPainter::Antialiasing, true);

        qint64 endTime = pts.last().timestamp;
        qint64 timeRange = qMax(m_timeRangeMs, endTime);
        qint64 minVisibleTime = qMax(qint64(0), endTime - timeRange);

        for (int i = m_lastDrawnIndex; i < pts.size() - 1; ++i) {
            const auto& p1 = pts[i];
            const auto& p2 = pts[i + 1];
            if (p1.timestamp < minVisibleTime) continue;
            qint64 r1 = p1.timestamp - minVisibleTime;
            qint64 r2 = p2.timestamp - minVisibleTime;
            QPointF s1 = dataToScreen(rect, r1, p1.value, timeRange, false);
            QPointF s2 = dataToScreen(rect, r2, p2.value, timeRange, false);
            cachePainter.setPen(QPen(m_lineColor, m_lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            cachePainter.drawLine(s1, s2);
        }
        m_lastDrawnIndex = pts.size() - 1;
    }

    painter->drawImage(0, 0, m_cachedImage);

    // 最新点（高亮 + 当前值）
    if (!pts.isEmpty()) {
        qint64 endTime = pts.last().timestamp;
        qint64 timeRange = qMax(m_timeRangeMs, endTime);
        qint64 minVisibleTime = qMax(qint64(0), endTime - timeRange);
        const auto& lp = pts.last();
        qint64 rel = lp.timestamp - minVisibleTime;
        QPointF sp = dataToScreen(rect, rel, lp.value, timeRange, false);

        painter->setPen(Qt::NoPen);
        painter->setBrush(m_lineColor);
        painter->drawEllipse(sp, 5, 5);
        painter->setBrush(Qt::white);
        painter->drawEllipse(sp, 3, 3);

        painter->setPen(Qt::black);
        painter->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        QString valueStr = QString::number(lp.value, 'f', 2);
        QRectF textRect(sp.x() + 10, sp.y() - 20, 100, 20);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, valueStr);
    }
}

void RealTimeChart::updateCachedImage(const QRectF& rect)
{
    // 创建或重置缓存图像
    int w = static_cast<int>(width());
    int h = static_cast<int>(height());

    if (m_cachedImage.isNull() || m_cachedImage.size() != QSize(w, h)) {
        m_cachedImage = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    }
    m_cachedImage.fill(Qt::transparent);

    QPainter cachePainter(&m_cachedImage);
    cachePainter.setRenderHint(QPainter::Antialiasing, true);

    // 计算可见范围（跨所有序列的最新时间戳）
    qint64 maxEnd = 0;
    for (const auto &s : m_series)
        if (!s.points.isEmpty() && s.points.last().timestamp > maxEnd)
            maxEnd = s.points.last().timestamp;
    if (maxEnd == 0) {
        m_cacheValid = true;
        return;
    }
    qint64 timeRange = qMax(m_timeRangeMs, maxEnd);
    qint64 minVisibleTime = qMax(qint64(0), maxEnd - timeRange);

    // 绘制所有序列
    for (const auto &series : m_series) {
        if (series.points.size() < 2)
            continue;
        drawSeriesInto(&cachePainter, series, rect, timeRange, minVisibleTime, false);
    }

    // 更新缓存状态
    m_cacheValid = true;
    m_lastDrawnIndex = m_series.isEmpty() ? -1 : static_cast<int>(m_series[0].points.size()) - 1;
    m_cachedYMin = m_yMin;
    m_cachedYMax = m_yMax;
    m_cachedRightYMin = m_rightYMin;
    m_cachedRightYMax = m_rightYMax;
    m_cachedTimeRange = m_timeRangeMs;
}
