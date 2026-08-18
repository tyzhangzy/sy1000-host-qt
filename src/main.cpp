#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>
#include <QtQml>

#include "core/simdevice.h"
#include "dao/database.h"
#include "languagehelper.h"
#include "services/hydroadapter.h"
#include "services/loginservice.h"
#include "ui/charts/RealTimeChart.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("SY1000"));
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    // Register the realtime chart widget for QML (import SyCharts 1.0).
    qmlRegisterType<RealTimeChart>("SyCharts", 1, 0, "RealTimeChart");

    // Initialize SQLite (database + tables + seed admin).
    if (!Database::initialize()) {
        qWarning("SY1000: database initialization failed");
    }

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // Print QML load/syntax errors for diagnostics
    QObject::connect(&engine, &QQmlEngine::warnings,
                     [](const QList<QQmlError> &warnings) {
                         for (const auto &w : warnings)
                             qWarning().noquote() << w.toString();
                     });

    // Language switcher (default EN, can switch to zh_CN). Exposed to QML as "lang".
    LanguageHelper langHelper(&engine);
    engine.rootContext()->setContextProperty(QStringLiteral("lang"), &langHelper);

    // Login service (SQLite users table). Exposed to QML as "loginService".
    LoginService loginService;
    engine.rootContext()->setContextProperty(QStringLiteral("loginService"), &loginService);

    // Hydrostatic test controller (simulated device, so the flow runs without
    // real hardware). Exposed to QML as "hydro".
    sy1000::SimulatedDeviceProvider simDevice;
    sy1000::HydroTestControllerAdapter hydro(&simDevice);
    engine.rootContext()->setContextProperty(QStringLiteral("hydro"), &hydro);

    // On successful login, pass the logged-in user to the adapter for result saving.
    QObject::connect(&loginService, &LoginService::loginSucceeded, [&]() {
        hydro.setTester(loginService.username(), loginService.company());
    });

    // Entry QML UI (login page; core/devices/dao/report layers plug in later).
    // qt_add_resources(PREFIX "/") -> qml/Main.qml is served at qrc:/qml/Main.qml
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    return app.exec();
}
