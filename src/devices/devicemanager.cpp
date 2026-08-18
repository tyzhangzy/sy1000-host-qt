#include "devices/devicemanager.h"

#include <QDebug>

DeviceManager &DeviceManager::instance()
{
    static DeviceManager inst;
    return inst;
}

DeviceManager::DeviceManager() = default;

bool DeviceManager::connectAll(const QString &tasPort, const QString &scalePort)
{
    const bool tasOk = m_tasIO.connect(tasPort);
    const bool scaleOk = m_precisa.connect(scalePort);
    qWarning() << "[devices] tasIO connected:" << tasOk << "| precisa connected:" << scaleOk;
    return tasOk && scaleOk;
}
