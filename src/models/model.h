#pragma once

#include <QDateTime>
#include <QList>
#include <QString>

namespace sy1000 {

// ============================================================
// 枚举（对应原 WPF Models + Dao）
// ============================================================

enum class InspectionResult { Qualified, ToRepair, ToReplace, Scrapped };
enum class TestResultStatus { NotTested, InProgress, Passed, Failed, Qualified, ToRepair, ToReplace, Scrapped };
enum class HydroTestState {
    Idle, Preparing, WaterJacketChecking, CylinderChecking, Initializing,
    PressurizingToWorking, HoldingAtWorking, PressurizingToTesting, HoldingAtTesting,
    ReleasingPressure, Stabilizing, CalculatingResult, Completed, Aborted
};
enum class ComConnectionStatus { CONNECTING, CONNECTED, UNCONNECTED, ERROR };

// ============================================================
// 用户（对应 User）
// ============================================================
struct User {
    int id = 0;
    QString username;
    QString company;
    QString password;
    QDateTime createDate;
    int isAdmin = 0;
};

// ============================================================
// 试验标准（对应 TestStandard；压力/变形率用 -1 表示"未设置"）
// ============================================================
struct TestStandard {
    QString standardName;
    double workingPressure = -1;             // MPa
    double testingPressure = -1;             // MPa
    int pressureHoldingTime = 0;             // 秒
    double residualDeformationRate = -1;     // %
    double residualDeformation = -1;         // 残余变形量
};

// ============================================================
// 样品外观检查（对应 SampleInspectionData / SerializableSampleInspectionData）
// ============================================================
struct SampleInspectionData {
    // 基本信息
    QString sampleId;
    QString sampleModel;
    QString manufacturer;
    double volume = 6.8;
    QString userCompany;
    QString serialNo;
    QDateTime inspectionDate;
    QString inspectorName;
    QString inspectorCertNo;
    bool inspectionCompleted = false;

    // 四部分结果
    InspectionResult externalResult = InspectionResult::Qualified;
    InspectionResult internalResult = InspectionResult::Qualified;
    InspectionResult threadResult = InspectionResult::Qualified;
    InspectionResult valveResult = InspectionResult::Qualified;

    // External 外观检查
    bool externalThermalDamage = false;
    bool externalScratch = false;
    bool externalWear = false;
    bool externalDelamination = false;
    bool externalDeformation = false;
    QString externalDefectLocation;
    QString externalOther;

    // Internal 内部检查
    bool internalSmell = false;
    QString internalDebris;
    QString internalSurfaceCondition;
    QString internalDefectLocation;
    QString internalOther;

    // Thread 螺纹检查
    QString threadSpecification;
    QString threadCondition;
    QString threadEvaluation;
    QString threadOther;

    // Valve 瓶阀检查
    QString valveNo;
    QString valveThreadCondition;
    QString valveAirTightness;
    bool valveDiaphragmReplaced = false;
    QString valveOther;
};

// ============================================================
// 水压试验数据（对应 HydroStaticTestData / PressureWeightPoint）
// ============================================================
struct PressureWeightPoint {
    QDateTime timestamp;
    double pressure = 0;
    double weight = 0;
};

struct HydroStaticTestData {
    double initialWeight = 0;
    double pressureWeight = 0;
    double finalWeight = 0;
    double fullDeformation = 0;
    double residualDeformation = 0;
    double residualDeformationRate = 0;
    double workingPressure = 0;
    double testPressure = 0;
    QList<PressureWeightPoint> pressureWeightData;
    TestResultStatus testResult = TestResultStatus::NotTested;
    QString resultDetails;
};

// ============================================================
// 环境数据（对应 TestEnvironmentData）
// ============================================================
struct TestEnvironmentData {
    double roomTemperature = 0;
    double humidity = 0;
    QString equipmentId;
    QString equipmentModel;
};

// ============================================================
// 样品（对应 Sample）
// ============================================================
struct Sample {
    QString sampleId;
    QString sampleModel;
    QString manufacturer;
    QString serialNo;
    double volume = 0;
    SampleInspectionData appearanceInspection;
    HydroStaticTestData hydroStaticTest;
    TestResultStatus overallResult = TestResultStatus::NotTested;
    QString notes;
};

// ============================================================
// 统一试验结果（对应 UnifiedTestResult）
// ============================================================
struct UnifiedTestResult {
    int id = 0;
    QString testSerialNo;
    QDateTime testDate;
    QString testerName;
    QString testerCompany;
    TestStandard testStandard;
    Sample sample;
    TestEnvironmentData testEnvironment;
};

} // namespace sy1000
