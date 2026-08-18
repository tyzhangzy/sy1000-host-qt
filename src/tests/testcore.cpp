// Headless smoke test for the data/services layer (no GUI).
// Verifies: generate serial -> create result -> save -> read back.
#include <QCoreApplication>
#include <cstdio>

#include "dao/database.h"
#include "dao/testresultdao.h"
#include "services/testresultservice.h"

using sy1000::Sample;
using sy1000::TestResultStatus;
using sy1000::TestStandard;
using sy1000::UnifiedTestResult;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (!Database::initialize()) {
        std::printf("[FAIL] database init\n");
        return 1;
    }

    // Build a sample with a hydrostatic result.
    Sample sample;
    sample.sampleId = "S1";
    sample.sampleModel = "ModelX";
    sample.manufacturer = "ACME";
    sample.serialNo = "SN-001";
    sample.hydroStaticTest.initialWeight = 100.0;
    sample.hydroStaticTest.pressureWeight = 150.0;
    sample.hydroStaticTest.finalWeight = 101.0;
    sample.hydroStaticTest.residualDeformationRate = 1.5;
    sample.hydroStaticTest.testResult = TestResultStatus::Qualified;

    TestStandard std;
    std.standardName = "GB1234";
    std.workingPressure = 25.0;
    std.testingPressure = 37.5;
    std.residualDeformationRate = 3.0;

    // Create + save.
    UnifiedTestResult r = TestResultService::createUnifiedTestResult("tester", "company", sample, std);
    r.sample.overallResult = TestResultService::determineOverallResult(sample.appearanceInspection, sample.hydroStaticTest);
    const int id = TestResultService::save(r);
    std::printf("saved id=%d serial=%s\n", id, r.testSerialNo.c_str());

    // Read back by serial.
    UnifiedTestResult loaded = TestResultService::findBySerialNo(r.testSerialNo);
    if (loaded.testSerialNo.empty()) {
        std::printf("[FAIL] find by serial\n");
        return 1;
    }

    std::printf("loaded serial=%s tester=%s manufacturer=%s overall=%d hydro_rate=%.2f standard=%s\n",
                loaded.testSerialNo.c_str(),
                loaded.testerName.c_str(),
                loaded.sample.manufacturer.c_str(),
                static_cast<int>(loaded.sample.overallResult),
                loaded.sample.hydroStaticTest.residualDeformationRate,
                loaded.testStandard.standardName.c_str());

    const int n = TestResultDao::count();
    std::printf("total results=%d\n", n);

    const bool ok = !loaded.sample.manufacturer.empty() &&
                    loaded.sample.manufacturer == "ACME" &&
                    loaded.sample.hydroStaticTest.residualDeformationRate == 1.5 &&
                    loaded.testStandard.standardName == "GB1234";
    std::printf(ok ? "SMOKE PASS\n" : "SMOKE FAIL\n");
    return ok ? 0 : 1;
}
