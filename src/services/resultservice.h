#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

namespace sy1000 {

// QML bridge for querying/persisting test results from SQLite.
class ResultServiceAdapter : public QObject
{
    Q_OBJECT
public:
    explicit ResultServiceAdapter(QObject *parent = nullptr);

    // List of saved results as QVariantMap {id, serial, tester, company, manufacturer, overall, rate, date}.
    Q_INVOKABLE QVariantList results() const;
    // Full detail of one result by id.
    Q_INVOKABLE QVariantMap details(int id) const;
    // Report-oriented data (all fields for the report preview page) by id.
    Q_INVOKABLE QVariantMap reportData(int id) const;
    // Generate a PDF report for the result; returns the file path ("" on failure).
    Q_INVOKABLE QString generatePdf(int id);
    // Open a generated PDF with the system viewer. Uses QUrl::fromLocalFile so
    // spaces / CJK characters in the path are correctly escaped (L14).
    Q_INVOKABLE bool openReportPdf(const QString &path);
    Q_INVOKABLE void removeResult(int id);
};

} // namespace sy1000
