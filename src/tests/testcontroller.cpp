// Headless smoke test for the hydrostatic state machine: runs the full 14-state
// flow with a mock device (fast pressure ramp) and short options, verifies it
// reaches Completed, computes the result, and that the release phase actually
// waits for the pressure to drop (L19). State assertions use milestones instead
// of a fragile "stateCount >= 10" check (L6).
#include <QCoreApplication>
#include <QEventLoop>
#include <QSet>
#include <QTimer>

#include <algorithm>
#include <cstdio>
#include <set>
#include <vector>

#include "core/controller.h"
#include "core/ideviceprovider.h"

using namespace sy1000;

class MockDevice : public IHydroDeviceProvider
{
public:
    double currentPressure() override
    {
        if (m_fast || m_slow) {
            m_pressure += 5.0; // fast ramp so pressurize finishes quickly
        } else if (m_releaseValve && m_pressure > 0.0) {
            // Pressure drops while the release valve is open (L19).
            m_pressure = std::max(0.0, m_pressure - 8.0);
        }
        return m_pressure;
    }
    std::vector<double> currentWeights() const override { return { 100.0, 200.0, 300.0, 400.0, 500.0 }; }
    std::set<int> availableScales() const override { return { 1, 2 }; }
    void setWaterInlet(bool) override {}
    void setFastPump(bool on) override { m_fast = on; }
    void setSlowPump(bool on) override { m_slow = on; }
    void setWaterJacketLock(unsigned, bool) override {}
    void setReleaseValveOpen(bool open) override { m_releaseValve = open; }

    double m_pressure = 0.0;
    bool m_fast = false;
    bool m_slow = false;
    bool m_releaseValve = false;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    MockDevice dev;
    HydrostaticTestController ctl(&dev);

    TestOptions opt;
    opt.checkCountdownSec = 1;
    opt.holdWorking1Sec = 1;
    opt.holdWorking2Sec = 1;
    opt.holdTestingSec = 1;
    opt.releaseSec = 1;
    opt.stabilizeSec = 1;
    ctl.setOptions(opt);
    ctl.setResidualDeformationRateLimit(3.0);

    QSet<int> visited;
    bool completed = false;
    QObject::connect(&ctl, &HydrostaticTestController::stateChanged,
                     [&](HydroTestState s) {
                         visited.insert(static_cast<int>(s));
                         std::printf("  state=%d\n", static_cast<int>(s));
                     });
    QObject::connect(&ctl, &HydrostaticTestController::testCompleted,
                     [&](bool ok) { completed = true; std::printf("  testCompleted ok=%d\n", ok); });
    QObject::connect(&ctl, &HydrostaticTestController::testAborted,
                     [&](HydroTestError e, const QString &m) {
                         std::printf("  ABORTED err=%d msg=%s\n", static_cast<int>(e), qPrintable(m));
                     });
    // Headless run: auto-accept any operator confirmation (e.g. release valve).
    QObject::connect(&ctl, &HydrostaticTestController::confirmRequested,
                     [&](const QString &title, const QString &msg) {
                         std::printf("  confirm: %s | %s -> auto-accept\n",
                                     qPrintable(title), qPrintable(msg));
                         ctl.respondConfirm(true);
                     });

    QEventLoop loop;
    QTimer::singleShot(30000, &loop, &QEventLoop::quit); // timeout guard
    QObject::connect(&ctl, &HydrostaticTestController::testCompleted, &loop, &QEventLoop::quit);
    QObject::connect(&ctl, &HydrostaticTestController::testAborted, &loop, &QEventLoop::quit);

    ctl.startTest();
    loop.exec();

    const auto &data = ctl.testData();
    // Milestones that the full flow must have passed through (L6).
    const bool milestones = visited.contains(static_cast<int>(HydroTestState::PressurizingToWorking)) &&
                            visited.contains(static_cast<int>(HydroTestState::HoldingAtTesting)) &&
                            visited.contains(static_cast<int>(HydroTestState::CalculatingResult)) &&
                            visited.contains(static_cast<int>(HydroTestState::Completed));
    // The release phase must have actually dropped the cylinder pressure (L19).
    const bool pressureReleased = dev.currentPressure() <= 1.0;
    const bool ok = completed &&
                    ctl.state() == HydroTestState::Completed &&
                    milestones &&
                    pressureReleased &&
                    data.results[1] == TestResultStatus::Passed;

    std::printf("final state=%d completed=%d milestones=%d released=%d result1=%d rate1=%.2f pressure=%.2f\n",
                static_cast<int>(ctl.state()), completed, milestones, pressureReleased,
                static_cast<int>(data.results[1]), data.residualDeformationRates[1], dev.currentPressure());
    std::printf(ok ? "CONTROLLER SMOKE PASS\n" : "CONTROLLER SMOKE FAIL\n");
    return ok ? 0 : 1;
}
