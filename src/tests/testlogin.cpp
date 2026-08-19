// Headless smoke test for LoginService (valid credentials + signal emission).
// Uses a throw-away SQLite database in a temp dir (L15).
#include <QCoreApplication>
#include <QEventLoop>
#include <QSqlDatabase>
#include <QTemporaryDir>
#include <QTimer>

#include <cstdio>

#include "dao/database.h"
#include "services/loginservice.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTemporaryDir tmp;
    if (!Database::initialize(tmp.filePath(QStringLiteral("sy1000_login_test.db")))) {
        std::printf("[FAIL] db init\n");
        return 1;
    }

    LoginService ls;
    int signalCount = 0;
    QObject::connect(&ls, &LoginService::loginSucceeded, [&]() { ++signalCount; });

    // Correct credentials.
    const bool ok = ls.tryLogin(QStringLiteral("admin"), QStringLiteral("9999"));
    std::printf("tryLogin(admin,9999)=%d err=%s signal=%d username=%s company=%s admin=%d\n",
                ok, qPrintable(ls.errorMessage()), signalCount,
                qPrintable(ls.username()), qPrintable(ls.company()), ls.isAdmin());

    // Wrong password should fail and not emit.
    const bool bad = ls.tryLogin(QStringLiteral("admin"), QStringLiteral("wrong"));
    std::printf("tryLogin(admin,wrong)=%d err=%s signal=%d\n",
                bad, qPrintable(ls.errorMessage()), signalCount);

    const bool pass = ok && signalCount == 1 && !bad && ls.username() == "admin";
    std::printf(pass ? "LOGIN SMOKE PASS\n" : "LOGIN SMOKE FAIL\n");
    // Close and drop the temp database so QTemporaryDir can clean it up (L15).
    if (QSqlDatabase::contains(QStringLiteral("qt_sql_default_connection"))) {
        QSqlDatabase::database().close();
        QSqlDatabase::removeDatabase(QStringLiteral("qt_sql_default_connection"));
    }
    return pass ? 0 : 1;
}
