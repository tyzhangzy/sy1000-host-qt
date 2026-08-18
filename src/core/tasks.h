#pragma once

#include <QDateTime>
#include <QTimer>

#include "core/subtask.h"

namespace sy1000 {

// Pressurize to a target pressure: fast pump until target-FastStopDelta,
// then (if useSlowPump) switch to slow pump until target-SlowStopDelta.
class PressurizeTask : public HydroSubTask
{
    Q_OBJECT
public:
    using HydroSubTask::HydroSubTask;

protected:
    void run() override;

private:
    void checkFastStage();
    void checkSlowStage();
    void stopPumps();

    QTimer m_timer;
    int m_stage = 0; // 1 = fast, 2 = slow
    double m_fastTarget = 0;
    double m_slowTarget = 0;
    QDateTime m_fastDeadline;
    QDateTime m_slowDeadline;
};

// Wait a fixed countdown (seconds), reporting remaining seconds as status.
class WaitTask : public HydroSubTask
{
    Q_OBJECT
public:
    using HydroSubTask::HydroSubTask;

protected:
    void run() override;

private:
    void tick();

    int m_remaining = 0;
    QTimer m_timer;
};

// Hold pressure for holdSampleInterval1Sec + holdSampleInterval2Sec, sampling
// weights/pressure at T10 and T30 and checking for leaks.
class HoldTask : public HydroSubTask
{
    Q_OBJECT
public:
    using HydroSubTask::HydroSubTask;

protected:
    void run() override;

private:
    void sampleFirst();
    void sampleSecond();
    void finishWithResult();

    std::array<double, 5> m_w10{};
    std::array<double, 5> m_w30{};
    double m_p10 = 0;
    double m_p30 = 0;
};

// Release pressure: waits for countdownSec (operator opens the release valve).
class ReleaseTask : public HydroSubTask
{
    Q_OBJECT
public:
    using HydroSubTask::HydroSubTask;

protected:
    void run() override;

private:
    void tick();
    int m_remaining = 0;
    QTimer m_timer;
};

// Wait for the scale readings to stabilize (countdown).
class StabilizeTask : public HydroSubTask
{
    Q_OBJECT
public:
    using HydroSubTask::HydroSubTask;

protected:
    void run() override;

private:
    void tick();
    int m_remaining = 0;
    QTimer m_timer;
};

} // namespace sy1000
