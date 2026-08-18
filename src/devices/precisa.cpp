#include "devices/precisa.h"

#include <QDebug>

PrecisaScale::PrecisaScale(QObject *parent)
    : QObject(parent)
{
    m_timer.setInterval(POLL_INTERVAL_MS);
    QObject::connect(&m_timer, &QTimer::timeout, this, &PrecisaScale::pollOnce);
}

bool PrecisaScale::connect(const QString &portName)
{
    if (!m_client.connectPort(portName, BAUD_RATE)) {
        qWarning() << "[precisa] connect failed:" << portName;
        return false;
    }
    return true;
}

void PrecisaScale::disconnect()
{
    stopContinuousReading();
    m_client.disconnectPort();
}

bool PrecisaScale::isConnected() const
{
    return m_client.isConnected();
}

void PrecisaScale::startContinuousReading()
{
    if (m_client.isConnected() && !m_timer.isActive())
        m_timer.start();
}

void PrecisaScale::stopContinuousReading()
{
    m_timer.stop();
}

void PrecisaScale::pollOnce()
{
    // Slave id is the scale number (1..4). Register 0 holds the weight.
    for (const int scaleNo : m_scales) {
        QVector<quint16> regs;
        if (m_client.readHoldingRegisters(scaleNo, 0, 1, regs) && !regs.isEmpty()) {
            const double weight = static_cast<double>(regs.at(0)) / 1000.0; // g -> kg, calibrate per hardware
            emit weightUpdated(scaleNo, weight);
        }
    }
}
