#include "dao/userdao.h"

#include <QSqlQuery>
#include <QVariant>

using sy1000::User;

namespace {
User rowToUser(const QSqlQuery &q)
{
    User u;
    u.id = q.value(0).toInt();
    u.username = q.value(1).toString().toStdString();
    u.company = q.value(2).toString().toStdString();
    u.password = q.value(3).toString().toStdString();
    u.isAdmin = q.value(4).toInt();
    return u;
}
} // namespace

std::vector<User> UserDao::findAll()
{
    std::vector<User> out;
    QSqlQuery q(QStringLiteral("SELECT id, username, company, password, is_admin FROM users"));
    while (q.next())
        out.push_back(rowToUser(q));
    return out;
}

User UserDao::findByUsername(const std::string &username)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id, username, company, password, is_admin FROM users WHERE username = ?"));
    q.addBindValue(QString::fromStdString(username));
    if (q.exec() && q.next())
        return rowToUser(q);
    return {};
}

User UserDao::findByUsernameAndPassword(const std::string &username, const std::string &password)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id, username, company, password, is_admin FROM users"
                             " WHERE username = ? AND password = ?"));
    q.addBindValue(QString::fromStdString(username));
    q.addBindValue(QString::fromStdString(password));
    if (q.exec() && q.next())
        return rowToUser(q);
    return {};
}
