#pragma once

#include <QMutex>
#include <QObject>
#include <QString>
#include <QThread>
#include <QVector>

#include "devices/precisa.h"
#include "devices/tasio.h"

// Singleton device manager: owns TasIO + PrecisaScale.
// The scale is polled on a dedicated worker thread so Modbus I/O does not
// block the GUI thread (H4). Weights are cached here for the provider.
class DeviceManager : public QObject
{
    Q_OBJECT
public:
    static DeviceManager &instance();
    ~DeviceManager();

    TasIO &tasIO() { return m_tasIO; }
    PrecisaScale &precisa() { return m_precisa; }

    // Latest weight cache (index 1..4). Updated from the scale worker thread.
    QVector<double> currentWeights() const;

public slots:
    void onWeightUpdated(int scaleNo, double weight);

private:
    DeviceManager();
    TasIO m_tasIO;
    PrecisaScale m_precisa;
    QThread m_scaleThread;
    mutable QMutex m_weightMutex;
    QVector<double> m_weights;
};
