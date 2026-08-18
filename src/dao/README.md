# dao/

对应原 WPF 的 `Dao/` + `Models/`。数据访问层。

计划迁移内容：
- `LiteDbRepository` / `Dao` → `QSqlDatabase` + SQLite（用户表 + 统一试验结果表）
- `User` / `UnifiedTestResult` / `Sample` / `TestStandard` 等模型 → C++ 结构
- 历史数据迁移工具：LiteDB 导出 JSON → SQLite 导入
