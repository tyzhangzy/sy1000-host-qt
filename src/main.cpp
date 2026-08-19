#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>
#include <QDebug>
#include <QtQml>

#include "core/simdevice.h"
#include "dao/database.h"
#include "languagehelper.h"
#include "services/configmanager.h"
#include "services/deviceservice.h"
#include "services/hydroadapter.h"
#include "services/loginservice.h"
#include "services/resultservice.h"
#include "services/userservice.h"
#include "ui/charts/RealTimeChart.h"

int main(int argc, char *argv[])
{
    // Material style tuned toward the WPF MaterialDesign theme (Indigo primary, Amber accent).
    qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", "Light");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_PRIMARY", "Indigo");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_ACCENT", "Amber");
    QQuickStyle::setStyle(QStringLiteral("Material"));

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("SY1000"));

    // Register the realtime chart widget for QML (import SyCharts 1.0).
    qmlRegisterType<RealTimeChart>("SyCharts", 1, 0, "RealTimeChart");

    // Initialize SQLite (database + tables + seed admin). A missing database
    // would make every data feature fail silently later, so fail fast (L2).
    if (!Database::initialize()) {
        qCritical() << "SY1000: database initialization failed, exiting";
        return -1;
    }

    // Load device/system configuration from config.json (fall back to defaults).
    sy1000::ConfigManager::load();

    // Services are declared BEFORE the engine so they are destroyed AFTER it:
    // QML objects may still reference them while the engine is being torn down
    // (L13). simDevice is declared before hydro, which borrows its pointer.
    sy1000::SimulatedDeviceProvider simDevice;
    sy1000::HydroTestControllerAdapter hydro(&simDevice);
    LoginService loginService;
    sy1000::ResultServiceAdapter resultService;
    sy1000::UserServiceAdapter userService;
    sy1000::DeviceServiceAdapter deviceService;

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

    // Language switcher (default zh_CN to match the original WPF Chinese UI;
    // can switch to EN). Exposed to QML as "lang".
    LanguageHelper langHelper(&engine);
    engine.rootContext()->setContextProperty(QStringLiteral("lang"), &langHelper);
    langHelper.setLanguage(QStringLiteral("zh_CN"));

    // Login service (SQLite users table). Exposed to QML as "loginService".
    engine.rootContext()->setContextProperty(QStringLiteral("loginService"), &loginService);

    // Hydrostatic test controller (simulated device, so the flow runs without
    // real hardware). Exposed to QML as "hydro".
    engine.rootContext()->setContextProperty(QStringLiteral("hydro"), &hydro);

    // On successful login, pass the logged-in user to the adapter for result saving.
    QObject::connect(&loginService, &LoginService::loginSucceeded, [&]() {
        hydro.setTester(loginService.username(), loginService.company());
    });

    // Result management bridge for QML (query saved results).
    engine.rootContext()->setContextProperty(QStringLiteral("resultService"), &resultService);

    // User management bridge for QML.
    engine.rootContext()->setContextProperty(QStringLiteral("userService"), &userService);

    // Device maintenance bridge for QML.
    engine.rootContext()->setContextProperty(QStringLiteral("deviceService"), &deviceService);

    // Entry QML UI (login page; core/devices/dao/report layers plug in later).
    // qt_add_resources(PREFIX "/") -> qml/Main.qml is served at qrc:/qml/Main.qml
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    return app.exec();
}
