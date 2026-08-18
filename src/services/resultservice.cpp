#include "services/resultservice.h"

#include <QDateTime>
#include <QVariantMap>

#include "dao/testresultdao.h"
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

void ResultServiceAdapter::removeResult(int id)
{
    TestResultDao::remove(id);
}

} // namespace sy1000
