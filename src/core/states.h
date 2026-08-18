#pragma once

#include "core/hydrotypes.h"
#include "models/model.h"

namespace sy1000 {

class HydrostaticTestController;
class HydroSubTask;

// State pattern: one class per HydroTestState. Each state owns its "enter"
// action (start a sub-task / record data) and decides the next state when a
// sub-task finishes. The controller only orchestrates transitions.
class HydroStateBase
{
public:
    virtual ~HydroStateBase() = default;

    virtual void enter(HydrostaticTestController &c) = 0;
    // sub-task finished successfully -> return the next HydroTestState
    virtual HydroTestState onTaskFinished(HydrostaticTestController &c, const TaskResult &result) = 0;
    // sub-task failed -> default aborts the test
    virtual HydroTestState onTaskFailed(HydrostaticTestController &c, HydroTestError error);
    virtual void leave(HydrostaticTestController &c);
};

// ---- concrete states ----
class IdleState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::Idle; }
};

class PreparingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::Idle; }
};

class WaterJacketCheckingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::CylinderChecking; }
};

class CylinderCheckingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::Initializing; }
};

class InitializingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::PressurizingToWorking; }
};

class PressurizingToWorkingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &c, const TaskResult &r) override;
};

class HoldingAtWorkingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::PressurizingToTesting; }
};

class PressurizingToTestingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &c, const TaskResult &) override;
};

class HoldingAtTestingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::ReleasingPressure; }
};

class ReleasingPressureState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::Stabilizing; }
};

class StabilizingState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::CalculatingResult; }
};

class CalculatingResultState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::Completed; }
};

class CompletedState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &c) override;
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::Completed; }
};

class AbortedState : public HydroStateBase
{
public:
    void enter(HydrostaticTestController &) override {}
    HydroTestState onTaskFinished(HydrostaticTestController &, const TaskResult &) override { return HydroTestState::Aborted; }
};

} // namespace sy1000
