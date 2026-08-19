#pragma once

#include <QString>

#include "models/model.h"

namespace sy1000 {

// Generates the PDF hydrostatic test report from a UnifiedTestResult, mirroring
// the original WPF TestReportGenerator (QTextDocument + QPrinter).
class TestReportGenerator
{
public:
    // Build an HTML document describing the result (used for both the PDF and
    // the on-screen preview). Returns the HTML string.
    static QString buildHtml(const sy1000::UnifiedTestResult &r);

    // Print the report to a PDF file under Documents/<baseFolder>/.
    //   folder naming: {manufacturer}_{serialNo}_{yyyyMMdd_HHmm}
    //   file naming:   {manufacturer}_{serialNo}_试验报告.pdf
    // On success returns true and writes the absolute path to outPath.
    static bool generatePdf(const sy1000::UnifiedTestResult &r, QString *outPath = nullptr,
                            const QString &baseFolder = QStringLiteral("水压测试结果"));
};

} // namespace sy1000
