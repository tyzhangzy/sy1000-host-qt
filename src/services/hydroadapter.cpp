#include "services/hydroadapter.h"

#include "services/testresultservice.h"

namespace sy1000 {

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

    // Sample pressure periodically for the realtime chart.
    m_sampleTimer.setInterval(100);
    QObject::connect(&m_sampleTimer, &QTimer::timeout, this, [this]() {
        emit pressureSample(m_controller.device()->currentPressure());
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
