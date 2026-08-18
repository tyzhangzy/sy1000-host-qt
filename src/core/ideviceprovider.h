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

    virtual double currentPressure() const = 0;
    virtual std::vector<double> currentWeights() const = 0;
    virtual std::set<int> availableScales() const = 0;

    virtual void setWaterInlet(bool on) = 0;
    virtual void setFastPump(bool on) = 0;
    virtual void setSlowPump(bool on) = 0;
    virtual void setWaterJacketLock(unsigned index, bool on) = 0;
};

} // namespace sy1000
