#include "devices/devicemanager.h"

#include <QDebug>

DeviceManager &DeviceManager::instance()
{
    static DeviceManager inst;
    return inst;
}

DeviceManager::DeviceManager()
    : QObject(nullptr)
    , m_weights(5, 0.0)
{
    // Move the scale to a dedicated worker thread so polling never blocks the
    // GUI thread (H4). QObject::moveToThread must happen before connections.
    m_precisa.moveToThread(&m_scaleThread);
    connect(&m_precisa, &PrecisaScale::weightUpdated,
            this, &DeviceManager::onWeightUpdated, Qt::QueuedConnection);
    m_scaleThread.start();
}

DeviceManager::~DeviceManager()
{
    m_precisa.stopContinuousReading();
    m_scaleThread.quit();
    m_scaleThread.wait(2000);
    // Move the scale back to the main thread before destruction so its QObject
    // cleanup happens safely after the worker thread has stopped.
    m_precisa.moveToThread(QThread::currentThread());
}

QVector<double> DeviceManager::currentWeights() const
{
    QMutexLocker lock(&m_weightMutex);
    return m_weights;
}

void DeviceManager::onWeightUpdated(int scaleNo, double weight)
{
    QMutexLocker lock(&m_weightMutex);
    if (scaleNo >= 1 && scaleNo < m_weights.size())
        m_weights[scaleNo] = weight;
}
