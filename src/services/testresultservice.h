#pragma once

#include <string>
#include <vector>

#include "models/model.h"

// Business service for unified test results (generation, overall result, persistence).
namespace TestResultService {

// yyyyMMddHHmm + 3 random chars, matching the original WPF generator.
std::string generateTestSerialNo();

// Build a UnifiedTestResult with a fresh serial and the given data.
sy1000::UnifiedTestResult createUnifiedTestResult(
    const std::string &testerName,
    const std::string &testerCompany,
    const sy1000::Sample &sample,
    const sy1000::TestStandard &testStandard);

// Overall result: scrap -> hydrostatic result -> all qualified -> not tested.
sy1000::TestResultStatus determineOverallResult(
    const sy1000::SampleInspectionData &inspection,
    const sy1000::HydroStaticTestData &hydro);

int save(const sy1000::UnifiedTestResult &r);                          // returns id
std::vector<sy1000::UnifiedTestResult> getLatest(int count = 10);
sy1000::UnifiedTestResult findBySerialNo(const std::string &serialNo);
std::vector<sy1000::UnifiedTestResult> findAll();

} // namespace TestResultService
