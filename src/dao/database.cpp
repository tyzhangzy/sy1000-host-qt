#include "dao/database.h"

#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

QString Database::databasePath()
{
    const QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString folder = docs + QStringLiteral("/QuanshenAppData/SY1000");
    QDir().mkpath(folder);
    // NOTE: the original WPF project uses a LiteDB file named "userInfo.db" in the
    // same folder; the Qt version uses its own SQLite file to avoid conflict.
    return folder + QStringLiteral("/sy1000_qt.db");
}

bool Database::initialize()
{
    if (QSqlDatabase::contains(QStringLiteral("qt_sql_default_connection"))) {
        return QSqlDatabase::database().isOpen();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(databasePath());
    if (!db.open()) {
        qWarning() << "[dao] DB open failed:" << db.lastError().text();
        return false;
    }

    if (!createTables()) {
        qWarning() << "[dao] create tables failed";
        return false;
    }
    seed();
    return true;
}

bool Database::createTables()
{
    QSqlQuery q;

    const char *usersSql =
        "CREATE TABLE IF NOT EXISTS users ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " username TEXT NOT NULL,"
        " company TEXT,"
        " password TEXT,"
        " create_date TEXT,"
        " is_admin INTEGER DEFAULT 0)";
    if (!q.exec(QString::fromLatin1(usersSql))) {
        qWarning() << "[dao] create users failed:" << q.lastError().text();
        return false;
    }

    const char *resultsSql =
        "CREATE TABLE IF NOT EXISTS unified_test_results ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " test_serial_no TEXT,"
        " test_date TEXT,"
        " tester_name TEXT,"
        " tester_company TEXT)";
    if (!q.exec(QString::fromLatin1(resultsSql))) {
        qWarning() << "[dao] create results failed:" << q.lastError().text();
        return false;
    }
    return true;
}

void Database::seed()
{
    QSqlQuery countQ(QStringLiteral("SELECT COUNT(*) FROM users"));
    countQ.next();
    if (countQ.value(0).toInt() == 0) {
        QSqlQuery q;
        q.prepare(QStringLiteral("INSERT INTO users (username, company, password, create_date, is_admin)"
                                 " VALUES (?, ?, ?, ?, ?)"));
        q.addBindValue(QStringLiteral("admin"));
        q.addBindValue(QStringLiteral("Quanshen"));
        q.addBindValue(QStringLiteral("9999"));
        q.addBindValue(QStringLiteral("2026-01-01 00:00:00"));
        q.addBindValue(1);
        if (!q.exec()) {
            qWarning() << "[dao] seed admin failed:" << q.lastError().text();
        }
    }
}
