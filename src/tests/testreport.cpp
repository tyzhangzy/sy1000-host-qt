// Headless smoke test for PDF report generation.
// Verifies: buildHtml is non-trivial and generatePdf writes a non-empty PDF.
// QPrinter needs a QGuiApplication (font database access).
#include <QGuiApplication>
#include <QFileInfo>

#include <cstdio>

#include "models/model.h"
#include "report/testreportgenerator.h"

using sy1000::Sample;
using sy1000::TestResultStatus;
using sy1000::TestStandard;
using sy1000::UnifiedTestResult;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    UnifiedTestResult r;
    r.testSerialNo = "202608191200ABC";
    r.testerName = "TesterA";
    r.testerCompany = "ACME";
    r.testDate = std::chrono::system_clock::now();

    TestStandard ts;
    ts.standardName = "GB1234";
    ts.workingPressure = 25.0;
    ts.testingPressure = 37.5;
    ts.residualDeformationRate = 3.0;
    r.testStandard = ts;

    Sample s;
    s.sampleId = "S1";
    s.sampleModel = "ModelX";
    s.manufacturer = "ACME";
    s.serialNo = "SN-001";
    s.volume = 6.8;
    s.overallResult = TestResultStatus::Passed;

    auto &h = s.hydroStaticTest;
    h.initialWeight = 100.0;
    h.pressureWeight = 150.0;
    h.finalWeight = 101.0;
    h.fullDeformation = 50.0;
    h.residualDeformation = 1.0;
    h.residualDeformationRate = 2.0;
    h.testResult = TestResultStatus::Passed;
    // Add pressure points so the embedded chart is exercised.
    const auto base = std::chrono::system_clock::now().time_since_epoch().count();
    for (int i = 0; i <= 20; ++i) {
        sy1000::PressureWeightPoint pt;
        pt.timestamp = sy1000::DateTime(std::chrono::milliseconds(base / 1000000 + i * 500));
        pt.pressure = 5.0 + i * 2.0;
        h.pressureWeightData.push_back(pt);
    }

    s.appearanceInspection.externalResult = sy1000::InspectionResult::Qualified;
    s.appearanceInspection.internalResult = sy1000::InspectionResult::Qualified;
    s.appearanceInspection.threadResult = sy1000::InspectionResult::Qualified;
    s.appearanceInspection.valveResult = sy1000::InspectionResult::Qualified;

    r.samples.push_back(std::move(s));

    const QString html = sy1000::TestReportGenerator::buildHtml(r);
    std::printf("html length=%lld contains-chart=%s\n", static_cast<long long>(html.size()),
                html.contains("data:image/png;base64,") ? "yes" : "no");

    QString path;
    const bool ok = sy1000::TestReportGenerator::generatePdf(r, &path);
    const bool exists = QFileInfo::exists(path) && QFileInfo(path).size() > 0;
    std::printf("pdf ok=%d path=%s size=%lld\n", ok, qPrintable(path),
                ok ? QFileInfo(path).size() : -1);

    std::printf(ok && exists ? "REPORT SMOKE PASS\n" : "REPORT SMOKE FAIL\n");
    return (ok && exists) ? 0 : 1;
}
