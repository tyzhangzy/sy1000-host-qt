#pragma once

#include <QJsonObject>
#include <QString>

#include "models/model.h"

// Serialization between the std::string models and JSON, used to persist
// the full UnifiedTestResult (with nested Sample/TestStandard) in SQLite.
namespace serial {

QString unifiedTestResultToJson(const sy1000::UnifiedTestResult &r);
sy1000::UnifiedTestResult unifiedTestResultFromJson(const QString &json);

QJsonObject testStandardToJson(const sy1000::TestStandard &t);
sy1000::TestStandard testStandardFromJson(const QJsonObject &o);

QJsonObject sampleInspectionToJson(const sy1000::SampleInspectionData &s);
sy1000::SampleInspectionData sampleInspectionFromJson(const QJsonObject &o);

QJsonObject hydroStaticToJson(const sy1000::HydroStaticTestData &h);
sy1000::HydroStaticTestData hydroStaticFromJson(const QJsonObject &o);

QJsonObject sampleToJson(const sy1000::Sample &s);
sy1000::Sample sampleFromJson(const QJsonObject &o);

// helpers
qint64 toEpochMillis(const sy1000::DateTime &t);
sy1000::DateTime fromEpochMillis(qint64 ms);

} // namespace serial
