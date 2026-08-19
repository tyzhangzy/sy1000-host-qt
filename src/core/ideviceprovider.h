#pragma once

#include <set>
#include <vector>

namespace sy1000 {

// Device capabilities needed by the state machine. The state machine and its
// sub-tasks depend only on this interface (decoupled from the concrete DeviceManager).
class IHydroDeviceProvider
{
public:
    virtual ~IHydroDeviceProvider() = default;

    // Note: currentPressure() is intentionally non-const — reading a live
    // sensor may advance simulation state (L10). Concrete providers must not
    // depend on const-correctness hacks (mutable members).
    virtual double currentPressure() = 0;
    virtual std::vector<double> currentWeights() const = 0;
    virtual std::set<int> availableScales() const = 0;

    virtual void setWaterInlet(bool on) = 0;
    virtual void setFastPump(bool on) = 0;
    virtual void setSlowPump(bool on) = 0;
    virtual void setWaterJacketLock(unsigned index, bool on) = 0;

    // Release-valve state. Default no-op for providers where the valve is
    // operated manually; simulated providers use it to model the pressure drop
    // during the release phase (L19).
    virtual void setReleaseValveOpen(bool /*open*/) {}
};

} // namespace sy1000
