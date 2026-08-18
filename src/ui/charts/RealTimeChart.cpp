#include "RealTimeChart.h"
#include <QPainterPath>
#include <QtMath>
#include <QDebug>
#include <QtAlgorithms>

RealTimeChart::RealTimeChart(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    // 启用抗锯齿
    setAntialiasing(true);
    setRenderTarget(QQuickPaintedItem::RenderTarget::FramebufferObject);
    
    // 设置默认大小
    setImplicitWidth(800);
    setImplicitHeight(400);
}

RealTimeChart::~RealTimeChart() = default;

void RealTimeChart::paint(QPainter *painter)
{
    if (!painter) return;
    
    // 启用抗锯齿
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    
    // 填充背景
    painter->fillRect(boundingRect(), Qt::white);
    
    // 计算绘图区域
    QRectF plotRect(
        MARGIN_LEFT,
        MARGIN_TOP,
        width() - MARGIN_LEFT - MARGIN_RIGHT,
        height() - MARGIN_TOP - MARGIN_BOTTOM
    );
    
    // 绘制网格
    if (m_showGrid) {
        drawGrid(painter, plotRect);
    }
    
    // 绘制坐标轴
    drawAxes(painter, plotRect);
    
    // 绘制曲线（使用优化版本）
    if (m_dataPoints.size() > 200) {
        // 大数据量时使用缓存优化
        drawCurveOptimized(painter, plotRect);
    } else {
        // 小数据量时使用原始绘制
        drawCurve(painter, plotRect);
    }
    
    // 绘制标题
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
    // Y轴
    painter->drawLine(QPointF(rect.left(), rect.top()), QPointF(rect.left(), rect.bottom()));
    
    // Y轴刻度和标签
    painter->setPen(QPen(Qt::black, 1));
    for (int i = 0; i <= 8; ++i) {
        double ratio = 1.0 - i / 8.0;  // 从上到下
        double value = m_yMin + (m_yMax - m_yMin) * ratio;
        double y = rect.top() + rect.height() * i / 8.0;
        
        // 刻度线
        painter->drawLine(QPointF(rect.left() - 5, y), QPointF(rect.left(), y));
        
        // 标签
        QString label = QString::number(value, 'f', 1);
        QRectF textRect(rect.left() - 55, y - 10, 50, 20);
        painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }
    
    // Y轴单位标签
    if (!m_yAxisLabel.isEmpty()) {
        painter->save();
        painter->translate(15, rect.center().y());
        painter->rotate(-90);
        painter->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        painter->drawText(QRectF(-50, -10, 100, 20), Qt::AlignCenter, m_yAxisLabel);
        painter->restore();
    }
    
    // X轴时间标签
    if (m_dataPoints.isEmpty()) return;
    
    qint64 startTime = m_dataPoints.first().timestamp;
    qint64 endTime = m_dataPoints.last().timestamp;
    qint64 timeRange = qMax(m_timeRangeMs, endTime - startTime);
    
    painter->setFont(QFont("Microsoft YaHei", 8));
    
    // 计算时间范围对应的分隔数
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
    
    if (m_dataPoints.size() < 2) return;
    
    // 自动计算Y轴范围
    if (m_autoScale) {
        calculateYRange();
    }
    
    // 创建绘制路径
    QPainterPath path;
    bool first = true;
    
    // 使用相对时间（从0开始）
    qint64 startTime = 0;  // 第一个点为0
    qint64 endTime = m_dataPoints.last().timestamp;
    qint64 timeRange = qMax(m_timeRangeMs, endTime);
    
    // 计算可见范围
    qint64 minVisibleTime = qMax(qint64(0), endTime - timeRange);
    
    for (const auto& point : m_dataPoints) {
        if (point.timestamp < minVisibleTime) continue;
        
        // 转换为相对时间（减去最小可见时间）
        qint64 relativeTime = point.timestamp - minVisibleTime;
        QPointF screenPos = dataToScreen(rect, relativeTime, point.value, timeRange);
        
        if (first) {
            path.moveTo(screenPos);
            first = false;
        } else {
            path.lineTo(screenPos);
        }
    }
    
    // 绘制填充区域（可选）
    QPainterPath fillPath = path;
    if (!first) {
        QPointF lastPoint = fillPath.currentPosition();
        fillPath.lineTo(lastPoint.x(), rect.bottom());
        fillPath.lineTo(rect.left(), rect.bottom());
        fillPath.closeSubpath();
        
        QColor fillColor = m_lineColor;
        fillColor.setAlpha(30);
        painter->fillPath(fillPath, fillColor);
    }
    
    // 绘制曲线
    painter->setPen(QPen(m_lineColor, m_lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(path);
    
    // 绘制最新点（高亮）
    if (!m_dataPoints.isEmpty()) {
        const auto& lastPoint = m_dataPoints.last();
        qint64 relativeTime = lastPoint.timestamp - minVisibleTime;
        QPointF screenPos = dataToScreen(rect, relativeTime, lastPoint.value, timeRange);
        
        // 外圈
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_lineColor);
        painter->drawEllipse(screenPos, 5, 5);
        
        // 内圈（白色）
        painter->setBrush(Qt::white);
        painter->drawEllipse(screenPos, 3, 3);
        
        // 显示当前值
        painter->setPen(Qt::black);
        painter->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        QString valueStr = QString::number(lastPoint.value, 'f', 2);
        QRectF textRect(screenPos.x() + 10, screenPos.y() - 20, 100, 20);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, valueStr);
        
        // 显示数据点数量（左上角）
        painter->setPen(QColor("#1976d2"));
        painter->setFont(QFont("Microsoft YaHei", 11, QFont::Bold));
        QString countStr = QString("Points: %1").arg(m_dataPoints.size());
        QRectF countRect(rect.left() + 10, rect.top() + 5, 120, 25);
        painter->drawText(countRect, Qt::AlignLeft | Qt::AlignVCenter, countStr);
    }
}

