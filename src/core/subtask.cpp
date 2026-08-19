#include "core/subtask.h"

#include <QTimer>

namespace sy1000 {

HydroSubTask::HydroSubTask(IHydroDeviceProvider *device, QObject *parent)
    : QObject(parent)
    , m_device(device)
{
}

void HydroSubTask::start(const TaskParams &params)
{
    m_params = params;
    m_stopped = false;
    m_finished = false;
    run();
}

void HydroSubTask::stop()
{
    if (m_stopped)
        return;
    m_stopped = true;
    finish(false, HydroTestError::Cancelled);
}

void HydroSubTask::reset()
{
    m_stopped = true;
    m_finished = true;
}

void HydroSubTask::delay(int ms, std::function<void()> callback)
{
    if (m_stopped) {
        finish(false, HydroTestError::Cancelled);
        return;
    }
    QTimer::singleShot(ms, this, [this, callback]() {
        if (m_stopped) {
            finish(false, HydroTestError::Cancelled);
            return;
        }
        callback();
    });
}

void HydroSubTask::requestConfirmation(const QString &title, const QString &message,
                                       std::function<void(bool)> callback)
{
    if (m_stopped) {
        finish(false, HydroTestError::Cancelled);
        return;
    }
    m_confirmCallback = std::move(callback);
    emit requestConfirm(title, message);
}

void HydroSubTask::confirmResponse(bool accepted)
{
    if (!m_confirmCallback)
        return;
    auto cb = std::move(m_confirmCallback);
    m_confirmCallback = nullptr;
    cb(accepted);
}

void HydroSubTask::finish(bool success, HydroTestError error, const TaskResult &result)
{
    if (m_finished)
        return;
    m_finished = true;
    emit finished(success, error, result);
}

} // namespace sy1000
