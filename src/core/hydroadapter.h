#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "core/controller.h"
#include "core/ideviceprovider.h"

namespace sy1000 {

// QML bridge for the hydrostatic test controller. Exposes start/stop, current
// state/status and a finished signal so the QML test page can drive the flow.
// Also samples the pressure periodically for the realtime chart.
class HydroTestControllerAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(double currentPressure READ currentPressure NOTIFY pressureSample)

public:
    explicit HydroTestControllerAdapter(IHydroDeviceProvider *device, QObject *parent = nullptr);

    Q_INVOKABLE void startTest();
    Q_INVOKABLE void stopTest();
    Q_INVOKABLE void setWorkingPressure(double p);
    Q_INVOKABLE void setTestingPressure(double p);

    int state() const;
    QString status() const { return m_status; }
    bool running() const;
    double currentPressure() const { return m_controller.device()->currentPressure(); }

signals:
    void stateChanged();
    void statusChanged();
    void runningChanged();
    void testFinished(bool ok, int passed, int failed);
    void pressureSample(double value);

private:
    int countPassed() const;
    int countFailed() const;
    void updateRunning();

    HydrostaticTestController m_controller;
    QString m_status;
    QTimer m_sampleTimer;
};

} // namespace sy1000
