#include "core/hydroadapter.h"

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
                         emit testFinished(ok, countPassed(), countFailed());
                         updateRunning();
                     });
    QObject::connect(&m_controller, &HydrostaticTestController::testAborted,
                     this, [this](HydroTestError, const QString &m) {
                         m_status = m;
                         emit statusChanged();
                         updateRunning();
                     });
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

} // namespace sy1000
