#include "devices/devicemanager.h"

#include <QDebug>

DeviceManager &DeviceManager::instance()
{
    static DeviceManager inst;
    return inst;
}

DeviceManager::DeviceManager() = default;
