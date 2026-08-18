#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <QTimer>

#include "devices/modbusrtu.h"

// Precisa electronic scales over Modbus RTU (one slave per scale).
// Polls the configured scales periodically and emits weightUpdated(scaleNo, weight).
class PrecisaScale : public QObject
{
    Q_OBJECT
public:
    explicit PrecisaScale(QObject *parent = nullptr);

    static constexpr int BAUD_RATE = 19200;
    static constexpr int INTER_SLAVE_DELAY_MS = 25;
    static constexpr int POLL_INTERVAL_MS = 50;

    bool connect(const QString &portName);
    void disconnect();
    bool isConnected() const;

    void setAvailableScales(const QSet<int> &scales) { m_scales = scales; }
    QSet<int> availableScales() const { return m_scales; }

    void startContinuousReading();
    void stopContinuousReading();

signals:
    void weightUpdated(int scaleNo, double weight);

private:
    void pollOnce();

    ModbusRtuClient m_client;
    QSet<int> m_scales;
    QTimer m_timer;
};
