#pragma once

#include <vector>

#include "models/model.h"

// Data access for the User table (uses the std::string model from src/models).
class UserDao
{
public:
    static std::vector<sy1000::User> findAll();
    static sy1000::User findByUsername(const std::string &username);
    static sy1000::User findByUsernameAndPassword(const std::string &username, const std::string &password);
};
