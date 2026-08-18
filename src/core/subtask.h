#pragma once

#include <QObject>
#include <QString>
#include <functional>

#include "core/hydrotypes.h"
#include "core/ideviceprovider.h"

namespace sy1000 {

// Base class for a hydrostatic sub-task. Each sub-task encapsulates one device
// action or wait (pressurize, hold, release, stabilize, ...). Results are
// reported via the finished signal. Asynchronous waits use QTimer.
class HydroSubTask : public QObject
{
    Q_OBJECT
public:
    explicit HydroSubTask(IHydroDeviceProvider *device, QObject *parent = nullptr);

    void start(const TaskParams &params);
    virtual void stop();
    virtual void reset();

signals:
    void statusChanged(const QString &status);
    void finished(bool success, sy1000::HydroTestError error, const sy1000::TaskResult &result);

protected:
    // Entry point invoked once by start(). Implementation must eventually call finish().
    virtual void run() = 0;

    bool stopped() const { return m_stopped; }

    // Schedule a single delayed call; no-op if stopped.
    void delay(int ms, std::function<void()> callback);

    // Report the sub-task result exactly once.
    void finish(bool success, HydroTestError error, const TaskResult &result = {});

    IHydroDeviceProvider *m_device;
    TaskParams m_params;

private:
    bool m_stopped = false;
    bool m_finished = false;
};

} // namespace sy1000
