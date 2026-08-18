#include "core/tasks.h"

#include <QDateTime>
#include <cmath>

namespace sy1000 {

// ---------------- PressurizeTask ----------------
void PressurizeTask::run()
{
    m_stage = 1;
    m_fastTarget = m_params.targetPressure - m_params.fastStopDelta;
    m_fastDeadline = QDateTime::currentDateTime().addSecs(m_params.fastTimeoutMinutes * 60);

    m_device->setWaterInlet(true);
    m_device->setFastPump(true);
    emit statusChanged(QStringLiteral("Pressurizing (fast)..."));

    m_timer.setInterval(200);
    QObject::connect(&m_timer, &QTimer::timeout, this, &PressurizeTask::checkFastStage);
    m_timer.start();
}

void PressurizeTask::checkFastStage()
{
    if (stopped())
        return;

    if (m_device->currentPressure() >= m_fastTarget) {
        m_device->setFastPump(false);
        if (m_params.useSlowPump) {
            m_stage = 2;
            m_slowTarget = m_params.targetPressure - m_params.slowStopDelta;
            m_slowDeadline = QDateTime::currentDateTime().addSecs(m_params.slowTimeoutMinutes * 60);
            m_device->setSlowPump(true);
            emit statusChanged(QStringLiteral("Pressurizing (slow)..."));
            QObject::disconnect(&m_timer, &QTimer::timeout, this, &PressurizeTask::checkFastStage);
            QObject::connect(&m_timer, &QTimer::timeout, this, &PressurizeTask::checkSlowStage);
            return;
        }
        m_device->setWaterInlet(false);
        finish(true, HydroTestError::None);
        m_timer.stop();
        return;
    }

    if (QDateTime::currentDateTime() > m_fastDeadline) {
        stopPumps();
        finish(false, HydroTestError::Timeout);
        m_timer.stop();
    }
}

void PressurizeTask::checkSlowStage()
{
    if (stopped())
        return;

    if (m_device->currentPressure() >= m_slowTarget) {
        m_device->setSlowPump(false);
        m_device->setWaterInlet(false);
        finish(true, HydroTestError::None);
        m_timer.stop();
        return;
    }

    if (QDateTime::currentDateTime() > m_slowDeadline) {
        stopPumps();
        finish(false, HydroTestError::Timeout);
        m_timer.stop();
    }
}

void PressurizeTask::stopPumps()
{
    m_device->setFastPump(false);
    m_device->setSlowPump(false);
    m_device->setWaterInlet(false);
}

// ---------------- WaitTask ----------------
void WaitTask::run()
{
    m_remaining = m_params.countdownSec;
    if (m_remaining <= 0) {
        finish(true, HydroTestError::None);
        return;
    }
    emit statusChanged(QString("Holding, %1 s").arg(m_remaining));
    m_timer.setInterval(1000);
    QObject::connect(&m_timer, &QTimer::timeout, this, &WaitTask::tick);
    m_timer.start();
}

void WaitTask::tick()
{
    if (stopped())
        return;
    --m_remaining;
    if (m_remaining <= 0) {
        m_timer.stop();
        finish(true, HydroTestError::None);
    } else {
        emit statusChanged(QString("Holding, %1 s").arg(m_remaining));
    }
}

// ---------------- HoldTask ----------------
void HoldTask::run()
{
    emit statusChanged(QStringLiteral("Hold sampling (T10)..."));
    delay(m_params.holdSampleInterval1Sec * 1000, [this]() { sampleFirst(); });
}

void HoldTask::sampleFirst()
{
    if (stopped())
        return;
    const auto w = m_device->currentWeights();
    for (std::size_t i = 0; i < w.size() && i < m_w10.size(); ++i)
        m_w10[i] = w[i];
    m_p10 = m_device->currentPressure();

    emit statusChanged(QStringLiteral("Hold sampling (T30)..."));
    delay(m_params.holdSampleInterval2Sec * 1000, [this]() { sampleSecond(); });
}

void HoldTask::sampleSecond()
{
    if (stopped())
        return;
    const auto w = m_device->currentWeights();
    for (std::size_t i = 0; i < w.size() && i < m_w30.size(); ++i)
        m_w30[i] = w[i];
    m_p30 = m_device->currentPressure();
    finishWithResult();
}

void HoldTask::finishWithResult()
{
    TaskResult r;
    r.weightT10 = m_w10;
    r.weightT30 = m_w30;
    r.pressureT10 = m_p10;
    r.pressureT30 = m_p30;
    r.pressureLeaking = std::abs(m_p30 - m_p10) > 0.5;
    for (int i : m_device->availableScales()) {
        if (i >= 0 && i < 5 && std::abs(m_w30[i] - m_w10[i]) > 0.5)
            r.leakingWaterJackets.insert(i);
    }
    if (r.pressureLeaking || !r.leakingWaterJackets.empty())
        finish(false, HydroTestError::LeakDetected, r);
    else
        finish(true, HydroTestError::None, r);
}

// ---------------- ReleaseTask ----------------
void ReleaseTask::run()
{
    m_remaining = m_params.countdownSec > 0 ? m_params.countdownSec : 10;
    emit statusChanged(QString("Releasing, %1 s").arg(m_remaining));
    m_timer.setInterval(1000);
    QObject::connect(&m_timer, &QTimer::timeout, this, &ReleaseTask::tick);
    m_timer.start();
}

void ReleaseTask::tick()
{
    if (stopped())
        return;
    --m_remaining;
    if (m_remaining <= 0) {
        m_timer.stop();
        finish(true, HydroTestError::None);
    } else {
        emit statusChanged(QString("Releasing, %1 s").arg(m_remaining));
    }
}

// ---------------- StabilizeTask ----------------
void StabilizeTask::run()
{
    m_remaining = m_params.countdownSec > 0 ? m_params.countdownSec : 5;
    emit statusChanged(QString("Stabilizing, %1 s").arg(m_remaining));
    m_timer.setInterval(1000);
    QObject::connect(&m_timer, &QTimer::timeout, this, &StabilizeTask::tick);
    m_timer.start();
}

void StabilizeTask::tick()
{
    if (stopped())
        return;
    --m_remaining;
    if (m_remaining <= 0) {
        m_timer.stop();
        finish(true, HydroTestError::None);
    } else {
        emit statusChanged(QString("Stabilizing, %1 s").arg(m_remaining));
    }
}

} // namespace sy1000
