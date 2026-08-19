#pragma once

#include <QObject>
#include <QString>

namespace sy1000 {

// QML bridge for the device maintenance page: system info, device connection,
// and manual controls (water jacket locks / water inlet / pumps).
class DeviceServiceAdapter : public QObject
{
    Q_OBJECT
public:
    explicit DeviceServiceAdapter(QObject *parent = nullptr);

    Q_INVOKABLE QString systemInfo() const;
    Q_INVOKABLE QString deviceName() const;
    Q_INVOKABLE QString manufacturer() const;
    Q_INVOKABLE QString connectDevices();   // returns status text
    Q_INVOKABLE void setWaterInlet(bool on);
    Q_INVOKABLE void setFastPump(bool on);
    Q_INVOKABLE void setSlowPump(bool on);
    Q_INVOKABLE void setWaterJacketLock(int index, bool on);
};

} // namespace sy1000
