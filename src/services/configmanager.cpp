#include "services/configmanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace sy1000 {

QVariantMap ConfigManager::s_config;

bool ConfigManager::load(const QString &path)
{
    QString p = path;
    if (p.isEmpty()) {
        const QString exeDir = QCoreApplication::applicationDirPath();
        const QStringList candidates = {
            exeDir + QStringLiteral("/config.json"),
            QDir::currentPath() + QStringLiteral("/config.json"),
        };
        for (const QString &c : candidates) {
            if (QFile::exists(c)) {
                p = c;
                break;
            }
        }
    }
    if (p.isEmpty() || !QFile::exists(p))
        return false;

    QFile f(p);
    if (!f.open(QIODevice::ReadOnly))
        return false;
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject())
        return false;
    s_config = doc.object().toVariantMap();
    return true;
}

QString ConfigManager::value(const QString &key, const QString &defaultValue)
{
    const auto it = s_config.constFind(key);
    if (it != s_config.constEnd())
        return it.value().toString();
    return defaultValue;
}

QString ConfigManager::deviceName()
{
    return value(QStringLiteral("deviceName"), QStringLiteral("SY1000"));
}

QString ConfigManager::manufacturer()
{
    return value(QStringLiteral("manufacturer"), QStringLiteral("Quanshen"));
}

QString ConfigManager::serialNo()
{
    return value(QStringLiteral("serialNo"), QStringLiteral("SY1000-0001"));
}

QString ConfigManager::manufactureDate()
{
    return value(QStringLiteral("manufactureDate"), QStringLiteral("2026-01-01"));
}

QString ConfigManager::tasPort()
{
    return value(QStringLiteral("tasPort"), QStringLiteral("COM1"));
}

QString ConfigManager::scalePort()
{
    return value(QStringLiteral("scalePort"), QStringLiteral("COM2"));
}

} // namespace sy1000
