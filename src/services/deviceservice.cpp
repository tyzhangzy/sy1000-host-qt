#include "services/deviceservice.h"

#include "devices/devicemanager.h"
#include "services/configmanager.h"

namespace sy1000 {

DeviceServiceAdapter::DeviceServiceAdapter(QObject *parent)
    : QObject(parent)
{
}

QString DeviceServiceAdapter::systemInfo() const
{
    // Loaded from config.json (ConfigManager), with defaults when absent.
    return QStringLiteral("Device Name: %1\n"
                          "Manufacturer: %2\n"
                          "Serial No: %3\n"
                          "Manufacture Date: %4")
        .arg(ConfigManager::deviceName(),
             ConfigManager::manufacturer(),
             ConfigManager::serialNo(),
             ConfigManager::manufactureDate());
}

QString DeviceServiceAdapter::deviceName() const
{
    return ConfigManager::deviceName();
}

QString DeviceServiceAdapter::manufacturer() const
{
    return ConfigManager::manufacturer();
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
