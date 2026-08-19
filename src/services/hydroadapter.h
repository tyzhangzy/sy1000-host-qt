#pragma once

#include <array>

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantMap>

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
    // Forward the operator's answer (OK/Cancel) to the waiting sub-task.
    Q_INVOKABLE void respondConfirm(bool accepted);
    // Sample info (index 1..4) used when persisting the result.
    Q_INVOKABLE void setSample(int index, const QString &model, const QString &manufacturer,
                               const QString &serialNo, double volume);
    // Full appearance inspection data (index 1..4) captured on the prep page;
    // written into the saved result via buildResult().
    Q_INVOKABLE void setSampleInspection(int index, const QVariantMap &inspection);

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
    // Per-sample weight/deformation sample (index 1..4) for the right Y axis.
    void weightSample(int index, double value);
    // Operator instruction/confirmation requested by the active sub-task.
    void confirmRequested(const QString &title, const QString &message);

private:
    struct SampleInfo {
        QString model;
        QString manufacturer;
        QString serialNo;
        double volume = 0;
    };

    int countPassed() const;
    int countFailed() const;
    void updateRunning();
    UnifiedTestResult buildResult() const;

    HydrostaticTestController m_controller;
    QString m_status;
    QString m_testerName;
    QString m_testerCompany;
    std::array<SampleInfo, 5> m_samples;
    std::array<SampleInspectionData, 5> m_inspections;
    QTimer m_sampleTimer;
};

} // namespace sy1000
