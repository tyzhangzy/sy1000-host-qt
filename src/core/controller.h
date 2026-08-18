#pragma once

#include <memory>

#include <QObject>
#include <QString>

#include "core/hydrotypes.h"
#include "core/ideviceprovider.h"
#include "core/states.h"
#include "models/model.h"

namespace sy1000 {

class HydroSubTask;

// Orchestrator for the hydrostatic test. Owns the current HydroStateBase,
// the shared test data and device; state classes drive the transitions.
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
    void setOptions(const TestOptions &options) { m_options = options; }
    const TestOptions &options() const { return m_options; }

    HydroTestState state() const { return m_state; }
    const HydroTestData &testData() const { return m_data; }

    // ---- accessors used by state classes ----
    IHydroDeviceProvider *device() { return m_device; }
    HydroTestData &data() { return m_data; }
    double rateLimit() const { return m_rateLimit; }

    void runTask(HydroSubTask *task, const TaskParams &params);
    void voice(const QString &text) { emit voicePrompt(text); }
    void emitDataUpdated() { emit dataUpdated(); }
    void emitTestCompleted(bool ok) { emit testCompleted(ok); }
    void safeShutdown();
    void transitionTo(HydroTestState next);

signals:
    void stateChanged(HydroTestState state);
    void statusChanged(const QString &content);
    void voicePrompt(const QString &text);
    void dataUpdated();
    void testCompleted(bool success);
    void testAborted(HydroTestError error, const QString &message);

private:
    std::unique_ptr<HydroStateBase> createState(HydroTestState s);
    void onSubTaskFinished(bool success, HydroTestError error, const TaskResult &result);

    IHydroDeviceProvider *m_device;
    HydroTestState m_state = HydroTestState::Idle;
    HydroTestData m_data;
    std::unique_ptr<HydroStateBase> m_currentState;
    HydroSubTask *m_currentTask = nullptr;
    double m_rateLimit = 3.0;
    TestOptions m_options;
};

} // namespace sy1000
