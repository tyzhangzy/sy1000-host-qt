#include "services/resultservice.h"

#include <QDateTime>
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
        m[QStringLiteral("manufacturer")] = QString::fromStdString(r.sample.manufacturer);
        m[QStringLiteral("sampleModel")] = QString::fromStdString(r.sample.sampleModel);
        m[QStringLiteral("sampleSerial")] = QString::fromStdString(r.sample.serialNo);
        m[QStringLiteral("overall")] = static_cast<int>(r.sample.overallResult);
        m[QStringLiteral("rate")] = r.sample.hydroStaticTest.residualDeformationRate;
        const auto t = std::chrono::system_clock::to_time_t(r.testDate);
        m[QStringLiteral("date")] = QDateTime::fromSecsSinceEpoch(t).toString(QStringLiteral("yyyy-MM-dd HH:mm"));
        out.append(m);
    }
    return out;
}

QVariantMap ResultServiceAdapter::details(int id) const
{
    for (const auto &r : TestResultService::findAll()) {
        if (r.id != id)
            continue;
        const auto t = std::chrono::system_clock::to_time_t(r.testDate);
        QVariantMap m;
        m[QStringLiteral("id")] = r.id;
        m[QStringLiteral("serial")] = QString::fromStdString(r.testSerialNo);
        m[QStringLiteral("tester")] = QString::fromStdString(r.testerName);
        m[QStringLiteral("company")] = QString::fromStdString(r.testerCompany);
        m[QStringLiteral("date")] = QDateTime::fromSecsSinceEpoch(t).toString(QStringLiteral("yyyy-MM-dd HH:mm"));
        m[QStringLiteral("sampleModel")] = QString::fromStdString(r.sample.sampleModel);
        m[QStringLiteral("manufacturer")] = QString::fromStdString(r.sample.manufacturer);
        m[QStringLiteral("sampleSerial")] = QString::fromStdString(r.sample.serialNo);
        m[QStringLiteral("volume")] = r.sample.volume;
        m[QStringLiteral("overall")] = static_cast<int>(r.sample.overallResult);
        m[QStringLiteral("workPressure")] = r.testStandard.workingPressure;
        m[QStringLiteral("testPressure")] = r.testStandard.testingPressure;
        m[QStringLiteral("rate")] = r.sample.hydroStaticTest.residualDeformationRate;
        m[QStringLiteral("initialWeight")] = r.sample.hydroStaticTest.initialWeight;
        m[QStringLiteral("pressureWeight")] = r.sample.hydroStaticTest.pressureWeight;
        m[QStringLiteral("finalWeight")] = r.sample.hydroStaticTest.finalWeight;
        m[QStringLiteral("fullDeformation")] = r.sample.hydroStaticTest.fullDeformation;
        m[QStringLiteral("residualDeformation")] = r.sample.hydroStaticTest.residualDeformation;
        return m;
    }
    return {};
}

QVariantMap ResultServiceAdapter::reportData(int id) const
{
    for (const auto &r : TestResultService::findAll()) {
        if (r.id != id)
            continue;
        const auto t = std::chrono::system_clock::to_time_t(r.testDate);
        const auto &s = r.sample;
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
    return {};
}

QString ResultServiceAdapter::generatePdf(int id)
{
    for (const auto &r : TestResultService::findAll()) {
        if (r.id != id)
            continue;
        QString path;
        if (TestReportGenerator::generatePdf(r, &path))
            return path;
        return QString();
    }
    return QString();
}

void ResultServiceAdapter::removeResult(int id)
{
    TestResultDao::remove(id);
}

} // namespace sy1000
