#include "dao/testresultdao.h"

#include <QSqlQuery>
#include <QVariant>

#include "dao/json_serializer.h"

using sy1000::TestResultStatus;
using sy1000::UnifiedTestResult;

namespace {

// Column order: id, test_serial_no, test_date, tester_name, tester_company,
//              manufacturer, overall_result, payload
UnifiedTestResult rowToResult(const QSqlQuery &q)
{
    UnifiedTestResult r;
    r.id = q.value(0).toInt();
    r.testSerialNo = q.value(1).toString().toStdString();
    r.testDate = serial::fromEpochMillis(q.value(2).toLongLong());
    r.testerName = q.value(3).toString().toStdString();
    r.testerCompany = q.value(4).toString().toStdString();
    r.sample.manufacturer = q.value(5).toString().toStdString();
    r.sample.overallResult = static_cast<TestResultStatus>(q.value(6).toInt(0));

    // The JSON payload is the authoritative full record; the columns are kept
    // only as index/query fields. Fall back to the columns when the payload is
    // absent or cannot be parsed so column data is never silently overwritten
    // by a partial parse (M3).
    const QString payload = q.value(7).toString();
    if (!payload.trimmed().isEmpty()) {
        UnifiedTestResult p = serial::unifiedTestResultFromJson(payload);
        if (!p.testSerialNo.empty() || p.id != 0) {
            p.id = r.id; // primary key always comes from the row
            return p;
        }
    }
    return r;
}

} // namespace

int TestResultDao::insert(const UnifiedTestResult &r)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("INSERT INTO unified_test_results"
                             " (test_serial_no, test_date, tester_name, tester_company,"
                             "  manufacturer, overall_result, payload)"
                             " VALUES (?, ?, ?, ?, ?, ?, ?)"));
    q.addBindValue(QString::fromStdString(r.testSerialNo));
    q.addBindValue(serial::toEpochMillis(r.testDate));
    q.addBindValue(QString::fromStdString(r.testerName));
    q.addBindValue(QString::fromStdString(r.testerCompany));
    q.addBindValue(QString::fromStdString(r.sample.manufacturer));
    q.addBindValue(static_cast<int>(r.sample.overallResult));
    q.addBindValue(serial::unifiedTestResultToJson(r));
    if (!q.exec())
        return 0;
    return q.lastInsertId().toInt();
}

bool TestResultDao::update(const UnifiedTestResult &r)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("UPDATE unified_test_results SET"
                             " test_serial_no=?, test_date=?, tester_name=?, tester_company=?,"
                             " manufacturer=?, overall_result=?, payload=? WHERE id=?"));
    q.addBindValue(QString::fromStdString(r.testSerialNo));
    q.addBindValue(serial::toEpochMillis(r.testDate));
    q.addBindValue(QString::fromStdString(r.testerName));
    q.addBindValue(QString::fromStdString(r.testerCompany));
    q.addBindValue(QString::fromStdString(r.sample.manufacturer));
    q.addBindValue(static_cast<int>(r.sample.overallResult));
    q.addBindValue(serial::unifiedTestResultToJson(r));
    q.addBindValue(r.id);
    return q.exec();
}

bool TestResultDao::remove(int id)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("DELETE FROM unified_test_results WHERE id=?"));
    q.addBindValue(id);
    return q.exec();
}

UnifiedTestResult TestResultDao::findBySerialNo(const std::string &serialNo)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id, test_serial_no, test_date, tester_name, tester_company,"
                             " manufacturer, overall_result, payload FROM unified_test_results"
                             " WHERE test_serial_no=?"));
    q.addBindValue(QString::fromStdString(serialNo));
    if (q.exec() && q.next())
        return rowToResult(q);
    return {};
}

UnifiedTestResult TestResultDao::findById(int id)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id, test_serial_no, test_date, tester_name, tester_company,"
                             " manufacturer, overall_result, payload FROM unified_test_results"
                             " WHERE id=?"));
    q.addBindValue(id);
    if (q.exec() && q.next())
        return rowToResult(q);
    return {};
}

std::vector<UnifiedTestResult> TestResultDao::findAll()
{
    std::vector<UnifiedTestResult> out;
    QSqlQuery q(QStringLiteral("SELECT id, test_serial_no, test_date, tester_name, tester_company,"
                               " manufacturer, overall_result, payload FROM unified_test_results"
                               " ORDER BY test_date DESC"));
    while (q.next())
        out.push_back(rowToResult(q));
    return out;
}

std::vector<UnifiedTestResult> TestResultDao::getLatest(int count)
{
    std::vector<UnifiedTestResult> out;
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id, test_serial_no, test_date, tester_name, tester_company,"
                             " manufacturer, overall_result, payload FROM unified_test_results"
                             " ORDER BY test_date DESC LIMIT ?"));
    q.addBindValue(count);
    if (q.exec()) {
        while (q.next())
            out.push_back(rowToResult(q));
    }
    return out;
}

int TestResultDao::count()
{
    QSqlQuery q(QStringLiteral("SELECT COUNT(*) FROM unified_test_results"));
    q.next();
    return q.value(0).toInt();
}
