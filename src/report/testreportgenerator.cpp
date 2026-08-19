#include "report/testreportgenerator.h"

#include <QBuffer>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QPrinter>
#include <QTextDocument>
#include <QTextStream>

namespace sy1000 {

namespace {

const char *resultLabel(TestResultStatus s)
{
    switch (s) {
    case TestResultStatus::Passed: return "合格 (Passed)";
    case TestResultStatus::Failed: return "不合格 (Failed)";
    case TestResultStatus::Qualified: return "合格 (Qualified)";
    case TestResultStatus::ToRepair: return "待维修 (To Repair)";
    case TestResultStatus::ToReplace: return "待更换 (To Replace)";
    case TestResultStatus::Scrapped: return "报废 (Scrapped)";
    case TestResultStatus::InProgress: return "进行中 (In Progress)";
    case TestResultStatus::NotTested:
    default: return "未测试 (Not Tested)";
    }
}

const char *inspectionLabel(InspectionResult r)
{
    switch (r) {
    case InspectionResult::ToRepair: return "待维修";
    case InspectionResult::ToReplace: return "待更换";
    case InspectionResult::Scrapped: return "报废";
    case InspectionResult::Qualified:
    default: return "合格";
    }
}

QString dateTimeString(const DateTime &t)
{
    const std::time_t secs = std::chrono::system_clock::to_time_t(t);
    return QDateTime::fromSecsSinceEpoch(secs).toString(QStringLiteral("yyyy-MM-dd HH:mm"));
}

QString esc(const QString &s)
{
    QString out = s;
    out.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;");
    return out;
}

// Render a simple pressure/time chart (if there is enough data) and return a
// data-URI PNG, or empty string when there is nothing to draw.
QString chartDataUri(const HydroStaticTestData &h)
{
    if (h.pressureWeightData.size() < 2)
        return QString();

    double pmin = h.pressureWeightData.front().pressure, pmax = pmin;
    for (const auto &p : h.pressureWeightData) {
        if (p.pressure < pmin) pmin = p.pressure;
        if (p.pressure > pmax) pmax = p.pressure;
    }
    if (pmax - pmin < 0.001) pmax = pmin + 1.0;

    const int w = 720, hh = 260, ml = 50, mr = 20, mt = 20, mb = 30;
    QImage img(w, hh, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRectF plot(ml, mt, w - ml - mr, hh - mt - mb);

    // grid + axes
    p.setPen(QPen(QColor("#E0E0E0"), 1, Qt::DotLine));
    for (int i = 0; i <= 8; ++i) {
        const double y = plot.top() + plot.height() * i / 8.0;
        p.drawLine(QPointF(plot.left(), y), QPointF(plot.right(), y));
    }
    p.setPen(QPen(Qt::black, 1.5));
    p.drawLine(plot.bottomLeft(), plot.bottomRight());
    p.drawLine(plot.topLeft(), plot.bottomLeft());
    p.drawText(QRectF(0, 0, w, mt), Qt::AlignCenter, QStringLiteral("压力曲线 (MPa)"));

    // y labels
    p.setFont(QFont("Microsoft YaHei", 8));
    for (int i = 0; i <= 4; ++i) {
        const double ratio = 1.0 - i / 4.0;
        const double v = pmin + (pmax - pmin) * ratio;
        const double y = plot.top() + plot.height() * ratio;
        p.drawText(QRectF(0, y - 10, ml - 6, 20), Qt::AlignRight | Qt::AlignVCenter,
                   QString::number(v, 'f', 1));
    }

    // curve
    const auto epochMs = [](const DateTime &t) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count();
    };
    QPainterPath path;
    bool first = true;
    const qint64 t0 = epochMs(h.pressureWeightData.front().timestamp);
    const qint64 t1 = epochMs(h.pressureWeightData.back().timestamp);
    const qint64 range = std::max<qint64>(1, t1 - t0);
    for (const auto &pt : h.pressureWeightData) {
        const double x = plot.left() + (epochMs(pt.timestamp) - t0) * plot.width() / double(range);
        const double y = plot.bottom() - (pt.pressure - pmin) * plot.height() / (pmax - pmin);
        if (first) { path.moveTo(x, y); first = false; }
        else path.lineTo(x, y);
    }
    p.setPen(QPen(QColor("#3f51b5"), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPath(path);
    p.end();

    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "PNG");
    return QStringLiteral("data:image/png;base64,") + QString::fromLatin1(ba.toBase64());
}

} // namespace (anonymous)


QString TestReportGenerator::buildHtml(const UnifiedTestResult &r)
{
    const auto &s = r.sample;
    const auto &h = s.hydroStaticTest;
    const auto &insp = s.appearanceInspection;
    const auto &ts = r.testStandard;

    QString html;
    QTextStream o(&html);
    o << "<html><body style='font-family:\"Microsoft YaHei\";font-size:11pt'>"
      << "<h2 style='text-align:center'>水压测试报告</h2>"
      << "<table width='100%' cellspacing='4'><tr>"
      << "<td><b>流水号:</b> " << esc(QString::fromStdString(r.testSerialNo)) << "</td>"
      << "<td><b>日期:</b> " << dateTimeString(r.testDate) << "</td>"
      << "</tr><tr>"
      << "<td><b>检验员:</b> " << esc(QString::fromStdString(r.testerName)) << "</td>"
      << "<td><b>单位:</b> " << esc(QString::fromStdString(r.testerCompany)) << "</td>"
      << "</tr></table><hr/>"

      << "<h3>试验标准</h3><table border='1' cellspacing='0' cellpadding='4' width='100%'>"
      << "<tr><td>名称</td><td>" << esc(QString::fromStdString(ts.standardName)) << "</td>"
      << "<td>工作压力 (MPa)</td><td>" << ts.workingPressure << "</td></tr>"
      << "<tr><td>试验压力 (MPa)</td><td>" << ts.testingPressure << "</td>"
      << "<td>残余变形率限值 (%)</td><td>" << ts.residualDeformationRate << "</td></tr></table>"

      << "<h3>样品信息</h3><table border='1' cellspacing='0' cellpadding='4' width='100%'>"
      << "<tr><td>型号</td><td>" << esc(QString::fromStdString(s.sampleModel)) << "</td>"
      << "<td>制造厂商</td><td>" << esc(QString::fromStdString(s.manufacturer)) << "</td></tr>"
      << "<tr><td>产品编号</td><td>" << esc(QString::fromStdString(s.serialNo)) << "</td>"
      << "<td>容积 (L)</td><td>" << s.volume << "</td></tr></table>"

      << "<h3>水压试验数据</h3><table border='1' cellspacing='0' cellpadding='4' width='100%'>"
      << "<tr><th>初始重量</th><th>试验压力重量</th><th>泄压后重量</th></tr>"
      << "<tr><td>" << h.initialWeight << "</td><td>" << h.pressureWeight
      << "</td><td>" << h.finalWeight << "</td></tr>"
      << "<tr><th>全变形量</th><th>残余变形量</th><th>残余变形率 (%)</th></tr>"
      << "<tr><td>" << h.fullDeformation << "</td><td>" << h.residualDeformation
      << "</td><td>" << h.residualDeformationRate << "</td></tr></table>"

      << "<h3>外观检查</h3><table border='1' cellspacing='0' cellpadding='4' width='100%'>"
      << "<tr><th>外观</th><th>内部</th><th>螺纹</th><th>瓶阀</th></tr>"
      << "<tr><td>" << inspectionLabel(insp.externalResult) << "</td>"
      << "<td>" << inspectionLabel(insp.internalResult) << "</td>"
      << "<td>" << inspectionLabel(insp.threadResult) << "</td>"
      << "<td>" << inspectionLabel(insp.valveResult) << "</td></tr></table>"

      << "<h3>总体结果</h3><p style='font-size:14pt'><b>" << resultLabel(s.overallResult)
      << "</b></p>";

    const QString chart = chartDataUri(h);
    if (!chart.isEmpty())
        o << "<p style='text-align:center'><img src='" << chart << "' width='620'/></p>";

    o << "</body></html>";
    return html;
}

bool TestReportGenerator::generatePdf(const UnifiedTestResult &r, QString *outPath,
                                      const QString &baseFolder)
{
    const auto &s = r.sample;
    const auto stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmm"));
    const QString mfg = QString::fromStdString(s.manufacturer).trimmed();
    const QString serial = QString::fromStdString(s.serialNo).trimmed();

    QDir dir(QDir::homePath() + QStringLiteral("/Documents/") + baseFolder);
    const QString folderName = QStringLiteral("%1_%2_%3").arg(mfg, serial, stamp);
    if (!dir.mkpath(folderName))
        return false;

    const QString fileName = QStringLiteral("%1_%2_试验报告.pdf").arg(mfg, serial);
    const QString filePath = dir.filePath(folderName + QDir::separator() + fileName);

    QTextDocument doc;
    doc.setHtml(buildHtml(r));

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    doc.print(&printer);

    if (outPath)
        *outPath = filePath;
    return QFileInfo::exists(filePath);
}

} // namespace
