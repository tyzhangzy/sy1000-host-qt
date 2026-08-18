#pragma once

#include <set>
#include <vector>

#include "core/ideviceprovider.h"

namespace sy1000 {

// Simulated device for demo/testing: pressure ramps up while a pump is on,
// weights are fixed. Lets the state machine run without real hardware.
class SimulatedDeviceProvider : public IHydroDeviceProvider
{
public:
    double currentPressure() const override
    {
        if (m_fast || m_slow)
            m_pressure += 2.0;
        return m_pressure;
    }
    std::vector<double> currentWeights() const override { return { 100.0, 200.0, 300.0, 400.0, 500.0 }; }
    std::set<int> availableScales() const override { return { 1, 2, 3, 4 }; }
    void setWaterInlet(bool) override {}
    void setFastPump(bool on) override { m_fast = on; }
    void setSlowPump(bool on) override { m_slow = on; }
    void setWaterJacketLock(unsigned, bool) override {}

    mutable double m_pressure = 0.0;
    bool m_fast = false;
    bool m_slow = false;
};

} // namespace sy1000
