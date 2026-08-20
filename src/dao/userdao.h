#pragma once

#include <vector>

#include <QString>

#include "models/model.h"

// Data access for the User table (uses the std::string model from src/models).
class UserDao
{
public:
    static std::vector<sy1000::User> findAll();
    static sy1000::User findByUsername(const std::string &username);
    // Authenticate by username + plaintext password. Replaces the old
    // findByUsernameAndPassword which compared plaintext in SQL (H2).
    static sy1000::User authenticate(const std::string &username, const std::string &password);
    // Password hashing helpers used by services before persisting a password (H2).
    static QString hashPassword(const QString &password);
    static bool verifyPassword(const QString &password, const QString &storedHash);
    static int insert(const sy1000::User &u); // returns new id
    static bool remove(int id);
    static bool update(const sy1000::User &u);
};
