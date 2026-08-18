#include "core/controller.h"

#include <cmath>
#include <vector>

#include "core/subtask.h"
#include "core/tasks.h"

namespace sy1000 {

HydrostaticTestController::HydrostaticTestController(IHydroDeviceProvider *device, QObject *parent)
    : QObject(parent)
    , m_device(device)
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
    m_state = next;
    emit stateChanged(next);
    enterState(next);
}

void HydrostaticTestController::enterState(HydroTestState state)
{
    switch (state) {
    case HydroTestState::Preparing:
        emit voicePrompt(QStringLiteral("Test preparation"));
        m_device->setWaterJacketLock(1, true);
        transitionTo(HydroTestState::WaterJacketChecking);
        break;

    case HydroTestState::WaterJacketChecking: {
        emit voicePrompt(QStringLiteral("Checking water jacket"));
        TaskParams p;
        p.countdownSec = m_options.checkCountdownSec;
        runTask(new WaitTask(m_device, this), p);
        break;
    }

    case HydroTestState::CylinderChecking: {
        emit voicePrompt(QStringLiteral("Checking cylinder pressure"));
        TaskParams p;
        p.countdownSec = m_options.checkCountdownSec;
        runTask(new WaitTask(m_device, this), p);
        break;
    }

    case HydroTestState::Initializing: {
        m_data.initialPressure = m_device->currentPressure();
        const auto w = m_device->currentWeights();
        for (std::size_t i = 0; i < w.size() && i < m_data.initialWeights.size(); ++i)
            m_data.initialWeights[i] = w[i];
        emit dataUpdated();
        transitionTo(HydroTestState::PressurizingToWorking);
        break;
    }

    case HydroTestState::PressurizingToWorking: {
        emit voicePrompt(QStringLiteral("Pressurizing to working pressure"));
        TaskParams p;
        p.targetPressure = m_options.workingPressure;
        p.fastStopDelta = 0.0;
        p.useSlowPump = false;
        p.fastTimeoutMinutes = m_options.fastTimeoutMin;
        runTask(new PressurizeTask(m_device, this), p);
        break;
    }

    case HydroTestState::HoldingAtWorking: {
        emit voicePrompt(QStringLiteral("Holding at working pressure"));
        TaskParams p;
        p.holdSampleInterval1Sec = m_options.holdWorking1Sec;
        p.holdSampleInterval2Sec = m_options.holdWorking2Sec;
        runTask(new HoldTask(m_device, this), p);
        break;
    }

    case HydroTestState::PressurizingToTesting: {
        emit voicePrompt(QStringLiteral("Pressurizing to test pressure"));
        TaskParams p;
        p.targetPressure = m_options.testingPressure;
        p.fastStopDelta = m_options.fastStopDelta;
        p.slowStopDelta = m_options.slowStopDelta;
        p.useSlowPump = true;
        p.fastTimeoutMinutes = m_options.fastTimeoutMin;
        p.slowTimeoutMinutes = m_options.slowTimeoutMin;
        runTask(new PressurizeTask(m_device, this), p);
        break;
    }

    case HydroTestState::HoldingAtTesting: {
        emit voicePrompt(QStringLiteral("Holding at test pressure"));
        TaskParams p;
        p.countdownSec = m_options.holdTestingSec;
        runTask(new WaitTask(m_device, this), p);
        break;
    }

    case HydroTestState::ReleasingPressure: {
        emit voicePrompt(QStringLiteral("Releasing pressure"));
        TaskParams p;
        p.countdownSec = m_options.releaseSec;
        p.initialPressure = m_data.initialPressure;
        runTask(new ReleaseTask(m_device, this), p);
        break;
    }

    case HydroTestState::Stabilizing: {
        emit voicePrompt(QStringLiteral("Waiting for scales to stabilize"));
        TaskParams p;
        p.countdownSec = m_options.stabilizeSec;
        runTask(new StabilizeTask(m_device, this), p);
        break;
    }

    case HydroTestState::CalculatingResult: {
        // store released weights then compute result
        const auto w = m_device->currentWeights();
        for (std::size_t i = 0; i < w.size() && i < m_data.releasedWeights.size(); ++i)
            m_data.releasedWeights[i] = w[i];
        calculateResult();
        transitionTo(HydroTestState::Completed);
        break;
    }

    case HydroTestState::Completed:
        emit voicePrompt(QStringLiteral("Test completed"));
        safeShutdown();
        emit testCompleted(true);
        break;

    default:
        break; // Idle / Aborted: stay
    }
}

void HydrostaticTestController::onSubTaskFinished(bool success, HydroTestError error, const TaskResult &result)
{
    Q_UNUSED(result);

    if (!success) {
        if (error == HydroTestError::LeakDetected)
            emit testAborted(error, QStringLiteral("Leak detected, please check and retry."));
        else if (error == HydroTestError::Timeout)
            emit testAborted(error, QStringLiteral("Pressurization timed out."));
        else if (error != HydroTestError::Cancelled)
            emit testAborted(error, QStringLiteral("Test aborted."));
        transitionTo(HydroTestState::Aborted);
        return;
    }

    switch (m_state) {
    case HydroTestState::WaterJacketChecking:
        transitionTo(HydroTestState::CylinderChecking);
        break;
    case HydroTestState::CylinderChecking:
        transitionTo(HydroTestState::Initializing);
        break;
    case HydroTestState::PressurizingToWorking:
        m_data.workingPressureT10 = result.pressureT10;
        m_data.workingPressureT30 = result.pressureT30;
        transitionTo(HydroTestState::HoldingAtWorking);
        break;
    case HydroTestState::HoldingAtWorking:
        transitionTo(HydroTestState::PressurizingToTesting);
        break;
    case HydroTestState::PressurizingToTesting: {
        const auto w = m_device->currentWeights();
        for (std::size_t i = 0; i < w.size() && i < m_data.testingPressureWeights.size(); ++i)
            m_data.testingPressureWeights[i] = w[i];
        transitionTo(HydroTestState::HoldingAtTesting);
        break;
    }
    case HydroTestState::HoldingAtTesting:
        transitionTo(HydroTestState::ReleasingPressure);
        break;
    case HydroTestState::ReleasingPressure:
        transitionTo(HydroTestState::Stabilizing);
        break;
    case HydroTestState::Stabilizing:
        transitionTo(HydroTestState::CalculatingResult);
        break;
    default:
        break;
    }
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

void HydrostaticTestController::calculateResult()
{
    for (const int i : m_device->availableScales()) {
        if (i < 0 || i >= 5)
            continue;
        const double full = m_data.testingPressureWeights[i] - m_data.initialWeights[i];
        const double residual = m_data.releasedWeights[i] - m_data.initialWeights[i];
        const double rate = (full != 0.0) ? (residual / full * 100.0) : 0.0;

        m_data.fullDeformations[i] = full;
        m_data.residualDeformations[i] = residual;
        m_data.residualDeformationRates[i] = rate;

        if (rate <= m_rateLimit) {
            m_data.results[i] = TestResultStatus::Passed;
            m_data.resultStrings[i] = "Passed";
        } else {
            m_data.results[i] = TestResultStatus::Failed;
            m_data.resultStrings[i] = "Failed";
        }
    }
    emit dataUpdated();
}

void HydrostaticTestController::safeShutdown()
{
    m_device->setFastPump(false);
    m_device->setSlowPump(false);
    m_device->setWaterInlet(false);
    m_device->setWaterJacketLock(1, false);
}

} // namespace sy1000
