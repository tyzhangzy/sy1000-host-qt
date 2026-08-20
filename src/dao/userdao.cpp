#include "dao/userdao.h"

#include <QByteArray>
#include <QDateTime>
#include <QCryptographicHash>
#include <QRandomGenerator>
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

QString nowText()
{
    return QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
}

// Stored as "base64(sha256(password+salt)):base64(salt)".
constexpr int SALT_BYTES = 16;
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

QString UserDao::hashPassword(const QString &password)
{
    if (password.isEmpty())
        return QString();
    QByteArray salt(SALT_BYTES, Qt::Uninitialized);
    QRandomGenerator::system()->fillRange(reinterpret_cast<quint32 *>(salt.data()),
                                          salt.size() / static_cast<int>(sizeof(quint32)));
    const QByteArray hash = QCryptographicHash::hash(password.toUtf8() + salt, QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toBase64() + ':' + salt.toBase64());
}

bool UserDao::verifyPassword(const QString &password, const QString &storedHash)
{
    if (password.isEmpty() || storedHash.isEmpty())
        return false;
    const QByteArray stored = storedHash.toLatin1();
    const int sep = stored.indexOf(':');
    if (sep <= 0)
        return false;
    const QByteArray hash = QByteArray::fromBase64(stored.left(sep));
    const QByteArray salt = QByteArray::fromBase64(stored.mid(sep + 1));
    if (hash.isEmpty())
        return false;
    return hash == QCryptographicHash::hash(password.toUtf8() + salt, QCryptographicHash::Sha256);
}

User UserDao::authenticate(const std::string &username, const std::string &password)
{
    User u = findByUsername(username);
    if (u.id == 0)
        return {};

    const QString plain = QString::fromStdString(password);
    const QString stored = QString::fromStdString(u.password);

    // New hashed format.
    if (verifyPassword(plain, stored))
        return u;

    // Backward compatibility: databases created before H2 stored plaintext
    // passwords. Allow them to log in and migrate to hashed storage.
    if (plain == stored) {
        User updated = u;
        updated.password = hashPassword(plain).toStdString();
        if (update(updated))
            u.password = updated.password;
        return u;
    }

    return {};
}

int UserDao::insert(const User &u)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("INSERT INTO users (username, company, password, create_date, is_admin)"
                             " VALUES (?, ?, ?, ?, ?)"));
    q.addBindValue(QString::fromStdString(u.username));
    q.addBindValue(QString::fromStdString(u.company));
    q.addBindValue(QString::fromStdString(u.password));
    q.addBindValue(nowText());   // real creation timestamp instead of a hard-coded date (L8)
    q.addBindValue(u.isAdmin);
    if (!q.exec())
        return 0;
    return q.lastInsertId().toInt();
}

bool UserDao::remove(int id)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("DELETE FROM users WHERE id = ?"));
    q.addBindValue(id);
    return q.exec();
}

bool UserDao::update(const User &u)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("UPDATE users SET username=?, company=?, password=?, is_admin=? WHERE id=?"));
    q.addBindValue(QString::fromStdString(u.username));
    q.addBindValue(QString::fromStdString(u.company));
    q.addBindValue(QString::fromStdString(u.password));
    q.addBindValue(u.isAdmin);
    q.addBindValue(u.id);
    return q.exec();
}
