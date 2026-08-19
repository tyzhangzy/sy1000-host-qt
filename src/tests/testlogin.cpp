// Headless smoke test for LoginService (valid credentials + signal emission).
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>

#include <cstdio>

#include "dao/database.h"
#include "services/loginservice.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (!Database::initialize()) {
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
    return pass ? 0 : 1;
}
