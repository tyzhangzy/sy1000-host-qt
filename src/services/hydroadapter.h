#pragma once

#include <array>
#include <vector>

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
    Q_PROPERTY(QString stateName READ stateName NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(double currentPressure READ currentPressure NOTIFY pressureSample)
    // Id of the result archived by the most recent completed test (0 = none);
    // used by the "View Test Report" button on the test pages (M5).
    Q_PROPERTY(int lastResultId READ lastResultId NOTIFY lastResultIdChanged)

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
    // Test-standard text fields (name/hold/rate) captured on the prep page.
    Q_INVOKABLE void setTestStandard(const QString &name, int holdTime, int residualRate);
    // Persist the current result now; returns the result id. Safe to call
    // repeatedly (the test result is saved at most once per test run).
    Q_INVOKABLE int saveCurrentResult();
    // Formatted strings shown in the left column of the test page.
    Q_INVOKABLE QString testStandardInfo() const;
    Q_INVOKABLE QString sampleInfo(int index) const;

    int state() const;
    // Localized human-readable name of the current HydroTestState (M7);
    // exposed both as a Q_PROPERTY (NOTIFY stateChanged) and an invokable.
    QString stateName() const;
    QString status() const { return m_status; }
    bool running() const;
    double currentPressure() const { return m_controller.device()->currentPressure(); }
    int lastResultId() const { return m_lastResultId; }

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
    void lastResultIdChanged();

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
    // Test-standard fields (from the preparation page).
    QString m_standardName = QStringLiteral("GB/T9251-2022");
    int m_holdTime = 30;
    double m_residualRate = 3.0;
    // Sampled pressure/weight points collected during the test (report curve).
    std::vector<PressureWeightPoint> m_curvePoints;
    QTimer m_sampleTimer;
    int m_lastResultId = 0;
    bool m_resultSaved = false;
};

} // namespace sy1000
