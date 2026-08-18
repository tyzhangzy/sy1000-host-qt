#pragma once

#include <QObject>
#include <QVariantList>

namespace sy1000 {

// QML bridge for querying/persisting test results from SQLite.
class ResultServiceAdapter : public QObject
{
    Q_OBJECT
public:
    explicit ResultServiceAdapter(QObject *parent = nullptr);

    // List of saved results as QVariantMap {id, serial, tester, company, manufacturer, overall, rate}.
    Q_INVOKABLE QVariantList results() const;
    Q_INVOKABLE void removeResult(int id);
};

} // namespace sy1000
