#pragma once

#include <QString>
#include <QVariantMap>

namespace sy1000 {

// Loads device/system configuration from config.json (C9). Falls back to
// defaults when the file is missing or a key is absent.
class ConfigManager
{
public:
    // Load configuration. If path is empty, searches the app dir and cwd for
    // config.json. Returns true on success (file found and parsed).
    static bool load(const QString &path = QString());

    // Generic accessor with default value.
    static QString value(const QString &key, const QString &defaultValue = QString());

    // Convenience accessors.
    static QString deviceName();
    static QString manufacturer();
    static QString serialNo();
    static QString manufactureDate();

    // Serial ports used by the device layer (config.json keys "tasPort"/"scalePort").
    // On Linux hosts (Kylin/UOS) these would be e.g. ttyS0 / ttyUSB0 (M8).
    static QString tasPort();
    static QString scalePort();

private:
    static QVariantMap s_config;
};

} // namespace sy1000
