#pragma once

#include <algorithm>
#include <set>
#include <vector>

#include "core/ideviceprovider.h"

namespace sy1000 {

// Simulated device for demo/testing: pressure ramps up while a pump is on,
// decays when the release valve is open, weights are fixed. Lets the state
// machine run without real hardware.
class SimulatedDeviceProvider : public IHydroDeviceProvider
{
public:
    double currentPressure() override
    {
        if (m_fast || m_slow) {
            m_pressure += 2.0;
        } else if (m_releaseValve && m_pressure > 0.0) {
            // Model the pressure dropping while the release valve is open (L19).
            m_pressure = std::max(0.0, m_pressure - 8.0);
        }
        return m_pressure;
    }
    std::vector<double> currentWeights() const override { return { 100.0, 200.0, 300.0, 400.0, 500.0 }; }
    std::set<int> availableScales() const override { return { 1, 2, 3, 4 }; }
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

} // namespace sy1000
