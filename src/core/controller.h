#pragma once

#include <QObject>
#include <QString>

#include "core/hydrotypes.h"
#include "core/ideviceprovider.h"
#include "models/model.h"

namespace sy1000 {

class HydroSubTask;

// Macro state machine for the hydrostatic test. Uses a hand-written
// enum + switch (no QStateMachine). Drives sub-tasks; sub-task completion
// triggers state transitions.
class HydrostaticTestController : public QObject
{
    Q_OBJECT
public:
    explicit HydrostaticTestController(IHydroDeviceProvider *device, QObject *parent = nullptr);

    void startTest();
    void stopTest();
    void reset();

    // Residual deformation rate limit (%) for pass/fail (default 3.0).
    void setResidualDeformationRateLimit(double limit) { m_rateLimit = limit; }

    // Runtime options (intervals / pressures / timeouts).
    void setOptions(const sy1000::TestOptions &options) { m_options = options; }
    const sy1000::TestOptions &options() const { return m_options; }

    HydroTestState state() const { return m_state; }
    const HydroTestData &testData() const { return m_data; }

signals:
    void stateChanged(sy1000::HydroTestState state);
    void statusChanged(const QString &content);
    void voicePrompt(const QString &text);
    void dataUpdated();
    void testCompleted(bool success);
    void testAborted(sy1000::HydroTestError error, const QString &message);

private:
    void transitionTo(HydroTestState next);
    void enterState(HydroTestState state);
    void onSubTaskFinished(bool success, HydroTestError error, const TaskResult &result);
    void runTask(HydroSubTask *task, const TaskParams &params);
    void calculateResult();
    void safeShutdown();

    IHydroDeviceProvider *m_device;
    HydroTestState m_state = HydroTestState::Idle;
    HydroTestData m_data;
    HydroSubTask *m_currentTask = nullptr;
    double m_rateLimit = 3.0;
    TestOptions m_options;
};

} // namespace sy1000
