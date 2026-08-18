#pragma once

#include <QObject>
#include <QString>

#include "devices/modbusrtu.h"

// TasIO428R2 digital-I/O module over Modbus RTU (slave 17).
// Controls water inlet / fast pump / slow pump / water jacket locks, and
// reads the pressure transmitter (slave 1, registers 0-1).
class TasIO : public QObject
{
    Q_OBJECT
public:
    explicit TasIO(QObject *parent = nullptr);

    static constexpr int BAUD_RATE = 19200;
    static constexpr int TAS_IO_SLAVE_ID = 17;
    static constexpr int PRESSURE_SLAVE_ID = 1;
    static constexpr int PRESSURE_START_ADDR = 0;
    static constexpr int PRESSURE_REG_COUNT = 2;

    bool connect(const QString &portName);
    void disconnect();
    bool isConnected() const;

    void setWaterInlet(bool on);
    void setFastPump(bool on);
    void setSlowPump(bool on);
    void setWaterJacketLock(quint16 index, bool on);

    double currentPressure() const { return m_currentPressure; }

    // Poll once and emit pressureUpdated if changed.
    void readPressureOnce();

signals:
    void pressureUpdated(double pressure);

private:
    // Convert two 16-bit registers (byte order 2-1-4-3) to a float pressure.
    static float convertFrom2143Format(quint16 reg0, quint16 reg1);

    ModbusRtuClient m_client;
    double m_currentPressure = 0.0;
};
