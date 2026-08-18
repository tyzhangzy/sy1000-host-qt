# core/

对应原 WPF 的 `HydroStaticTestPage/StateMachine/`。

计划迁移内容：
- `HydrostaticTestController`（宏观状态机，手写 C++ `enum class + switch`，不使用 QStateMachine）
- 子任务：`PressurizeTask` / `HoldTask` / `LeakCheckTask` / `ReleaseTask` / `WaitTask` / `StabilizeTask` / `CylinderCheckTask`
- `HydroTestData` / `TaskParams` / `TaskResult` / `HydroTestError` 等数据结构

> 决策：状态机采用手写 C++ 异步状态机 1:1 平移（详见原仓库 `Qt6_CMake_CrossPlatform_Implementation.md` §4.2）。
