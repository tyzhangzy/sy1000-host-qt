#include "services/resultservice.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QVariantMap>

#include "dao/testresultdao.h"
#include "report/testreportgenerator.h"
#include "services/testresultservice.h"

namespace sy1000 {

ResultServiceAdapter::ResultServiceAdapter(QObject *parent)
    : QObject(parent)
{
}

QVariantList ResultServiceAdapter::results() const
{
    QVariantList out;
    for (const auto &r : TestResultService::findAll()) {
        QVariantMap m;
        m[QStringLiteral("id")] = r.id;
        m[QStringLiteral("serial")] = QString::fromStdString(r.testSerialNo);
        m[QStringLiteral("tester")] = QString::fromStdString(r.testerName);
        m[QStringLiteral("company")] = QString::fromStdString(r.testerCompany);
        m[QStringLiteral("manufacturer")] = QString::fromStdString(r.primarySample().manufacturer);
        m[QStringLiteral("sampleModel")] = QString::fromStdString(r.primarySample().sampleModel);
        m[QStringLiteral("sampleSerial")] = QString::fromStdString(r.primarySample().serialNo);
        m[QStringLiteral("overall")] = static_cast<int>(r.primarySample().overallResult);
        m[QStringLiteral("rate")] = r.primarySample().hydroStaticTest.residualDeformationRate;
        const auto t = std::chrono::system_clock::to_time_t(r.testDate);
        m[QStringLiteral("date")] = QDateTime::fromSecsSinceEpoch(t).toString(QStringLiteral("yyyy-MM-dd HH:mm"));
        out.append(m);
    }
    return out;
}

QVariantMap ResultServiceAdapter::details(int id) const
{
    const auto r = TestResultService::findById(id); // primary-key lookup (M4)
    if (r.id == 0 && r.testSerialNo.empty())
        return {};
    const auto t = std::chrono::system_clock::to_time_t(r.testDate);
    QVariantMap m;
    m[QStringLiteral("id")] = r.id;
    m[QStringLiteral("serial")] = QString::fromStdString(r.testSerialNo);
    m[QStringLiteral("tester")] = QString::fromStdString(r.testerName);
    m[QStringLiteral("company")] = QString::fromStdString(r.testerCompany);
    m[QStringLiteral("date")] = QDateTime::fromSecsSinceEpoch(t).toString(QStringLiteral("yyyy-MM-dd HH:mm"));
    m[QStringLiteral("sampleModel")] = QString::fromStdString(r.primarySample().sampleModel);
    m[QStringLiteral("manufacturer")] = QString::fromStdString(r.primarySample().manufacturer);
    m[QStringLiteral("sampleSerial")] = QString::fromStdString(r.primarySample().serialNo);
    m[QStringLiteral("volume")] = r.primarySample().volume;
    m[QStringLiteral("overall")] = static_cast<int>(r.primarySample().overallResult);
    m[QStringLiteral("workPressure")] = r.testStandard.workingPressure;
    m[QStringLiteral("testPressure")] = r.testStandard.testingPressure;
    m[QStringLiteral("rate")] = r.primarySample().hydroStaticTest.residualDeformationRate;
    m[QStringLiteral("initialWeight")] = r.primarySample().hydroStaticTest.initialWeight;
    m[QStringLiteral("pressureWeight")] = r.primarySample().hydroStaticTest.pressureWeight;
    m[QStringLiteral("finalWeight")] = r.primarySample().hydroStaticTest.finalWeight;
    m[QStringLiteral("fullDeformation")] = r.primarySample().hydroStaticTest.fullDeformation;
    m[QStringLiteral("residualDeformation")] = r.primarySample().hydroStaticTest.residualDeformation;
    m[QStringLiteral("external")] = static_cast<int>(r.primarySample().appearanceInspection.externalResult);
    m[QStringLiteral("internal")] = static_cast<int>(r.primarySample().appearanceInspection.internalResult);
    m[QStringLiteral("thread")] = static_cast<int>(r.primarySample().appearanceInspection.threadResult);
    m[QStringLiteral("valve")] = static_cast<int>(r.primarySample().appearanceInspection.valveResult);
    return m;
}

QVariantMap ResultServiceAdapter::reportData(int id) const
{
    const auto r = TestResultService::findById(id); // primary-key lookup (M4)
    if (r.id == 0 && r.testSerialNo.empty())
        return {};
    const auto t = std::chrono::system_clock::to_time_t(r.testDate);
    const auto &s = r.primarySample();
    const auto &h = s.hydroStaticTest;
    const auto &insp = s.appearanceInspection;
    QVariantMap m;
    m[QStringLiteral("id")] = r.id;
    m[QStringLiteral("serial")] = QString::fromStdString(r.testSerialNo);
    m[QStringLiteral("date")] = QDateTime::fromSecsSinceEpoch(t).toString(QStringLiteral("yyyy-MM-dd HH:mm"));
    m[QStringLiteral("tester")] = QString::fromStdString(r.testerName);
    m[QStringLiteral("company")] = QString::fromStdString(r.testerCompany);
    m[QStringLiteral("standardName")] = QString::fromStdString(r.testStandard.standardName);
    m[QStringLiteral("workPressure")] = r.testStandard.workingPressure;
    m[QStringLiteral("testPressure")] = r.testStandard.testingPressure;
    m[QStringLiteral("rateLimit")] = r.testStandard.residualDeformationRate;
    m[QStringLiteral("sampleModel")] = QString::fromStdString(s.sampleModel);
    m[QStringLiteral("manufacturer")] = QString::fromStdString(s.manufacturer);
    m[QStringLiteral("sampleSerial")] = QString::fromStdString(s.serialNo);
    m[QStringLiteral("volume")] = s.volume;
    m[QStringLiteral("overall")] = static_cast<int>(s.overallResult);
    m[QStringLiteral("initialWeight")] = h.initialWeight;
    m[QStringLiteral("pressureWeight")] = h.pressureWeight;
    m[QStringLiteral("finalWeight")] = h.finalWeight;
    m[QStringLiteral("fullDeformation")] = h.fullDeformation;
    m[QStringLiteral("residualDeformation")] = h.residualDeformation;
    m[QStringLiteral("rate")] = h.residualDeformationRate;
    m[QStringLiteral("external")] = static_cast<int>(insp.externalResult);
    m[QStringLiteral("internal")] = static_cast<int>(insp.internalResult);
    m[QStringLiteral("thread")] = static_cast<int>(insp.threadResult);
    m[QStringLiteral("valve")] = static_cast<int>(insp.valveResult);
    m[QStringLiteral("inspector")] = QString::fromStdString(insp.inspectorName);
    return m;
}

QString ResultServiceAdapter::generatePdf(int id)
{
    const auto r = TestResultService::findById(id); // primary-key lookup (M4)
    if (r.id == 0 && r.testSerialNo.empty())
        return QString();
    QString path;
    if (TestReportGenerator::generatePdf(r, &path))
        return path;
    return QString();
}

bool ResultServiceAdapter::openReportPdf(const QString &path)
{
    if (path.isEmpty())
        return false;
    // fromLocalFile escapes spaces / CJK characters correctly (L14).
    return QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void ResultServiceAdapter::removeResult(int id)
{
    TestResultDao::remove(id);
}

} // namespace sy1000
