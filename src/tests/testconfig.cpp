// Headless smoke test for ConfigManager (config.json loading, C9).
// Verifies: loads an explicit JSON, returns configured values, and falls back
// to defaults when a key/file is missing.
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTemporaryDir>

#include <cstdio>

#include "services/configmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Write a temp config.json and load it explicitly.
    QTemporaryDir dir;
    const QString path = dir.filePath(QStringLiteral("config.json"));
    {
        QFile f(path);
        f.open(QIODevice::WriteOnly);
        f.write(QJsonDocument(QJsonObject{
            { "deviceName", "SY1000-TEST" },
            { "manufacturer", "ACME" },
            { "serialNo", "SN-9" },
            { "manufactureDate", "2026-06-06" },
        }).toJson());
    }

    const bool ok = sy1000::ConfigManager::load(path);
    std::printf("load=%d\n", ok);

    const bool good = ok &&
                      sy1000::ConfigManager::deviceName() == "SY1000-TEST" &&
                      sy1000::ConfigManager::manufacturer() == "ACME" &&
                      sy1000::ConfigManager::serialNo() == "SN-9" &&
                      sy1000::ConfigManager::manufactureDate() == "2026-06-06";
    std::printf("device=%s mfg=%s serial=%s date=%s\n",
                qPrintable(sy1000::ConfigManager::deviceName()),
                qPrintable(sy1000::ConfigManager::manufacturer()),
                qPrintable(sy1000::ConfigManager::serialNo()),
                qPrintable(sy1000::ConfigManager::manufactureDate()));

    // Missing key -> default.
    const QString unknown = sy1000::ConfigManager::value(QStringLiteral("noSuchKey"), QStringLiteral("dflt"));
    std::printf("unknown-key=%s\n", qPrintable(unknown));

    std::printf(good && unknown == "dflt" ? "CONFIG SMOKE PASS\n" : "CONFIG SMOKE FAIL\n");
    return (good && unknown == "dflt") ? 0 : 1;
}
