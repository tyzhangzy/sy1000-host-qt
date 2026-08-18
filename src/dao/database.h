#pragma once

#include <QString>

// SQLite database manager: open connection, create tables, seed defaults.
class Database
{
public:
    // Open (or create) the SQLite database and initialize schema. Call once at startup.
    static bool initialize();

    // Absolute path of the database file (Documents/QuanshenAppData/SY1000/userInfo.db).
    static QString databasePath();

private:
    static bool createTables();
    static void seed();
};
