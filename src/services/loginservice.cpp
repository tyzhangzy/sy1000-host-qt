#include "services/loginservice.h"

#include <QDateTime>

#include "dao/userdao.h"

LoginService::LoginService(QObject *parent)
    : QObject(parent)
{
}

bool LoginService::tryLogin(const QString &username, const QString &password)
{
    m_error.clear();

    if (username.trimmed().isEmpty() || password.isEmpty()) {
        m_error = QStringLiteral("Please enter username and password.");
        return false;
    }

    sy1000::User user = UserDao::findByUsernameAndPassword(username.toStdString(), password.toStdString());
    if (user.username.empty()) {
        m_error = QStringLiteral("Login failed: invalid username or password.");
        return false;
    }

    m_username = QString::fromStdString(user.username);
    m_company = QString::fromStdString(user.company);
    m_isAdmin = user.isAdmin != 0;

    emit loginSucceeded();
    return true;
}

QString LoginService::errorMessage() const
{
    return m_error;
}

QStringList LoginService::usernames() const
{
    QStringList list;
    for (const auto &u : UserDao::findAll())
        list << QString::fromStdString(u.username);
    return list;
}

QString LoginService::username() const
{
    return m_username;
}

QString LoginService::company() const
{
    return m_company;
}

bool LoginService::isAdmin() const
{
    return m_isAdmin;
}
