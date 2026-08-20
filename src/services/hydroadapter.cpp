#include "services/hydroadapter.h"

#include <algorithm>
#include <cstddef>

#include <QCoreApplication>
#include <QDateTime>
#include <QVariantMap>

#include "services/configmanager.h"
#include "services/testresultservice.h"

namespace sy1000 {

namespace {

// Convert the QML inspection object (camelCase keys) into the model struct.
void fillInspection(SampleInspectionData &out, const QVariantMap &m)
{
    const auto str = [&m](const char *key) {
        return m.value(QLatin1String(key)).toString().toStdString();
    };
    const auto boolean = [&m](const char *key) {
        return m.value(QLatin1String(key)).toBool();
    };
    const auto result = [&m](const char *key) {
        return static_cast<InspectionResult>(m.value(QLatin1String(key)).toInt(0));
    };

    out.inspectorName = str("inspectorName");
    out.inspectorCertNo = str("inspectorCertNo");
    out.userCompany = str("userCompany");
    out.inspectionCompleted = boolean("inspectionCompleted");

    out.externalResult = result("external");
    out.internalResult = result("internal");
    out.threadResult = result("thread");
    out.valveResult = result("valve");

    out.externalThermalDamage = boolean("externalThermalDamage");
    out.externalScratch = boolean("externalScratch");
    out.externalWear = boolean("externalWear");
    out.externalDelamination = boolean("externalDelamination");
    out.externalDeformation = boolean("externalDeformation");
    out.externalDefectLocation = str("externalDefectLocation");
    out.externalOther = str("externalOther");

    out.internalSmell = boolean("internalSmell");
    out.internalDebris = str("internalDebris");
    out.internalSurfaceCondition = str("internalSurfaceCondition");
    out.internalDefectLocation = str("internalDefectLocation");
    out.internalOther = str("internalOther");

    out.threadSpecification = str("threadSpecification");
    out.threadCondition = str("threadCondition");
    out.threadEvaluation = str("threadEvaluation");
    out.threadOther = str("threadOther");

    out.valveNo = str("valveNo");
    out.valveThreadCondition = str("valveThreadCondition");
    out.valveAirTightness = str("valveAirTightness");
    out.valveDiaphragmReplaced = boolean("valveDiaphragmReplaced");
    out.valveOther = str("valveOther");
}

} // namespace


HydroTestControllerAdapter::HydroTestControllerAdapter(IHydroDeviceProvider *device, QObject *parent)
    : QObject(parent)
    , m_controller(device, this)
{
    // Short options so the demo flow completes quickly without real hardware.
    TestOptions opt;
    opt.checkCountdownSec = 1;
    opt.holdWorking1Sec = 1;
    opt.holdWorking2Sec = 1;
    opt.holdTestingSec = 1;
    opt.releaseSec = 1;
    opt.stabilizeSec = 1;
    m_controller.setOptions(opt);
    m_controller.setResidualDeformationRateLimit(3.0);

    QObject::connect(&m_controller, &HydrostaticTestController::stateChanged,
                     this, [this]() { emit stateChanged(); updateRunning(); });
    QObject::connect(&m_controller, &HydrostaticTestController::statusChanged,
                     this, [this](const QString &s) { m_status = s; emit statusChanged(); });
    QObject::connect(&m_controller, &HydrostaticTestController::testCompleted,
                     this, [this](bool ok) {
                         if (ok) {
                             m_lastResultId = TestResultService::save(buildResult());
                             m_resultSaved = true;
                             emit lastResultIdChanged();
                             m_status = QCoreApplication::translate("sy1000_core", "Result saved (id=%1)")
                                           .arg(m_lastResultId);
                             emit statusChanged();
                         }
                         emit testFinished(ok, countPassed(), countFailed());
                         updateRunning();
                     });
    QObject::connect(&m_controller, &HydrostaticTestController::testAborted,
                     this, [this](HydroTestError, const QString &m) {
                         m_status = m;
                         emit statusChanged();
                         updateRunning();
                     });
    QObject::connect(&m_controller, &HydrostaticTestController::confirmRequested,
                     this, [this](const QString &t, const QString &m) {
                         emit confirmRequested(t, m);
                     });

    // Sample pressure + per-sample weights periodically for the realtime chart,
    // and accumulate curve points for the saved report.
    m_sampleTimer.setInterval(100);
    QObject::connect(&m_sampleTimer, &QTimer::timeout, this, [this]() {
        const double p = m_controller.device()->currentPressure();
        const auto w = m_controller.device()->currentWeights();
        for (int i = 1; i <= 4 && i < static_cast<int>(w.size()); ++i)
            emit weightSample(i, w[i]);
        emit pressureSample(p);

        // Record a curve point for the persisted sample (index 1) while running.
        if (m_controller.state() != HydroTestState::Idle &&
            m_controller.state() != HydroTestState::Completed &&
            m_controller.state() != HydroTestState::Aborted) {
            PressureWeightPoint pt;
            pt.timestamp = DateTime(std::chrono::milliseconds(QDateTime::currentMSecsSinceEpoch()));
            pt.pressure = p;
            if (!w.empty())
                pt.weight = w[std::min<std::size_t>(1, w.size() - 1)];
            m_curvePoints.push_back(pt);

            // Cap the persisted curve so a long test does not keep growing the
            // JSON payload without bound (M9): keep the most recent samples.
            constexpr std::size_t kMaxCurvePoints = 20000;
            if (m_curvePoints.size() > kMaxCurvePoints) {
                const auto excess = static_cast<std::ptrdiff_t>(m_curvePoints.size() - kMaxCurvePoints);
                m_curvePoints.erase(m_curvePoints.begin(), m_curvePoints.begin() + excess);
            }
        }
    });
    m_sampleTimer.start();
}

void HydroTestControllerAdapter::startTest()
{
    m_curvePoints.clear();
    m_resultSaved = false;
    m_controller.startTest();
    updateRunning();
}

void HydroTestControllerAdapter::stopTest()
{
    m_controller.stopTest();
    updateRunning();
}

int HydroTestControllerAdapter::saveCurrentResult()
{
    // Save at most once per test run: later calls return the stored id (M5).
    if (!m_resultSaved) {
        m_lastResultId = TestResultService::save(buildResult());
        m_resultSaved = true;
        emit lastResultIdChanged();
    }
    return m_lastResultId;
}

void HydroTestControllerAdapter::setWorkingPressure(double p)
{
    TestOptions opt = m_controller.options();
    opt.workingPressure = p;
    m_controller.setOptions(opt);
}

void HydroTestControllerAdapter::setTestingPressure(double p)
{
    TestOptions opt = m_controller.options();
    opt.testingPressure = p;
    m_controller.setOptions(opt);
}

void HydroTestControllerAdapter::setTester(const QString &name, const QString &company)
{
    m_testerName = name;
    m_testerCompany = company;
}

void HydroTestControllerAdapter::respondConfirm(bool accepted)
{
    m_controller.respondConfirm(accepted);
}

void HydroTestControllerAdapter::setSample(int index, const QString &model, const QString &manufacturer,
                                           const QString &serialNo, double volume)
{
    if (index < 1 || index > 4)
        return;
    m_samples[index] = { model, manufacturer, serialNo, volume };
}

void HydroTestControllerAdapter::setSampleInspection(int index, const QVariantMap &inspection)
{
    if (index < 1 || index > 4)
        return;
    fillInspection(m_inspections[index], inspection);
    // Mark as completed so the saved result reflects that the inspection was done.
    if (m_inspections[index].inspectionCompleted)
        m_inspections[index].inspectionDate = std::chrono::system_clock::now();
}

void HydroTestControllerAdapter::setTestStandard(const QString &name, int holdTime, int residualRate)
{
    m_standardName = name;
    m_holdTime = holdTime;
    m_residualRate = residualRate;
    // Keep the controller's pass/fail limit in sync with the preparation page
    // input so the saved result and the actual decision use the same value (H1).
    m_controller.setResidualDeformationRateLimit(static_cast<double>(residualRate));
}

QString HydroTestControllerAdapter::testStandardInfo() const
{
    const auto &o = m_controller.options();
    return QStringLiteral("试验标准: %1\n公称工作压力: %2 MPa\n试验压力: %3 MPa\n"
                          "保压时间: %4 秒\n允许容积残余变形率: %5%")
        .arg(m_standardName)
        .arg(o.workingPressure)
        .arg(o.testingPressure)
        .arg(m_holdTime)
        .arg(m_residualRate);
}

QString HydroTestControllerAdapter::sampleInfo(int index) const
{
    if (index < 1 || index > 4)
        return QString();
    const auto &s = m_samples[index];
    return QStringLiteral("气瓶型号: %1\n制造厂商: %2\n产品编号: %3\n气瓶容积: %4 L")
        .arg(s.model, s.manufacturer, s.serialNo, QString::number(s.volume));
}

int HydroTestControllerAdapter::state() const
{
    return static_cast<int>(m_controller.state());
}

QString HydroTestControllerAdapter::stateName() const
{
    // Localized state names so the UI shows text instead of a raw enum int (M7).
    switch (m_controller.state()) {
    case HydroTestState::Idle: return QCoreApplication::translate("sy1000_core", "Idle");
    case HydroTestState::Preparing: return QCoreApplication::translate("sy1000_core", "Preparing");
    case HydroTestState::WaterJacketChecking: return QCoreApplication::translate("sy1000_core", "WaterJacketChecking");
    case HydroTestState::CylinderChecking: return QCoreApplication::translate("sy1000_core", "CylinderChecking");
    case HydroTestState::Initializing: return QCoreApplication::translate("sy1000_core", "Initializing");
    case HydroTestState::PressurizingToWorking: return QCoreApplication::translate("sy1000_core", "PressurizingToWorking");
    case HydroTestState::HoldingAtWorking: return QCoreApplication::translate("sy1000_core", "HoldingAtWorking");
    case HydroTestState::PressurizingToTesting: return QCoreApplication::translate("sy1000_core", "PressurizingToTesting");
    case HydroTestState::HoldingAtTesting: return QCoreApplication::translate("sy1000_core", "HoldingAtTesting");
    case HydroTestState::ReleasingPressure: return QCoreApplication::translate("sy1000_core", "ReleasingPressure");
    case HydroTestState::Stabilizing: return QCoreApplication::translate("sy1000_core", "Stabilizing");
    case HydroTestState::CalculatingResult: return QCoreApplication::translate("sy1000_core", "CalculatingResult");
    case HydroTestState::Completed: return QCoreApplication::translate("sy1000_core", "Completed");
    case HydroTestState::Aborted: return QCoreApplication::translate("sy1000_core", "Aborted");
    }
    return QString();
}

bool HydroTestControllerAdapter::running() const
{
    const auto s = m_controller.state();
    return s != HydroTestState::Idle && s != HydroTestState::Completed && s != HydroTestState::Aborted;
}

int HydroTestControllerAdapter::countPassed() const
{
    int n = 0;
    for (const auto r : m_controller.testData().results)
        if (r == TestResultStatus::Passed)
            ++n;
    return n;
}

int HydroTestControllerAdapter::countFailed() const
{
    int n = 0;
    for (const auto r : m_controller.testData().results)
        if (r == TestResultStatus::Failed)
            ++n;
    return n;
}

void HydroTestControllerAdapter::updateRunning()
{
    emit runningChanged();
}

UnifiedTestResult HydroTestControllerAdapter::buildResult() const
{
    const auto &d = m_controller.testData();

    TestStandard ts;
    ts.standardName = m_standardName.toStdString();
    ts.workingPressure = m_controller.options().workingPressure;
    ts.testingPressure = m_controller.options().testingPressure;
    ts.pressureHoldingTime = m_holdTime;
    ts.residualDeformationRate = m_residualRate;

    UnifiedTestResult r = TestResultService::createUnifiedTestResult(m_testerName.toStdString(),
                                                                     m_testerCompany.toStdString(),
                                                                     Sample(), ts);
    // Build all four samples so no data is dropped (H3).
    for (int i = 1; i <= 4; ++i) {
        const auto &info = m_samples[i];
        Sample sample;
        sample.sampleId = (QStringLiteral("S") + QString::number(i)).toStdString();
        sample.sampleModel = info.model.toStdString();
        sample.manufacturer = info.manufacturer.toStdString();
        sample.serialNo = info.serialNo.toStdString();
        sample.volume = info.volume;

        auto &h = sample.hydroStaticTest;
        h.initialWeight = d.initialWeights[i];
        h.pressureWeight = d.testingPressureWeights[i];
        h.finalWeight = d.releasedWeights[i];
        h.fullDeformation = d.fullDeformations[i];
        h.residualDeformation = d.residualDeformations[i];
        h.residualDeformationRate = d.residualDeformationRates[i];
        h.testResult = d.results[i];
        h.workingPressure = m_controller.options().workingPressure;
        h.testPressure = m_controller.options().testingPressure;
        sample.appearanceInspection = m_inspections[i];
        sample.overallResult = TestResultService::determineOverallResult(sample.appearanceInspection, h);
        // Only the primary sample carries the full pressure/weight curve for the
        // report; the other samples store the per-sample weight result data.
        if (i == 1)
            h.pressureWeightData = m_curvePoints;
        r.samples.push_back(std::move(sample));
    }

    // Environment data (from config + placeholders; sensors would feed these live).
    r.testEnvironment.roomTemperature = 23.0;
    r.testEnvironment.humidity = 45.0;
    r.testEnvironment.equipmentId = ConfigManager::serialNo().toStdString();
    r.testEnvironment.equipmentModel = ConfigManager::deviceName().toStdString();
    return r;
}

} // namespace sy1000
