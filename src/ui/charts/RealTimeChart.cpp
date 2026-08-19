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

    // With the FramebufferObject render target paint() executes on the render
    // thread, so all shared state (series data, config, cache image) is guarded
    // by m_dataMutex for the whole paint pass (M10).
    QMutexLocker locker(&m_dataMutex);

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
    for (const auto &s : m_series)
        total += s.points.size();
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

    // X轴时间标签（m_series 已在 paint() 的 m_dataMutex 保护下访问，M10）
    qint64 startTime = 0, endTime = 0;
    if (m_series.isEmpty() || m_series[0].points.isEmpty())
        return;
    startTime = m_series[0].points.first().timestamp;
    endTime = m_series[0].points.last().timestamp;
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
    // Lock is already held by paint() (M10).
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
        m_yMin = leftMin - range * 0.1;
        m_yMax = leftMax + range * 0.1;
    }

    // 右轴范围
    if (rightHas) {
        double range = rightMax - rightMin;
        if (range < 0.001) range = 1.0;
        m_rightYMin = rightMin - range * 0.1;
        m_rightYMax = rightMax + range * 0.1;
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
    {
        QMutexLocker locker(&m_dataMutex);
        if (m_isPaused)
            return;
        ensureSeries(series);
        m_series[series].points.enqueue({timestamp, value});
        while (m_series[series].points.size() > m_maxPoints)
            m_series[series].points.dequeue();
    }

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
    update();
}

void RealTimeChart::clear()
{
    {
        QMutexLocker locker(&m_dataMutex);
        for (auto &s : m_series)
            s.points.clear();
        m_cachedImage = QImage(); // 缓存与数据同锁重置（M10）
    }
    update();
}

void RealTimeChart::setMaxPoints(int maxPoints)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        if (maxPoints < 10) maxPoints = 10;
        if (maxPoints > 10000) maxPoints = 10000;

        if (m_maxPoints != maxPoints) {
            m_maxPoints = maxPoints;
            changed = true;
            // 清理多余数据（每序列）
            for (auto &s : m_series)
                while (s.points.size() > m_maxPoints)
                    s.points.dequeue();
        }
    }

    if (changed) {
        emit maxPointsChanged();
        update();
    }
}

void RealTimeChart::setYMin(double yMin)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = !qFuzzyCompare(m_yMin, yMin);
        if (changed)
            m_yMin = yMin;
    }
    if (changed) {
        emit yRangeChanged();
        update();
    }
}

void RealTimeChart::setYMax(double yMax)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = !qFuzzyCompare(m_yMax, yMax);
        if (changed)
            m_yMax = yMax;
    }
    if (changed) {
        emit yRangeChanged();
        update();
    }
}

void RealTimeChart::setRightYMin(double yMin)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = !qFuzzyCompare(m_rightYMin, yMin);
        if (changed)
            m_rightYMin = yMin;
    }
    if (changed) {
        emit rightYRangeChanged();
        update();
    }
}

void RealTimeChart::setRightYMax(double yMax)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = !qFuzzyCompare(m_rightYMax, yMax);
        if (changed)
            m_rightYMax = yMax;
    }
    if (changed) {
        emit rightYRangeChanged();
        update();
    }
}

void RealTimeChart::setAutoScale(bool autoScale)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_autoScale != autoScale);
        if (changed)
            m_autoScale = autoScale;
    }
    if (changed) {
        emit autoScaleChanged();
        update();
    }
}

void RealTimeChart::setTitle(const QString& title)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_title != title);
        if (changed)
            m_title = title;
    }
    if (changed) {
        emit titleChanged();
        update();
    }
}

void RealTimeChart::setYAxisLabel(const QString& label)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_yAxisLabel != label);
        if (changed)
            m_yAxisLabel = label;
    }
    if (changed) {
        emit yAxisLabelChanged();
        update();
    }
}

void RealTimeChart::setRightYAxisLabel(const QString& label)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_rightYAxisLabel != label);
        if (changed)
            m_rightYAxisLabel = label;
    }
    if (changed) {
        emit rightYAxisLabelChanged();
        update();
    }
}

void RealTimeChart::setShowGrid(bool show)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_showGrid != show);
        if (changed)
            m_showGrid = show;
    }
    if (changed) {
        emit showGridChanged();
        update();
    }
}

void RealTimeChart::setLineWidth(int width)
{
    if (width < 1) width = 1;
    if (width > 10) width = 10;

    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_lineWidth != width);
        if (changed)
            m_lineWidth = width;
    }
    if (changed) {
        emit lineWidthChanged();
        update();
    }
}

void RealTimeChart::setLineColor(const QColor& color)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_lineColor != color);
        if (changed)
            m_lineColor = color;
    }
    if (changed) {
        emit lineColorChanged();
        update();
    }
}

void RealTimeChart::setGridColor(const QColor& color)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_gridColor != color);
        if (changed)
            m_gridColor = color;
    }
    if (changed) {
        emit gridColorChanged();
        update();
    }
}

void RealTimeChart::setIsPaused(bool paused)
{
    bool changed = false;
    {
        QMutexLocker locker(&m_dataMutex);
        changed = (m_isPaused != paused);
        if (changed)
            m_isPaused = paused;
    }
    if (changed)
        emit isPausedChanged();
}

void RealTimeChart::setTimeRange(qint64 rangeMs)
{
    {
        QMutexLocker locker(&m_dataMutex);
        m_timeRangeMs = (rangeMs > 0) ? rangeMs : 60000; // 默认60秒
    }
    update();
}

void RealTimeChart::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    // 尺寸变化：缓存图像大小与 width()/height() 不匹配时会在下一次
    // updateCachedImage() 自动重建，无需额外标志（M10）。
    update();
}

// 优化的绘制函数 - 使用缓存减少重绘开销（M10）。
// 单序列的"增量绘制"分支已删除：它因每次 addSeriesValueAt() 使缓存失效而
// 永远不会被走到（死代码），且包含陈旧像素未清除的潜在 bug。现在无论序列
// 数量一律重建缓存图并一次性贴出，正确且简单。
void RealTimeChart::drawCurveOptimized(QPainter* painter, const QRectF& rect)
{
    // Lock is already held by paint().
    if (m_autoScale)
        calculateYRange();

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

    // Decorate each visible series (latest-point highlight + value label).
    for (const auto &series : m_series)
        if (series.points.size() >= 2)
            drawSeriesInto(painter, series, rect, timeRange, minVisibleTime, true);
}

void RealTimeChart::updateCachedImage(const QRectF& rect)
{
    // 创建或重置缓存图像（在 paint() 的 m_dataMutex 保护下执行）
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
    if (maxEnd == 0)
        return;
    qint64 timeRange = qMax(m_timeRangeMs, maxEnd);
    qint64 minVisibleTime = qMax(qint64(0), maxEnd - timeRange);

    // 绘制所有序列
    for (const auto &series : m_series) {
        if (series.points.size() < 2)
            continue;
        drawSeriesInto(&cachePainter, series, rect, timeRange, minVisibleTime, false);
    }
}
