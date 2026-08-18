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
    static int insert(const sy1000::User &u); // returns new id
    static bool remove(int id);
    static bool update(const sy1000::User &u);
};
