#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "core/controller.h"
#include "core/ideviceprovider.h"
#include "models/model.h"

namespace sy1000 {

// QML bridge for the hydrostatic test controller. Exposes start/stop, current
// state/status, samples pressure for the realtime chart, and persists the
// result to SQLite when the test completes.
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
    Q_INVOKABLE void setTester(const QString &name, const QString &company);

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
    UnifiedTestResult buildResult() const;

    HydrostaticTestController m_controller;
    QString m_status;
    QString m_testerName;
    QString m_testerCompany;
    QTimer m_sampleTimer;
};

} // namespace sy1000
