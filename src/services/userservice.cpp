#include "services/userservice.h"

#include <QVariantMap>

#include "dao/userdao.h"

namespace sy1000 {

UserServiceAdapter::UserServiceAdapter(QObject *parent)
    : QObject(parent)
{
}

QVariantList UserServiceAdapter::users() const
{
    QVariantList out;
    for (const auto &u : UserDao::findAll()) {
        QVariantMap m;
        m[QStringLiteral("id")] = u.id;
        m[QStringLiteral("username")] = QString::fromStdString(u.username);
        m[QStringLiteral("company")] = QString::fromStdString(u.company);
        m[QStringLiteral("isAdmin")] = u.isAdmin != 0;
        out.append(m);
    }
    return out;
}

bool UserServiceAdapter::addUser(const QString &username, const QString &company,
                                 const QString &password, bool isAdmin)
{
    if (username.trimmed().isEmpty() || password.isEmpty())
        return false;
    sy1000::User u;
    u.username = username.toStdString();
    u.company = company.toStdString();
    u.password = password.toStdString();
    u.isAdmin = isAdmin ? 1 : 0;
    return UserDao::insert(u) > 0;
}

bool UserServiceAdapter::removeUser(int id)
{
    return UserDao::remove(id);
}

bool UserServiceAdapter::resetPassword(int id, const QString &newPassword)
{
    for (auto &candidate : UserDao::findAll()) {
        if (candidate.id == id) {
            candidate.password = newPassword.toStdString();
            return UserDao::update(candidate);
        }
    }
    return false;
}

} // namespace sy1000
