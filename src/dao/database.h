#pragma once

#include <QString>

// SQLite database manager: open connection, create tables, seed defaults.
class Database
{
public:
    // Open (or create) the SQLite database at the default location and initialize
    // schema. Call once at startup.
    static bool initialize();

    // Same as initialize() but at an explicit file path — used by headless tests
    // so they never touch the real production database (L15).
    static bool initialize(const QString &databaseFilePath);

    // Absolute path of the default database file (Documents/QuanshenAppData/SY1000/sy1000_qt.db).
    static QString databasePath();

private:
    static bool createTables();
    static void seed();
};
