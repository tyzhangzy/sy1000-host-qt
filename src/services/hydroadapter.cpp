#include "services/hydroadapter.h"

#include <QVariantMap>

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
                             const int id = TestResultService::save(buildResult());
                             m_status = QStringLiteral("Result saved (id=%1)").arg(id);
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

    // Sample pressure + per-sample weights periodically for the realtime chart.
    m_sampleTimer.setInterval(100);
    QObject::connect(&m_sampleTimer, &QTimer::timeout, this, [this]() {
        emit pressureSample(m_controller.device()->currentPressure());
        const auto w = m_controller.device()->currentWeights();
        for (int i = 1; i <= 4 && i < static_cast<int>(w.size()); ++i)
            emit weightSample(i, w[i]);
    });
    m_sampleTimer.start();
}

void HydroTestControllerAdapter::startTest()
{
    m_controller.startTest();
    updateRunning();
}

void HydroTestControllerAdapter::stopTest()
{
    m_controller.stopTest();
    updateRunning();
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

int HydroTestControllerAdapter::state() const
{
    return static_cast<int>(m_controller.state());
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

    const auto &s = m_samples[1];
    Sample sample;
    sample.sampleId = QStringLiteral("S1").toStdString();
    sample.sampleModel = s.model.toStdString();
    sample.manufacturer = s.manufacturer.toStdString();
    sample.serialNo = s.serialNo.toStdString();
    sample.volume = s.volume;

    auto &h = sample.hydroStaticTest;
    h.initialWeight = d.initialWeights[1];
    h.pressureWeight = d.testingPressureWeights[1];
    h.finalWeight = d.releasedWeights[1];
    h.fullDeformation = d.fullDeformations[1];
    h.residualDeformation = d.residualDeformations[1];
    h.residualDeformationRate = d.residualDeformationRates[1];
    h.testResult = d.results[1];
    // Attach the appearance inspection data captured on the preparation page.
    sample.appearanceInspection = m_inspections[1];
    sample.overallResult = TestResultService::determineOverallResult(sample.appearanceInspection, h);

    TestStandard ts;
    ts.standardName = QStringLiteral("Demo").toStdString();
    ts.workingPressure = m_controller.options().workingPressure;
    ts.testingPressure = m_controller.options().testingPressure;
    ts.residualDeformationRate = 3.0;

    return TestResultService::createUnifiedTestResult(m_testerName.toStdString(),
                                                      m_testerCompany.toStdString(),
                                                      sample, ts);
}

} // namespace sy1000
