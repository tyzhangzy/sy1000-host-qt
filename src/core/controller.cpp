#include "core/controller.h"

#include "core/states.h"
#include "core/subtask.h"

namespace sy1000 {

HydrostaticTestController::HydrostaticTestController(IHydroDeviceProvider *device, QObject *parent)
    : QObject(parent)
    , m_device(device)
    , m_currentState(new IdleState)
{
}

void HydrostaticTestController::startTest()
{
    reset();
    transitionTo(HydroTestState::Preparing);
}

void HydrostaticTestController::stopTest()
{
    if (m_currentTask)
        m_currentTask->stop();
    safeShutdown();
    transitionTo(HydroTestState::Aborted);
    emit statusChanged(QStringLiteral("Test stopped manually"));
}

void HydrostaticTestController::reset()
{
    if (m_currentTask) {
        m_currentTask->reset();
        m_currentTask->deleteLater();
        m_currentTask = nullptr;
    }
    m_data.reset();
    transitionTo(HydroTestState::Idle);
}

void HydrostaticTestController::transitionTo(HydroTestState next)
{
    if (m_state == next)
        return;
    if (m_currentState)
        m_currentState->leave(*this);
    m_currentState = createState(next);
    m_state = next;
    emit stateChanged(next);
    m_currentState->enter(*this);
}

std::unique_ptr<HydroStateBase> HydrostaticTestController::createState(HydroTestState s)
{
    switch (s) {
    case HydroTestState::Idle: return std::make_unique<IdleState>();
    case HydroTestState::Preparing: return std::make_unique<PreparingState>();
    case HydroTestState::WaterJacketChecking: return std::make_unique<WaterJacketCheckingState>();
    case HydroTestState::CylinderChecking: return std::make_unique<CylinderCheckingState>();
    case HydroTestState::Initializing: return std::make_unique<InitializingState>();
    case HydroTestState::PressurizingToWorking: return std::make_unique<PressurizingToWorkingState>();
    case HydroTestState::HoldingAtWorking: return std::make_unique<HoldingAtWorkingState>();
    case HydroTestState::PressurizingToTesting: return std::make_unique<PressurizingToTestingState>();
    case HydroTestState::HoldingAtTesting: return std::make_unique<HoldingAtTestingState>();
    case HydroTestState::ReleasingPressure: return std::make_unique<ReleasingPressureState>();
    case HydroTestState::Stabilizing: return std::make_unique<StabilizingState>();
    case HydroTestState::CalculatingResult: return std::make_unique<CalculatingResultState>();
    case HydroTestState::Completed: return std::make_unique<CompletedState>();
    case HydroTestState::Aborted: return std::make_unique<AbortedState>();
    }
    return std::make_unique<IdleState>();
}

void HydrostaticTestController::onSubTaskFinished(bool success, HydroTestError error, const TaskResult &result)
{
    if (!success) {
        if (error == HydroTestError::LeakDetected)
            emit testAborted(error, QStringLiteral("Leak detected, please check and retry."));
        else if (error == HydroTestError::Timeout)
            emit testAborted(error, QStringLiteral("Pressurization timed out."));
        else if (error != HydroTestError::Cancelled)
            emit testAborted(error, QStringLiteral("Test aborted."));
        const HydroTestState next = m_currentState->onTaskFailed(*this, error);
        transitionTo(next);
        return;
    }

    const HydroTestState next = m_currentState->onTaskFinished(*this, result);
    transitionTo(next);
}

void HydrostaticTestController::runTask(HydroSubTask *task, const TaskParams &params)
{
    if (m_currentTask)
        m_currentTask->deleteLater();
    m_currentTask = task;
    QObject::connect(task, &HydroSubTask::finished, this, &HydrostaticTestController::onSubTaskFinished);
    QObject::connect(task, &HydroSubTask::statusChanged, this,
                     [this](const QString &s) { emit statusChanged(s); });
    task->start(params);
}

void HydrostaticTestController::safeShutdown()
{
    m_device->setFastPump(false);
    m_device->setSlowPump(false);
    m_device->setWaterInlet(false);
    m_device->setWaterJacketLock(1, false);
}

} // namespace sy1000
