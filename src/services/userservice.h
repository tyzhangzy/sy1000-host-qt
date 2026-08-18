#pragma once

#include <QObject>
#include <QVariantList>

namespace sy1000 {

// QML bridge for managing users in SQLite.
class UserServiceAdapter : public QObject
{
    Q_OBJECT
public:
    explicit UserServiceAdapter(QObject *parent = nullptr);

    // List of users as QVariantMap {id, username, company, isAdmin}.
    Q_INVOKABLE QVariantList users() const;
    Q_INVOKABLE bool addUser(const QString &username, const QString &company, const QString &password, bool isAdmin);
    Q_INVOKABLE bool removeUser(int id);
    Q_INVOKABLE bool resetPassword(int id, const QString &newPassword);
};

} // namespace sy1000
