#include "devices/tasio.h"

#include <QDebug>

TasIO::TasIO(QObject *parent)
    : QObject(parent)
{
}

float TasIO::convertFrom2143Format(quint16 reg0, quint16 reg1)
{
    // 2143: assemble the 4 bytes in order [reg0.lo, reg0.hi, reg1.lo, reg1.hi]
    // then reinterpret as a little-endian float. Calibrate per hardware.
    quint8 bytes[4] = {
        static_cast<quint8>(reg0 & 0xFF),
        static_cast<quint8>((reg0 >> 8) & 0xFF),
        static_cast<quint8>(reg1 & 0xFF),
        static_cast<quint8>((reg1 >> 8) & 0xFF),
    };
    float value = 0.0f;
    memcpy(&value, bytes, 4);
    return value;
}

bool TasIO::connect(const QString &portName)
{
    if (!m_client.connectPort(portName, BAUD_RATE)) {
        qWarning() << "[tasio] connect failed:" << portName;
        return false;
    }
    return true;
}

void TasIO::disconnect()
{
    m_client.disconnectPort();
}

bool TasIO::isConnected() const
{
    return m_client.isConnected();
}

void TasIO::setWaterInlet(bool on)
{
    if (isConnected())
        m_client.writeSingleCoil(TAS_IO_SLAVE_ID, 5, on);
}

void TasIO::setFastPump(bool on)
{
    if (isConnected())
        m_client.writeSingleCoil(TAS_IO_SLAVE_ID, 6, on);
}

void TasIO::setSlowPump(bool on)
{
    if (isConnected())
        m_client.writeSingleCoil(TAS_IO_SLAVE_ID, 7, on);
}

void TasIO::setWaterJacketLock(quint16 index, bool on)
{
    // coil addresses 0..3 for locks 1..4
    if (isConnected() && index >= 1 && index <= 4)
        m_client.writeSingleCoil(TAS_IO_SLAVE_ID, index - 1, on);
}

void TasIO::readPressureOnce()
{
    QVector<quint16> regs;
    if (m_client.readHoldingRegisters(PRESSURE_SLAVE_ID, PRESSURE_START_ADDR, PRESSURE_REG_COUNT, regs) && regs.size() == 2) {
        const float p = convertFrom2143Format(regs.at(0), regs.at(1));
        m_currentPressure = static_cast<double>(p);
        emit pressureUpdated(m_currentPressure);
    }
}
