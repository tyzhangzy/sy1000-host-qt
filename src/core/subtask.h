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

public slots:
    // Resume a pending confirmation request with the operator's answer.
    void confirmResponse(bool accepted);

signals:
    void statusChanged(const QString &status);
    // Ask the operator for a blocking instruction confirmation. The sub-task
    // pauses until confirmResponse() is invoked.
    void requestConfirm(const QString &title, const QString &message);
    void finished(bool success, sy1000::HydroTestError error, const sy1000::TaskResult &result);

protected:
    // Entry point invoked once by start(). Implementation must eventually call finish().
    virtual void run() = 0;

    bool stopped() const { return m_stopped; }

    // Schedule a single delayed call; no-op if stopped.
    void delay(int ms, std::function<void()> callback);

    // Emit a blocking instruction confirmation and pause until confirmResponse();
    // invokes callback(accepted) on resume.
    void requestConfirmation(const QString &title, const QString &message,
                             std::function<void(bool)> callback);

    // Report the sub-task result exactly once.
    void finish(bool success, HydroTestError error, const TaskResult &result = {});

    IHydroDeviceProvider *m_device;
    TaskParams m_params;

private:
    std::function<void(bool)> m_confirmCallback;

private:
    bool m_stopped = false;
    bool m_finished = false;
};

} // namespace sy1000
