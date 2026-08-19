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
    // Report the software + two COM ports (TasIO / PrecisaScale) status.
    const QString tasPort = QStringLiteral("COM1");
    const QString scalePort = QStringLiteral("COM2");
    const bool tasOk = dm.tasIO().connect(tasPort);
    const bool scaleOk = dm.precisa().connect(scalePort);
    const QString okS = QStringLiteral("已连接");
    const QString noS = QStringLiteral("未连接");
    return QStringLiteral("%1\nTasIO (%2): %3\n天平 (%4): %5")
        .arg(ConfigManager::deviceName(), tasPort, tasOk ? okS : noS,
             scalePort, scaleOk ? okS : noS);
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