QPointF RealTimeChart::dataToScreen(const QRectF& rect, qint64 timestamp, double value, qint64 timeRange)
{
    if (timeRange <= 0) timeRange = 60000;  // 默认60秒
    
    // X坐标：基于相对时间（0 ~ timeRange）
    double x = rect.left() + timestamp * rect.width() / timeRange;
    
    // 限制在显示范围内
    x = qBound(rect.left(), x, rect.right());
    
    // Y坐标：基于数值
    double yRange = m_yMax - m_yMin;
    if (yRange <= 0) yRange = 1;
    double y = rect.bottom() - (value - m_yMin) * rect.height() / yRange;
    
    return QPointF(x, y);
}

void RealTimeChart::calculateYRange()
{
    if (m_dataPoints.isEmpty()) return;
    
    double minVal = m_dataPoints.first().value;
    double maxVal = m_dataPoints.first().value;
    
    for (const auto& point : m_dataPoints) {
        if (point.value < minVal) minVal = point.value;
        if (point.value > maxVal) maxVal = point.value;
    }
    
    // 添加边距
    double range = maxVal - minVal;
    if (range < 0.001) range = 1.0;
    
    double newMin = minVal - range * 0.1;
    double newMax = maxVal + range * 0.1;
    
    if (newMin != m_yMin || newMax != m_yMax) {
        m_yMin = newMin;
        m_yMax = newMax;
        emit yRangeChanged();
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
    addValueWithTimestamp(value, QDateTime::currentMSecsSinceEpoch());
}

void RealTimeChart::addValueWithTimestamp(double value, qint64 timestamp)
{
    if (m_isPaused) return;
    
    QMutexLocker locker(&m_dataMutex);
    
    // 添加数据点
    m_dataPoints.enqueue({timestamp, value});
    
    // 限制数据点数量
    while (m_dataPoints.size() > m_maxPoints) {
        m_dataPoints.dequeue();
    }
    
    locker.unlock();
    
    emit dataAdded(value);
    
    // 触发重绘
    update();
}

void RealTimeChart::clear()
{
    QMutexLocker locker(&m_dataMutex);
    m_dataPoints.clear();
    locker.unlock();
    
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
        
        // 清理多余数据
        QMutexLocker locker(&m_dataMutex);
        while (m_dataPoints.size() > m_maxPoints) {
            m_dataPoints.dequeue();
        }
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
    
    if (m_dataPoints.size() < 2) return;
    
    // 自动计算Y轴范围
    if (m_autoScale) {
        calculateYRange();
    }
    
    // 检查是否需要重新生成缓存
    bool needFullRedraw = !m_cacheValid || 
                          m_cachedImage.isNull() ||
                          m_cachedImage.size() != QSize(static_cast<int>(width()), static_cast<int>(height())) ||
                          qFuzzyCompare(m_cachedYMin, m_yMin) == false ||
                          qFuzzyCompare(m_cachedYMax, m_yMax) == false ||
                          m_cachedTimeRange != m_timeRangeMs ||
                          m_lastDrawnIndex < 0 ||
                          m_lastDrawnIndex >= m_dataPoints.size() - 1;
    
    if (needFullRedraw) {
        // 完整重绘
        updateCachedImage(rect);
    } else {
        // 增量绘制：只绘制新增的点
        QPainter cachePainter(&m_cachedImage);
        cachePainter.setRenderHint(QPainter::Antialiasing, true);
        
        qint64 endTime = m_dataPoints.last().timestamp;
        qint64 timeRange = qMax(m_timeRangeMs, endTime);
        qint64 minVisibleTime = qMax(qint64(0), endTime - timeRange);
        
        // 绘制从上一个点到新点的线段
        for (int i = m_lastDrawnIndex; i < m_dataPoints.size() - 1; ++i) {
            const auto& point1 = m_dataPoints[i];
            const auto& point2 = m_dataPoints[i + 1];
            
            if (point1.timestamp < minVisibleTime) continue;
            
            qint64 relTime1 = point1.timestamp - minVisibleTime;
            qint64 relTime2 = point2.timestamp - minVisibleTime;
            
            QPointF screenPos1 = dataToScreen(rect, relTime1, point1.value, timeRange);
            QPointF screenPos2 = dataToScreen(rect, relTime2, point2.value, timeRange);
            
            cachePainter.setPen(QPen(m_lineColor, m_lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            cachePainter.drawLine(screenPos1, screenPos2);
        }
        
        m_lastDrawnIndex = m_dataPoints.size() - 1;
    }
    
    // 绘制缓存图像
    painter->drawImage(0, 0, m_cachedImage);
    
    // 绘制最新点（高亮）- 这部分每次都重绘，不需要缓存
    if (!m_dataPoints.isEmpty()) {
        qint64 endTime = m_dataPoints.last().timestamp;
        qint64 timeRange = qMax(m_timeRangeMs, endTime);
        qint64 minVisibleTime = qMax(qint64(0), endTime - timeRange);
        
        const auto& lastPoint = m_dataPoints.last();
        qint64 relativeTime = lastPoint.timestamp - minVisibleTime;
        QPointF screenPos = dataToScreen(rect, relativeTime, lastPoint.value, timeRange);
        
        // 外圈
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_lineColor);
        painter->drawEllipse(screenPos, 5, 5);
        
        // 内圈（白色）
        painter->setBrush(Qt::white);
        painter->drawEllipse(screenPos, 3, 3);
        
        // 显示当前值
        painter->setPen(Qt::black);
        painter->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        QString valueStr = QString::number(lastPoint.value, 'f', 2);
        QRectF textRect(screenPos.x() + 10, screenPos.y() - 20, 100, 20);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, valueStr);
        
        // 显示数据点数量（左上角）
        painter->setPen(QColor("#1976d2"));
        painter->setFont(QFont("Microsoft YaHei", 11, QFont::Bold));
        QString countStr = QString("Points: %1").arg(m_dataPoints.size());
        QRectF countRect(rect.left() + 10, rect.top() + 5, 120, 25);
        painter->drawText(countRect, Qt::AlignLeft | Qt::AlignVCenter, countStr);
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
    
    // 清空缓存（保留alpha通道）
    m_cachedImage.fill(Qt::transparent);
    
    QPainter cachePainter(&m_cachedImage);
    cachePainter.setRenderHint(QPainter::Antialiasing, true);
    
    // 计算可见范围
    qint64 endTime = m_dataPoints.last().timestamp;
    qint64 timeRange = qMax(m_timeRangeMs, endTime);
    qint64 minVisibleTime = qMax(qint64(0), endTime - timeRange);
    
    // 绘制所有线段
    QPainterPath path;
    bool first = true;
    
    for (const auto& point : m_dataPoints) {
        if (point.timestamp < minVisibleTime) continue;
        
        qint64 relativeTime = point.timestamp - minVisibleTime;
        QPointF screenPos = dataToScreen(rect, relativeTime, point.value, timeRange);
        
        if (first) {
            path.moveTo(screenPos);
            first = false;
        } else {
            path.lineTo(screenPos);
        }
    }
    
    // 绘制填充区域
    QPainterPath fillPath = path;
    if (!first) {
        QPointF lastPoint = fillPath.currentPosition();
        fillPath.lineTo(lastPoint.x(), rect.bottom());
        fillPath.lineTo(rect.left(), rect.bottom());
        fillPath.closeSubpath();
        
        QColor fillColor = m_lineColor;
        fillColor.setAlpha(30);
        cachePainter.fillPath(fillPath, fillColor);
    }
    
    // 绘制曲线
    cachePainter.setPen(QPen(m_lineColor, m_lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    cachePainter.drawPath(path);
    
    // 更新缓存状态
    m_cacheValid = true;
    m_lastDrawnIndex = m_dataPoints.size() - 1;
    m_cachedYMin = m_yMin;
    m_cachedYMax = m_yMax;
    m_cachedTimeRange = m_timeRangeMs;
}
