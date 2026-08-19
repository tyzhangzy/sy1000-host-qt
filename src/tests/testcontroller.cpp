// Headless smoke test for the hydrostatic state machine: runs the full 14-state
// flow with a mock device (fast pressure ramp) and short options, verifies it
// reaches Completed and computes the result.
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>

#include <cstdio>
#include <set>
#include <vector>

#include "core/controller.h"
#include "core/ideviceprovider.h"

using namespace sy1000;

class MockDevice : public IHydroDeviceProvider
{
public:
    double currentPressure() const override
    {
        if (m_fast || m_slow)
            m_pressure += 5.0; // fast ramp so pressurize finishes quickly
        return m_pressure;
    }
    std::vector<double> currentWeights() const override { return { 100.0, 200.0, 300.0, 400.0, 500.0 }; }
    std::set<int> availableScales() const override { return { 1, 2 }; }
    void setWaterInlet(bool) override {}
    void setFastPump(bool on) override { m_fast = on; }
    void setSlowPump(bool on) override { m_slow = on; }
    void setWaterJacketLock(unsigned, bool) override {}

    mutable double m_pressure = 0.0;
    bool m_fast = false;
    bool m_slow = false;
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

    int stateCount = 0;
    bool completed = false;
    QObject::connect(&ctl, &HydrostaticTestController::stateChanged,
                     [&](HydroTestState s) { ++stateCount; std::printf("  state=%d\n", static_cast<int>(s)); });
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
    const bool ok = completed &&
                    ctl.state() == HydroTestState::Completed &&
                    stateCount >= 10 &&
                    data.results[1] == TestResultStatus::Passed;

    std::printf("final state=%d completed=%d states=%d result1=%d rate1=%.2f\n",
                static_cast<int>(ctl.state()), completed, stateCount,
                static_cast<int>(data.results[1]), data.residualDeformationRates[1]);
    std::printf(ok ? "CONTROLLER SMOKE PASS\n" : "CONTROLLER SMOKE FAIL\n");
    return ok ? 0 : 1;
}
