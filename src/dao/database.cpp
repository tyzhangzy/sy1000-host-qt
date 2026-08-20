#include "dao/database.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

#include "dao/userdao.h"

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
    return initialize(databasePath());
}

bool Database::initialize(const QString &databaseFilePath)
{
    if (QSqlDatabase::contains(QStringLiteral("qt_sql_default_connection"))) {
        return QSqlDatabase::database().isOpen();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(databaseFilePath);
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
        " username TEXT NOT NULL UNIQUE,"       // unique username (L8)
        " company TEXT,"
        " password TEXT,"
        " create_date TEXT,"
        " is_admin INTEGER DEFAULT 0)";
    if (!q.exec(QString::fromLatin1(usersSql))) {
        qWarning() << "[dao] create users failed:" << q.lastError().text();
        return false;
    }
    // Enforce the unique-username rule on pre-existing databases too (L8).
    // Not fatal if duplicates already exist in an old DB (index creation fails).
    QSqlQuery uniqIdx(QStringLiteral("CREATE UNIQUE INDEX IF NOT EXISTS idx_users_username ON users(username)"));
    if (!uniqIdx.exec())
        qWarning() << "[dao] unique username index failed:" << uniqIdx.lastError().text();

    const char *resultsSql =
        "CREATE TABLE IF NOT EXISTS unified_test_results ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " test_serial_no TEXT,"
        " test_date INTEGER,"
        " tester_name TEXT,"
        " tester_company TEXT,"
        " manufacturer TEXT,"
        " overall_result INTEGER,"
        " payload TEXT)";
    if (!q.exec(QString::fromLatin1(resultsSql))) {
        qWarning() << "[dao] create results failed:" << q.lastError().text();
        return false;
    }

    // index for common queries
    QSqlQuery idx(QStringLiteral("CREATE INDEX IF NOT EXISTS idx_results_serial ON unified_test_results(test_serial_no)"));
    idx.exec();
    return true;
}

void Database::seed()
{
    QSqlQuery countQ(QStringLiteral("SELECT COUNT(*) FROM users"));
    if (countQ.exec() && countQ.next() && countQ.value(0).toInt() == 0) {
        QSqlQuery q;
        q.prepare(QStringLiteral("INSERT INTO users (username, company, password, create_date, is_admin)"
                                 " VALUES (?, ?, ?, ?, ?)"));
        q.addBindValue(QStringLiteral("admin"));
        q.addBindValue(QStringLiteral("Quanshen"));
        // Store a salted hash instead of the plaintext default password (H2).
        q.addBindValue(UserDao::hashPassword(QStringLiteral("9999")));
        q.addBindValue(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));  // real timestamp (L8)
        q.addBindValue(1);
        if (!q.exec()) {
            qWarning() << "[dao] seed admin failed:" << q.lastError().text();
        }
    }
}
