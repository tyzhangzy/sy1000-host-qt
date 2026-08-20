#include "services/testresultservice.h"

#include <cstdlib>
#include <ctime>
#include <random>

#include "dao/testresultdao.h"

using sy1000::HydroStaticTestData;
using sy1000::InspectionResult;
using sy1000::Sample;
using sy1000::SampleInspectionData;
using sy1000::TestResultStatus;
using sy1000::TestStandard;
using sy1000::UnifiedTestResult;

namespace TestResultService {

std::string generateTestSerialNo()
{
    static const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(chars) - 2));

    const std::time_t now = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif

    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y%m%d%H%M", &tm);

    std::string suffix;
    for (int i = 0; i < 3; ++i)
        suffix += chars[dist(rng)];

    return std::string(buf) + suffix;
}

UnifiedTestResult createUnifiedTestResult(const std::string &testerName,
                                          const std::string &testerCompany,
                                          const Sample &sample,
                                          const TestStandard &testStandard)
{
    UnifiedTestResult r;
    r.testSerialNo = generateTestSerialNo();
    r.testDate = std::chrono::system_clock::now();
    r.testerName = testerName;
    r.testerCompany = testerCompany;
    r.samples.push_back(sample);
    r.testStandard = testStandard;
    return r;
}

TestResultStatus determineOverallResult(const SampleInspectionData &inspection,
                                        const HydroStaticTestData &hydro)
{
    const auto isScrapped = [](InspectionResult r) {
        return r == InspectionResult::Scrapped;
    };
    if (isScrapped(inspection.externalResult) || isScrapped(inspection.internalResult) ||
        isScrapped(inspection.threadResult) || isScrapped(inspection.valveResult)) {
        return TestResultStatus::Scrapped;
    }

    if (hydro.testResult != TestResultStatus::NotTested)
        return hydro.testResult;

    if (inspection.externalResult == InspectionResult::Qualified &&
        inspection.internalResult == InspectionResult::Qualified &&
        inspection.threadResult == InspectionResult::Qualified &&
        inspection.valveResult == InspectionResult::Qualified) {
        return TestResultStatus::Qualified;
    }

    return TestResultStatus::NotTested;
}

int save(const UnifiedTestResult &r)
{
    if (r.id == 0)
        return TestResultDao::insert(r);
    TestResultDao::update(r);
    return r.id;
}

std::vector<UnifiedTestResult> getLatest(int count)
{
    return TestResultDao::getLatest(count);
}

UnifiedTestResult findBySerialNo(const std::string &serialNo)
{
    return TestResultDao::findBySerialNo(serialNo);
}

UnifiedTestResult findById(int id)
{
    return TestResultDao::findById(id);
}

std::vector<UnifiedTestResult> findAll()
{
    return TestResultDao::findAll();
}

} // namespace TestResultService
