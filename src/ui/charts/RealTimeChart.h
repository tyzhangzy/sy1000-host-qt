#pragma once

#include <QQuickPaintedItem>
#include <QObject>
#include <QPainter>
#include <QQueue>
#include <QDateTime>
#include <QTimer>
#include <QMutex>

/**
 * @brief RealTimeChart - 实时曲线绘制组件
 * 
 * 使用 QQuickPaintedItem 实现高性能实时曲线绘制
 * 横坐标：时间（自动滚动）
 * 纵坐标：水压数据
 */
class RealTimeChart : public QQuickPaintedItem
{
    Q_OBJECT
    
    // QML 可访问属性
    Q_PROPERTY(int maxPoints READ maxPoints WRITE setMaxPoints NOTIFY maxPointsChanged)
    Q_PROPERTY(double yMin READ yMin WRITE setYMin NOTIFY yRangeChanged)
    Q_PROPERTY(double yMax READ yMax WRITE setYMax NOTIFY yRangeChanged)
    Q_PROPERTY(bool autoScale READ autoScale WRITE setAutoScale NOTIFY autoScaleChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString yAxisLabel READ yAxisLabel WRITE setYAxisLabel NOTIFY yAxisLabelChanged)
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor NOTIFY lineColorChanged)
    Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor NOTIFY gridColorChanged)
    Q_PROPERTY(bool isPaused READ isPaused WRITE setIsPaused NOTIFY isPausedChanged)

public:
    explicit RealTimeChart(QQuickItem *parent = nullptr);
    ~RealTimeChart();
    
    // QQuickPaintedItem 接口
    void paint(QPainter *painter) override;
    
    // Getters
    int maxPoints() const { return m_maxPoints; }
    double yMin() const { return m_yMin; }
    double yMax() const { return m_yMax; }
    bool autoScale() const { return m_autoScale; }
    QString title() const { return m_title; }
    QString yAxisLabel() const { return m_yAxisLabel; }
    bool showGrid() const { return m_showGrid; }
    int lineWidth() const { return m_lineWidth; }
    QColor lineColor() const { return m_lineColor; }
    QColor gridColor() const { return m_gridColor; }
    bool isPaused() const { return m_isPaused; }

public slots:
    // 添加数据点
    void addValue(double value);
    void addValueWithTimestamp(double value, qint64 timestamp);
    
    // 清除数据
    void clear();
    
    // Setters
    void setMaxPoints(int maxPoints);
    void setYMin(double yMin);
    void setYMax(double yMax);
    void setAutoScale(bool autoScale);
    void setTitle(const QString& title);
    void setYAxisLabel(const QString& label);
    void setShowGrid(bool show);
    void setLineWidth(int width);
    void setLineColor(const QColor& color);
    void setGridColor(const QColor& color);
    void setIsPaused(bool paused);
    
    // 设置固定时间范围（毫秒），0表示自适应
    void setTimeRange(qint64 rangeMs);

signals:
    void maxPointsChanged();
    void yRangeChanged();
    void autoScaleChanged();
    void titleChanged();
    void yAxisLabelChanged();
    void showGridChanged();
    void lineWidthChanged();
    void lineColorChanged();
    void gridColorChanged();
    void isPausedChanged();
    void dataAdded(double value);

protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    // 绘制网格
    void drawGrid(QPainter* painter, const QRectF& rect);
    // 绘制坐标轴
    void drawAxes(QPainter* painter, const QRectF& rect);
    // 绘制曲线
    void drawCurve(QPainter* painter, const QRectF& rect);
    // 优化绘制：使用缓存
    void drawCurveOptimized(QPainter* painter, const QRectF& rect);
    void updateCachedImage(const QRectF& rect);
    // 计算 Y 轴范围
    void calculateYRange();
    // 坐标转换
    QPointF dataToScreen(const QRectF& rect, qint64 timestamp, double value, qint64 timeRange);
    // 格式化时间
    QString formatTime(qint64 timestamp);
    QString formatElapsedTime(qint64 ms);

private:
    // 数据点结构
    struct DataPoint {
        qint64 timestamp;  // 毫秒时间戳
        double value;      // 水压值
    };
    
    QQueue<DataPoint> m_dataPoints;  // 数据队列
    QMutex m_dataMutex;              // 数据保护锁
    
    // 配置参数
    int m_maxPoints = 500;           // 最大数据点数
    double m_yMin = 0.0;             // Y轴最小值
    double m_yMax = 100.0;           // Y轴最大值
    bool m_autoScale = true;         // 自动缩放Y轴
    QString m_title = "Water Pressure";  // 图表标题
    QString m_yAxisLabel = "Pressure (kPa)"; // Y轴标签
    bool m_showGrid = true;          // 显示网格
    int m_lineWidth = 2;             // 曲线宽度
    QColor m_lineColor = QColor("#2196F3");  // 曲线颜色
    QColor m_gridColor = QColor("#E0E0E0");  // 网格颜色
    bool m_isPaused = false;         // 暂停更新
    
    // 时间范围
    qint64 m_timeRangeMs = 60000;    // 默认显示60秒
    
    // 边距
    static constexpr int MARGIN_LEFT = 60;
    static constexpr int MARGIN_RIGHT = 20;
    static constexpr int MARGIN_TOP = 40;
    
    // 绘制缓存（优化大数据量性能）
    QImage m_cachedImage;           // 缓存图像
    bool m_cacheValid = false;      // 缓存是否有效
    int m_lastDrawnIndex = -1;      // 上次绘制的数据点索引
    double m_cachedYMin = 0.0;      // 缓存时的Y轴范围
    double m_cachedYMax = 0.0;
    qint64 m_cachedTimeRange = 0;   // 缓存时的时间范围
    static constexpr int MARGIN_BOTTOM = 40;
};
