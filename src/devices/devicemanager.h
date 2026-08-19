#pragma once

#include <QObject>
#include <QString>

#include "devices/precisa.h"
#include "devices/tasio.h"

// Singleton device manager: owns TasIO + PrecisaScale.
class DeviceManager : public QObject
{
    Q_OBJECT
public:
    static DeviceManager &instance();

    TasIO &tasIO() { return m_tasIO; }
    PrecisaScale &precisa() { return m_precisa; }

private:
    DeviceManager();
    TasIO m_tasIO;
    PrecisaScale m_precisa;
};
