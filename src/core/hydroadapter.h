#pragma once

#include <QObject>
#include <QString>

#include "core/controller.h"
#include "core/ideviceprovider.h"

namespace sy1000 {

// QML bridge for the hydrostatic test controller. Exposes start/stop, current
// state/status and a finished signal so the QML test page can drive the flow.
class HydroTestControllerAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
    explicit HydroTestControllerAdapter(IHydroDeviceProvider *device, QObject *parent = nullptr);

    Q_INVOKABLE void startTest();
    Q_INVOKABLE void stopTest();
    Q_INVOKABLE void setWorkingPressure(double p);
    Q_INVOKABLE void setTestingPressure(double p);

    int state() const;
    QString status() const { return m_status; }
    bool running() const;

signals:
    void stateChanged();
    void statusChanged();
    void runningChanged();
    void testFinished(bool ok, int passed, int failed);

private:
    int countPassed() const;
    int countFailed() const;
    void updateRunning();

    HydrostaticTestController m_controller;
    QString m_status;
};

} // namespace sy1000
