#include "dao/json_serializer.h"

#include <QJsonArray>
#include <QJsonDocument>

using sy1000::DateTime;
using sy1000::HydroStaticTestData;
using sy1000::InspectionResult;
using sy1000::PressureWeightPoint;
using sy1000::Sample;
using sy1000::SampleInspectionData;
using sy1000::TestResultStatus;
using sy1000::TestStandard;
using sy1000::UnifiedTestResult;

namespace serial {

qint64 toEpochMillis(const DateTime &t)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()).count();
}

DateTime fromEpochMillis(qint64 ms)
{
    return DateTime(std::chrono::milliseconds(ms));
}

QJsonObject testStandardToJson(const TestStandard &t)
{
    return QJsonObject{
        { "standardName", QString::fromStdString(t.standardName) },
        { "workingPressure", t.workingPressure },
        { "testingPressure", t.testingPressure },
        { "pressureHoldingTime", t.pressureHoldingTime },
        { "residualDeformationRate", t.residualDeformationRate },
        { "residualDeformation", t.residualDeformation },
    };
}

TestStandard testStandardFromJson(const QJsonObject &o)
{
    TestStandard t;
    t.standardName = o.value("standardName").toString().toStdString();
    t.workingPressure = o.value("workingPressure").toDouble(-1);
    t.testingPressure = o.value("testingPressure").toDouble(-1);
    t.pressureHoldingTime = o.value("pressureHoldingTime").toInt();
    t.residualDeformationRate = o.value("residualDeformationRate").toDouble(-1);
    t.residualDeformation = o.value("residualDeformation").toDouble(-1);
    return t;
}

QJsonObject sampleInspectionToJson(const SampleInspectionData &s)
{
    QJsonObject o{
        { "sampleId", QString::fromStdString(s.sampleId) },
        { "sampleModel", QString::fromStdString(s.sampleModel) },
        { "manufacturer", QString::fromStdString(s.manufacturer) },
        { "volume", s.volume },
        { "userCompany", QString::fromStdString(s.userCompany) },
        { "serialNo", QString::fromStdString(s.serialNo) },
        { "inspectionDate", toEpochMillis(s.inspectionDate) },
        { "inspectorName", QString::fromStdString(s.inspectorName) },
        { "inspectorCertNo", QString::fromStdString(s.inspectorCertNo) },
        { "inspectionCompleted", s.inspectionCompleted },
        { "externalResult", static_cast<int>(s.externalResult) },
        { "internalResult", static_cast<int>(s.internalResult) },
        { "threadResult", static_cast<int>(s.threadResult) },
        { "valveResult", static_cast<int>(s.valveResult) },
        { "externalThermalDamage", s.externalThermalDamage },
        { "externalScratch", s.externalScratch },
        { "externalWear", s.externalWear },
        { "externalDelamination", s.externalDelamination },
        { "externalDeformation", s.externalDeformation },
        { "externalDefectLocation", QString::fromStdString(s.externalDefectLocation) },
        { "externalOther", QString::fromStdString(s.externalOther) },
        { "internalSmell", s.internalSmell },
        { "internalDebris", QString::fromStdString(s.internalDebris) },
        { "internalSurfaceCondition", QString::fromStdString(s.internalSurfaceCondition) },
        { "internalDefectLocation", QString::fromStdString(s.internalDefectLocation) },
        { "internalOther", QString::fromStdString(s.internalOther) },
        { "threadSpecification", QString::fromStdString(s.threadSpecification) },
        { "threadCondition", QString::fromStdString(s.threadCondition) },
        { "threadEvaluation", QString::fromStdString(s.threadEvaluation) },
        { "threadOther", QString::fromStdString(s.threadOther) },
        { "valveNo", QString::fromStdString(s.valveNo) },
        { "valveThreadCondition", QString::fromStdString(s.valveThreadCondition) },
        { "valveAirTightness", QString::fromStdString(s.valveAirTightness) },
        { "valveDiaphragmReplaced", s.valveDiaphragmReplaced },
        { "valveOther", QString::fromStdString(s.valveOther) },
    };
    return o;
}

