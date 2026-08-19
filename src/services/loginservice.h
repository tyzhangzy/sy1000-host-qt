#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

// Login service exposed to QML as a context property. Bridges the Qt UI layer
// to the std::string DAO/models layer.
class LoginService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
public:
    explicit LoginService(QObject *parent = nullptr);

    // Validate username/password against the database. On success remembers the session.
    Q_INVOKABLE bool tryLogin(const QString &username, const QString &password);

    // Latest error message (empty on success).
    Q_INVOKABLE QString errorMessage() const;

    // All usernames (for the login page dropdown).
    Q_INVOKABLE QStringList usernames() const;

    Q_INVOKABLE QString username() const;
    Q_INVOKABLE QString company() const;
    Q_INVOKABLE bool isAdmin() const;

signals:
    void loginSucceeded();
    void usernameChanged();

private:
    QString m_error;
    QString m_username;
    QString m_company;
    bool m_isAdmin = false;
};
