#include "services/deviceservice.h"

#include "devices/devicemanager.h"

namespace sy1000 {

DeviceServiceAdapter::DeviceServiceAdapter(QObject *parent)
    : QObject(parent)
{
}

QString DeviceServiceAdapter::systemInfo() const
{
    // Placeholder system info (could be loaded from config.json later).
    return QStringLiteral("Device Name: SY1000\n"
                          "Manufacturer: Quanshen\n"
                          "Serial No: SY1000-0001\n"
                          "Manufacture Date: 2026-01-01");
}

QString DeviceServiceAdapter::connectDevices()
{
    auto &dm = DeviceManager::instance();
    // Real COM ports required; without hardware this reports failure.
    const bool ok = dm.connectAll(QStringLiteral("COM1"), QStringLiteral("COM2"));
    return ok ? QStringLiteral("TasIO + scales connected")
              : QStringLiteral("Connection failed (check COM ports / hardware)");
}

void DeviceServiceAdapter::setWaterInlet(bool on)
{
    DeviceManager::instance().tasIO().setWaterInlet(on);
}

void DeviceServiceAdapter::setFastPump(bool on)
{
    DeviceManager::instance().tasIO().setFastPump(on);
}

void DeviceServiceAdapter::setSlowPump(bool on)
{
    DeviceManager::instance().tasIO().setSlowPump(on);
}

void DeviceServiceAdapter::setWaterJacketLock(int index, bool on)
{
    DeviceManager::instance().tasIO().setWaterJacketLock(static_cast<quint16>(index), on);
}

} // namespace sy1000
