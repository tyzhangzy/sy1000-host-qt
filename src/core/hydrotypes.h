#pragma once

#include <array>
#include <set>
#include <string>

#include "models/model.h"

namespace sy1000 {

// Error codes produced by sub-tasks.
enum class HydroTestError { None, Timeout, LeakDetected, ReleaseFailed, DeviceError, Cancelled, InternalError };

// Parameters passed to a sub-task.
struct TaskParams {
    // dialog / instruction texts (empty = none)
    std::string dialogTitle;
    std::string dialogMessage;

    // pressurize
    double targetPressure = 0;
    double fastStopDelta = 0;
    double slowStopDelta = 0;
    bool useSlowPump = false;
    int fastTimeoutMinutes = 5;
    int slowTimeoutMinutes = 2;

    // hold / leak / release / stabilize / wait
    int holdSampleInterval1Sec = 10;
    int holdSampleInterval2Sec = 20;
    int leakCheckSampleIntervalSec = 5;
    // Leak pass/fail threshold (kg / MPa) used by HoldTask (L1).
    double leakThresholdKg = 0.5;
    int countdownSec = 0;
    double initialPressure = 0;
    // Extra time (s) ReleaseTask keeps waiting for the pressure to actually drop
    // before failing with ReleaseFailed (L19).
    int releaseMaxExtraSec = 60;
};

// Result produced by a sub-task (sample weights/pressures, leak flags).
struct TaskResult {
    std::array<double, 5> weightT10{};
    std::array<double, 5> weightT30{};
    double pressureT10 = 0;
    double pressureT30 = 0;
    bool pressureLeaking = false;
    std::set<int> leakingWaterJackets;
};

// Mutable container carrying data across the whole hydrostatic test.
struct HydroTestData {
    double initialPressure = 0;
    std::array<double, 5> initialWeights{};
    std::array<double, 5> testingPressureWeights{};
    std::array<double, 5> releasedWeights{};
    std::array<double, 5> fullDeformations{};
    std::array<double, 5> residualDeformations{};
    std::array<double, 5> residualDeformationRates{};
    std::array<TestResultStatus, 5> results{};
    std::array<std::string, 5> resultStrings{};

    double workingPressureT10 = 0;
    double workingPressureT30 = 0;
    std::array<double, 5> workingPressureWeightT10{};
    std::array<double, 5> workingPressureWeightT30{};

    void reset() { *this = HydroTestData{}; }
};

// Runtime options for the hydrostatic test (intervals, pressures, timeouts).
struct TestOptions {
    int checkCountdownSec = 5;       // water jacket / cylinder check wait
    int holdWorking1Sec = 10;        // hold@working first sample
    int holdWorking2Sec = 20;        // hold@working second sample
    int holdTestingSec = 60;         // hold@test pressure
    int releaseSec = 10;
    int stabilizeSec = 5;

    double workingPressure = 30.0;   // aligned with the preparation page (L18)
    double testingPressure = 45.0;
    double leakThresholdKg = 0.5;    // leak pass/fail threshold (L1)
    int releaseMaxExtraSec = 60;     // extra wait for pressure to drop (L19)
    double fastStopDelta = 1.0;
    double slowStopDelta = 0.15;
    int fastTimeoutMin = 5;
    int slowTimeoutMin = 2;
};

} // namespace sy1000
