#pragma once

#include <string>
#include <vector>

#include "models/model.h"

// Data access for the unified_test_results table.
class TestResultDao
{
public:
    static int insert(const sy1000::UnifiedTestResult &r);                 // returns new id
    static bool update(const sy1000::UnifiedTestResult &r);
    static bool remove(int id);
    static sy1000::UnifiedTestResult findBySerialNo(const std::string &serialNo);
    static sy1000::UnifiedTestResult findById(int id);                     // primary-key lookup (M4)
    static std::vector<sy1000::UnifiedTestResult> findAll();
    static std::vector<sy1000::UnifiedTestResult> getLatest(int count);
    static int count();
};
