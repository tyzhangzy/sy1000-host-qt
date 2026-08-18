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
    Q_INVOKABLE void removeResult(int id);
};

} // namespace sy1000
