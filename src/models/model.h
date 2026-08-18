#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace sy1000 {

// Time point alias (UTC epoch; serializable as sec/ms timestamp, cross-platform)
using DateTime = std::chrono::system_clock::time_point;

// ============================================================
// Enums (mapped from original WPF Models + Dao)
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
// User
// ============================================================
struct User {
    int id = 0;
    std::string username;
    std::string company;
    std::string password;
    DateTime createDate;
    int isAdmin = 0;
};

// ============================================================
// TestStandard (pressure/rate use -1 for "not set")
// ============================================================
struct TestStandard {
    std::string standardName;
    double workingPressure = -1;             // MPa
    double testingPressure = -1;             // MPa
    int pressureHoldingTime = 0;             // seconds
    double residualDeformationRate = -1;     // %
    double residualDeformation = -1;         // residual deformation
};

// ============================================================
// SampleInspectionData (appearance/internal/thread/valve inspection)
// ============================================================
struct SampleInspectionData {
    // Basic info
    std::string sampleId;
    std::string sampleModel;
    std::string manufacturer;
    double volume = 6.8;
    std::string userCompany;
    std::string serialNo;
    DateTime inspectionDate;
    std::string inspectorName;
    std::string inspectorCertNo;
    bool inspectionCompleted = false;

    // Four section results
    InspectionResult externalResult = InspectionResult::Qualified;
    InspectionResult internalResult = InspectionResult::Qualified;
    InspectionResult threadResult = InspectionResult::Qualified;
    InspectionResult valveResult = InspectionResult::Qualified;

    // External inspection
    bool externalThermalDamage = false;
    bool externalScratch = false;
    bool externalWear = false;
    bool externalDelamination = false;
    bool externalDeformation = false;
    std::string externalDefectLocation;
    std::string externalOther;

    // Internal inspection
    bool internalSmell = false;
    std::string internalDebris;
    std::string internalSurfaceCondition;
    std::string internalDefectLocation;
    std::string internalOther;

    // Thread inspection
    std::string threadSpecification;
    std::string threadCondition;
    std::string threadEvaluation;
    std::string threadOther;

    // Valve inspection
    std::string valveNo;
    std::string valveThreadCondition;
    std::string valveAirTightness;
    bool valveDiaphragmReplaced = false;
    std::string valveOther;
};

// ============================================================
// Hydrostatic test data (HydroStaticTestData / PressureWeightPoint)
// ============================================================
struct PressureWeightPoint {
    DateTime timestamp;
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
    std::vector<PressureWeightPoint> pressureWeightData;
    TestResultStatus testResult = TestResultStatus::NotTested;
    std::string resultDetails;
};

// ============================================================
// Environment data (TestEnvironmentData)
// ============================================================
struct TestEnvironmentData {
    double roomTemperature = 0;
    double humidity = 0;
    std::string equipmentId;
    std::string equipmentModel;
};

// ============================================================
// Sample
// ============================================================
struct Sample {
    std::string sampleId;
    std::string sampleModel;
    std::string manufacturer;
    std::string serialNo;
    double volume = 0;
    SampleInspectionData appearanceInspection;
    HydroStaticTestData hydroStaticTest;
    TestResultStatus overallResult = TestResultStatus::NotTested;
    std::string notes;
};

// ============================================================
// UnifiedTestResult
// ============================================================
struct UnifiedTestResult {
    int id = 0;
    std::string testSerialNo;
    DateTime testDate;
    std::string testerName;
    std::string testerCompany;
    TestStandard testStandard;
    Sample sample;
    TestEnvironmentData testEnvironment;
};

} // namespace sy1000
