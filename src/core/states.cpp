#include "core/states.h"

#include "core/controller.h"
#include "core/subtask.h"
#include "core/tasks.h"

namespace sy1000 {

HydroTestState HydroStateBase::onTaskFailed(HydrostaticTestController &, HydroTestError)
{
    return HydroTestState::Aborted;
}

void HydroStateBase::leave(HydrostaticTestController &)
{
}

// ---------------- Idle / Preparing ----------------
void IdleState::enter(HydrostaticTestController &)
{
}

void PreparingState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Test preparation"));
    c.device()->setWaterJacketLock(1, true);
    c.transitionTo(HydroTestState::WaterJacketChecking);
}

// ---------------- Checks ----------------
void WaterJacketCheckingState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Checking water jacket"));
    TaskParams p;
    p.countdownSec = c.options().checkCountdownSec;
    c.runTask(new WaitTask(c.device(), &c), p);
}

void CylinderCheckingState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Checking cylinder pressure"));
    TaskParams p;
    p.countdownSec = c.options().checkCountdownSec;
    c.runTask(new WaitTask(c.device(), &c), p);
}

void InitializingState::enter(HydrostaticTestController &c)
{
    c.data().initialPressure = c.device()->currentPressure();
    const auto w = c.device()->currentWeights();
    for (std::size_t i = 0; i < w.size() && i < c.data().initialWeights.size(); ++i)
        c.data().initialWeights[i] = w[i];
    c.emitDataUpdated();
    c.transitionTo(HydroTestState::PressurizingToWorking);
}

// ---------------- Pressurize / Hold ----------------
void PressurizingToWorkingState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Pressurizing to working pressure"));
    TaskParams p;
    p.targetPressure = c.options().workingPressure;
    p.fastStopDelta = 0.0;
    p.useSlowPump = false;
    p.fastTimeoutMinutes = c.options().fastTimeoutMin;
    c.runTask(new PressurizeTask(c.device(), &c), p);
}

HydroTestState PressurizingToWorkingState::onTaskFinished(HydrostaticTestController &c, const TaskResult &r)
{
    c.data().workingPressureT10 = r.pressureT10;
    c.data().workingPressureT30 = r.pressureT30;
    return HydroTestState::HoldingAtWorking;
}

void HoldingAtWorkingState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Holding at working pressure"));
    TaskParams p;
    p.holdSampleInterval1Sec = c.options().holdWorking1Sec;
    p.holdSampleInterval2Sec = c.options().holdWorking2Sec;
    c.runTask(new HoldTask(c.device(), &c), p);
}

void PressurizingToTestingState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Pressurizing to test pressure"));
    TaskParams p;
    p.targetPressure = c.options().testingPressure;
    p.fastStopDelta = c.options().fastStopDelta;
    p.slowStopDelta = c.options().slowStopDelta;
    p.useSlowPump = true;
    p.fastTimeoutMinutes = c.options().fastTimeoutMin;
    p.slowTimeoutMinutes = c.options().slowTimeoutMin;
    c.runTask(new PressurizeTask(c.device(), &c), p);
}

HydroTestState PressurizingToTestingState::onTaskFinished(HydrostaticTestController &c, const TaskResult &)
{
    const auto w = c.device()->currentWeights();
    for (std::size_t i = 0; i < w.size() && i < c.data().testingPressureWeights.size(); ++i)
        c.data().testingPressureWeights[i] = w[i];
    return HydroTestState::HoldingAtTesting;
}

void HoldingAtTestingState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Holding at test pressure"));
    TaskParams p;
    p.countdownSec = c.options().holdTestingSec;
    c.runTask(new WaitTask(c.device(), &c), p);
}

// ---------------- Release / Stabilize / Result ----------------
void ReleasingPressureState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Releasing pressure"));
    TaskParams p;
    p.countdownSec = c.options().releaseSec;
    p.initialPressure = c.data().initialPressure;
    c.runTask(new ReleaseTask(c.device(), &c), p);
}

void StabilizingState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Waiting for scales to stabilize"));
    TaskParams p;
    p.countdownSec = c.options().stabilizeSec;
    c.runTask(new StabilizeTask(c.device(), &c), p);
}

void CalculatingResultState::enter(HydrostaticTestController &c)
{
    const auto w = c.device()->currentWeights();
    for (std::size_t i = 0; i < w.size() && i < c.data().releasedWeights.size(); ++i)
        c.data().releasedWeights[i] = w[i];

    for (const int i : c.device()->availableScales()) {
        if (i < 0 || i >= 5)
            continue;
        const double full = c.data().testingPressureWeights[i] - c.data().initialWeights[i];
        const double residual = c.data().releasedWeights[i] - c.data().initialWeights[i];
        const double rate = (full != 0.0) ? (residual / full * 100.0) : 0.0;

        c.data().fullDeformations[i] = full;
        c.data().residualDeformations[i] = residual;
        c.data().residualDeformationRates[i] = rate;

        if (rate <= c.rateLimit()) {
            c.data().results[i] = TestResultStatus::Passed;
            c.data().resultStrings[i] = "Passed";
        } else {
            c.data().results[i] = TestResultStatus::Failed;
            c.data().resultStrings[i] = "Failed";
        }
    }
    c.emitDataUpdated();
    c.transitionTo(HydroTestState::Completed);
}

void CompletedState::enter(HydrostaticTestController &c)
{
    c.voice(QStringLiteral("Test completed"));
    c.safeShutdown();
    c.emitTestCompleted(true);
}

} // namespace sy1000