SampleInspectionData sampleInspectionFromJson(const QJsonObject &o)
{
    SampleInspectionData s;
    s.sampleId = o.value("sampleId").toString().toStdString();
    s.sampleModel = o.value("sampleModel").toString().toStdString();
    s.manufacturer = o.value("manufacturer").toString().toStdString();
    s.volume = o.value("volume").toDouble();
    s.userCompany = o.value("userCompany").toString().toStdString();
    s.serialNo = o.value("serialNo").toString().toStdString();
    s.inspectionDate = fromEpochMillis(o.value("inspectionDate").toVariant().toLongLong());
    s.inspectorName = o.value("inspectorName").toString().toStdString();
    s.inspectorCertNo = o.value("inspectorCertNo").toString().toStdString();
    s.inspectionCompleted = o.value("inspectionCompleted").toBool();
    s.externalResult = static_cast<InspectionResult>(o.value("externalResult").toInt(0));
    s.internalResult = static_cast<InspectionResult>(o.value("internalResult").toInt(0));
    s.threadResult = static_cast<InspectionResult>(o.value("threadResult").toInt(0));
    s.valveResult = static_cast<InspectionResult>(o.value("valveResult").toInt(0));
    s.externalThermalDamage = o.value("externalThermalDamage").toBool();
    s.externalScratch = o.value("externalScratch").toBool();
    s.externalWear = o.value("externalWear").toBool();
    s.externalDelamination = o.value("externalDelamination").toBool();
    s.externalDeformation = o.value("externalDeformation").toBool();
    s.externalDefectLocation = o.value("externalDefectLocation").toString().toStdString();
    s.externalOther = o.value("externalOther").toString().toStdString();
    s.internalSmell = o.value("internalSmell").toBool();
    s.internalDebris = o.value("internalDebris").toString().toStdString();
    s.internalSurfaceCondition = o.value("internalSurfaceCondition").toString().toStdString();
    s.internalDefectLocation = o.value("internalDefectLocation").toString().toStdString();
    s.internalOther = o.value("internalOther").toString().toStdString();
    s.threadSpecification = o.value("threadSpecification").toString().toStdString();
    s.threadCondition = o.value("threadCondition").toString().toStdString();
    s.threadEvaluation = o.value("threadEvaluation").toString().toStdString();
    s.threadOther = o.value("threadOther").toString().toStdString();
    s.valveNo = o.value("valveNo").toString().toStdString();
    s.valveThreadCondition = o.value("valveThreadCondition").toString().toStdString();
    s.valveAirTightness = o.value("valveAirTightness").toString().toStdString();
    s.valveDiaphragmReplaced = o.value("valveDiaphragmReplaced").toBool();
    s.valveOther = o.value("valveOther").toString().toStdString();
    return s;
}

QJsonObject hydroStaticToJson(const HydroStaticTestData &h)
{
    QJsonArray pts;
    for (const auto &pt : h.pressureWeightData)
        pts.append(QJsonObject{
            { "ts", toEpochMillis(pt.timestamp) },
            { "pressure", pt.pressure },
            { "weight", pt.weight },
        });
    return QJsonObject{
        { "initialWeight", h.initialWeight },
        { "pressureWeight", h.pressureWeight },
        { "finalWeight", h.finalWeight },
        { "fullDeformation", h.fullDeformation },
        { "residualDeformation", h.residualDeformation },
        { "residualDeformationRate", h.residualDeformationRate },
        { "workingPressure", h.workingPressure },
        { "testPressure", h.testPressure },
        { "testResult", static_cast<int>(h.testResult) },
        { "resultDetails", QString::fromStdString(h.resultDetails) },
        { "pressureWeightData", pts },
    };
}

HydroStaticTestData hydroStaticFromJson(const QJsonObject &o)
{
    HydroStaticTestData h;
    h.initialWeight = o.value("initialWeight").toDouble();
    h.pressureWeight = o.value("pressureWeight").toDouble();
    h.finalWeight = o.value("finalWeight").toDouble();
    h.fullDeformation = o.value("fullDeformation").toDouble();
    h.residualDeformation = o.value("residualDeformation").toDouble();
    h.residualDeformationRate = o.value("residualDeformationRate").toDouble();
    h.workingPressure = o.value("workingPressure").toDouble();
    h.testPressure = o.value("testPressure").toDouble();
    h.testResult = static_cast<TestResultStatus>(o.value("testResult").toInt(0));
    h.resultDetails = o.value("resultDetails").toString().toStdString();
    const QJsonArray pts = o.value("pressureWeightData").toArray();
    for (const auto &v : pts) {
        const QJsonObject po = v.toObject();
        PressureWeightPoint pt;
        pt.timestamp = fromEpochMillis(po.value("ts").toVariant().toLongLong());
        pt.pressure = po.value("pressure").toDouble();
        pt.weight = po.value("weight").toDouble();
        h.pressureWeightData.push_back(pt);
    }
    return h;
}

