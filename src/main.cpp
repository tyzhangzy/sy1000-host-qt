#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>

#include "core/hydroadapter.h"
#include "core/simdevice.h"
#include "dao/database.h"
#include "languagehelper.h"
#include "services/loginservice.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("SY1000"));
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    // Initialize SQLite (database + tables + seed admin).
    if (!Database::initialize()) {
        qWarning("SY1000: database initialization failed");
    }

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // 打印 QML 加载/语法错误，便于诊断
    QObject::connect(&engine, &QQmlEngine::warnings,
                     [](const QList<QQmlError> &warnings) {
                         for (const auto &w : warnings)
                             qWarning().noquote() << w.toString();
                     });

    // 语言切换助手（默认英文，可切中文）。暴露为 QML 全局 "lang"。
    LanguageHelper langHelper(&engine);
    engine.rootContext()->setContextProperty(QStringLiteral("lang"), &langHelper);

    // 登录服务（接入 SQLite 用户表）。暴露为 QML 全局 "loginService"。
    LoginService loginService;
    engine.rootContext()->setContextProperty(QStringLiteral("loginService"), &loginService);

    // 水压试验控制器（用模拟设备，无硬件也能演示完整流程）。暴露为 QML 全局 "hydro"。
    sy1000::SimulatedDeviceProvider simDevice;
    sy1000::HydroTestControllerAdapter hydro(&simDevice);
    engine.rootContext()->setContextProperty(QStringLiteral("hydro"), &hydro);

    // 入口 QML 界面（登录页；后续把 core/devices/dao/report 分层接入）
    // qt_add_resources(PREFIX "/") 下，文件 qml/Main.qml 的资源路径为 qrc:/qml/Main.qml
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    return app.exec();
}