QJsonObject sampleToJson(const Sample &s)
{
    return QJsonObject{
        { "sampleId", QString::fromStdString(s.sampleId) },
        { "sampleModel", QString::fromStdString(s.sampleModel) },
        { "manufacturer", QString::fromStdString(s.manufacturer) },
        { "serialNo", QString::fromStdString(s.serialNo) },
        { "volume", s.volume },
        { "overallResult", static_cast<int>(s.overallResult) },
        { "notes", QString::fromStdString(s.notes) },
        { "appearanceInspection", sampleInspectionToJson(s.appearanceInspection) },
        { "hydroStaticTest", hydroStaticToJson(s.hydroStaticTest) },
    };
}

Sample sampleFromJson(const QJsonObject &o)
{
    Sample s;
    s.sampleId = o.value("sampleId").toString().toStdString();
    s.sampleModel = o.value("sampleModel").toString().toStdString();
    s.manufacturer = o.value("manufacturer").toString().toStdString();
    s.serialNo = o.value("serialNo").toString().toStdString();
    s.volume = o.value("volume").toDouble();
    s.overallResult = static_cast<TestResultStatus>(o.value("overallResult").toInt(0));
    s.notes = o.value("notes").toString().toStdString();
    s.appearanceInspection = sampleInspectionFromJson(o.value("appearanceInspection").toObject());
    s.hydroStaticTest = hydroStaticFromJson(o.value("hydroStaticTest").toObject());
    return s;
}

QString unifiedTestResultToJson(const UnifiedTestResult &r)
{
    QJsonArray samples;
    for (const auto &s : r.samples)
        samples.append(sampleToJson(s));
    QJsonObject o{
        { "id", r.id },
        { "testSerialNo", QString::fromStdString(r.testSerialNo) },
        { "testDate", toEpochMillis(r.testDate) },
        { "testerName", QString::fromStdString(r.testerName) },
        { "testerCompany", QString::fromStdString(r.testerCompany) },
        { "testStandard", testStandardToJson(r.testStandard) },
        { "samples", samples },
        { "roomTemperature", r.testEnvironment.roomTemperature },
        { "humidity", r.testEnvironment.humidity },
        { "equipmentId", QString::fromStdString(r.testEnvironment.equipmentId) },
        { "equipmentModel", QString::fromStdString(r.testEnvironment.equipmentModel) },
    };
    return QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact));
}

UnifiedTestResult unifiedTestResultFromJson(const QString &json)
{
    UnifiedTestResult r;
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (doc.isNull())
        return r;
    const QJsonObject o = doc.object();
    r.id = o.value("id").toInt();
    r.testSerialNo = o.value("testSerialNo").toString().toStdString();
    r.testDate = fromEpochMillis(o.value("testDate").toVariant().toLongLong());
    r.testerName = o.value("testerName").toString().toStdString();
    r.testerCompany = o.value("testerCompany").toString().toStdString();
    r.testStandard = testStandardFromJson(o.value("testStandard").toObject());

    // Multi-sample payload (H3).
    const QJsonArray sampleArr = o.value("samples").toArray();
    if (!sampleArr.isEmpty()) {
        for (const auto &v : sampleArr)
            r.samples.push_back(sampleFromJson(v.toObject()));
    } else {
        // Backward compatibility: old records stored a single "sample" field.
        const QJsonObject legacy = o.value("sample").toObject();
        if (!legacy.isEmpty())
            r.samples.push_back(sampleFromJson(legacy));
    }

    r.testEnvironment.roomTemperature = o.value("roomTemperature").toDouble();
    r.testEnvironment.humidity = o.value("humidity").toDouble();
    r.testEnvironment.equipmentId = o.value("equipmentId").toString().toStdString();
    r.testEnvironment.equipmentModel = o.value("equipmentModel").toString().toStdString();
    return r;
}

} // namespace serial

