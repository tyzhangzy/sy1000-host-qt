# SY1000-host-qt 代码审查报告

> 审查日期：2026-08-19
> 审查范围：`D:\LocalSoftware\sy1000-host-qt` 全部源码、QML 界面、构建配置与文档

---

## 总体评价

项目架构清晰、分层合理：`models`（纯标准库）/ `dao` / `devices` / `core`（State 模式）/ `services` / `report` / `charts` / QML 界面，耦合度低，通过 `IHydroDeviceProvider` 接口解耦硬件便于测试，并自带 headless 冒烟测试。整体代码质量良好，可作为 Qt6 跨平台重构的可靠基础。但存在若干安全、硬件与可靠性问题需要修复。

---

## 🔴 高优先级问题

### 1. 密码明文存储与硬编码默认密码

- **位置**：`src/dao/database.cpp:90`、`src/dao/userdao.cpp:40-50`、`src/services/loginservice.cpp:21`
- **问题**：admin 密码硬编码为 `"9999"` 明文入库；`findByUsernameAndPassword` 直接用明文 SQL 比对；无任何哈希（SHA-256/PBKDF2/bcrypt）。
- **风险**：数据库中全员密码可读，任何能访问 `Documents/QuanshenAppData/SY1000/sy1000_qt.db` 的人可窃取全部凭据。
- **建议**：改用 Qt 自带 `QCryptographicHash`（或引入第三方库）加盐哈希存储；登录改为"按用户名查哈希再比对"；移除硬编码默认密码，首次启动强制设置。

### 2. 多样品试验数据丢失（功能不完整）

- **位置**：`src/services/hydroadapter.cpp:250-293` `buildResult()`
- **问题**：4 个样品在 QML 中完整录入并显示 4 条实时曲线，但 `buildResult()` 只读取 `m_samples[1]`（第 1 个样品）的数据，另外 3 个样品的结果被丢弃。
- **建议**：`UnifiedTestResult` 应支持多样品（vector\<Sample\>），或明确当前版本仅单样品试验并在 UI 上限制只填 1 个样品。

### 3. 串口阻塞式读取运行在主线程

- **位置**：`src/devices/modbusrtu.cpp:119-136` `sendFrame()`、`src/devices/precisa.cpp:43-53` `pollOnce()`
- **问题**：`waitForBytesWritten/waitForReadyRead`（各 200ms）在主线程同步阻塞。`PrecisaScale` 每轮轮询 4 个从站，最坏阻塞 800ms；UI 会被卡顿。
- **建议**：改造为非阻塞异步（QSerialPort readyRead 信号驱动）或移入工作线程；至少将轮询放入 `QThread`，通过信号跨线程更新。

---

## 🟡 中优先级问题

### 4. 设备控制调用静默失败

- **位置**：`src/devices/tasio.cpp:44-67`
- **问题**：`setWaterInlet/setFastPump/setSlowPump/setWaterJacketLock` 均不检查 `writeSingleCoil` 返回值，写入失败时（如串口断开）UI 无任何反馈，状态机却继续运行。
- **建议**：返回 bool 并向上传播错误，状态机在设备错误时应中止（`HydroTestError::DeviceError`）。

### 5. 状态机 `stopTest` 存在重复状态切换

- **位置**：`src/core/controller.cpp:21-29`
- **问题**：`m_currentTask->stop()` 会同步 emit `finished(Cancelled)` → `onSubTaskFinished` → `transitionTo(Aborted)`；随后 `stopTest()` 又直接调用 `transitionTo(Aborted)`。虽有 `if (m_state == next) return` 保护，但 `m_currentTask` 未置空，路径依赖脆弱的守卫逻辑。
- **建议**：`stopTest` 中先置 `m_currentTask=nullptr` 再 stop，或在 stop 时标记抑制后续回调；明确"单次停止只触发一次 transition"。

### 6. 硬编码 COM 口，无自动识别

- **位置**：`src/services/deviceservice.cpp:36-49`
- **问题**：`connectDevices()` 硬编码 `COM1`/`COM2`，与 TODO C8 一致（真机未接入）。在国产系统（串口名 ttyS0/ttyUSB0）上会失效。
- **建议**：枚举系统串口列表 + 配置驱动（config.json 增加串口配置），或按设备特征（TasIO/天平协议）自动探测。

### 7. 称重数据永不刷新（真机模式下）

- **位置**：`src/devices/precisa.cpp:8-9`、`src/devices/devicemanager.cpp:13-18`
- **问题**：`startContinuousReading()` 从未被任何代码调用，电子秤轮询定时器从未启动。当前因 main.cpp 使用 `SimulatedDeviceProvider` 而掩盖；接入真机后读数将恒为 0。
- **建议**：DeviceServiceAdapter 连接成功后启动 `startContinuousReading()`，并通过 `weightUpdated` 信号更新 provider 缓存。

### 8. `PressurizeTask` 结果字段未填充导致无意义数据

- **位置**：`src/core/states.cpp:69-74`、`src/core/tasks.cpp:8-79`
- **问题**：`PressurizingToWorkingState::onTaskFinished` 读取 `r.pressureT10/pressureT30` 存入 `workingPressureT10/T30`，但 `PressurizeTask` 从不设置这两个字段（恒为 0）。
- **建议**：删除该赋值，或在加压完成时记录实际到达压力与时间点。

### 9. 泄漏判定阈值硬编码

- **位置**：`src/core/tasks.cpp:146-150`
- **问题**：压力差/重量差超过 `0.5` 判定泄漏，属业务规则应可配置。
- **建议**：移入 `TestOptions` 或 config.json。

---

## 🟢 低优先级问题 / 改进建议

### 10. i18n 不完整

- `src/core/tasks.cpp:161-186`（泄压状态中文硬编码）、`src/services/hydroadapter.cpp:198-205`（`testStandardInfo` 中文硬编码）、状态机 `statusChanged` 英文硬编码——均不走 `qsTr`，语言切换只对 QML 生效。
- 建议：C++ 侧字符串统一走翻译（QCoreApplication::translate），或将状态文本下放 QML 映射。

### 11. 数据库初始化失败仅告警不退出

- `src/main.cpp:34-36`：`Database::initialize()` 失败只 `qWarning`，后续登录/查库全部失败但 UI 照常启动。建议启动时强校验并提示。

### 12. `TestResultDao::rowToResult` 列与 payload 双写冗余

- `src/dao/testresultdao.cpp:15-27`：先读列再被 JSON payload 整体覆盖。若 payload 缺字段，列数据丢失。建议二选一：只存 payload，列仅作索引；或列缺失时回退。

### 13. PDF 路径跨平台与文件名安全

- `src/report/testreportgenerator.cpp:194,199`：`QDir::homePath()+"/Documents/"` 在 Linux/麒麟 上需用 `QStandardPaths::DocumentsLocation`；文件名含制造商/序列号未 sanitize，遇 `/` 等特殊字符会失败。

### 14. 报告跨页/分页控制缺失

- `testreportgenerator.cpp` 直接 `doc.print(&printer)`，无分页控制。内容超一页时表格可能被切分。建议用 `QTextDocument` 分页或拆成多段。

### 15. 死代码

- `DeviceManager::connectAll()`（`src/devices/devicemanager.cpp:13`）未被调用；`PrecisaScale::startContinuousReading()` 未被调用。

### 16. 测试为冒烟级，无单元测试框架

- `src/tests/` 均为打印 PASS/FAIL 的 smoke test；`testcontroller.cpp:85` 依赖 `stateCount >= 10` 的脆弱断言。建议引入 QtTest/GoogleTest 对 DAO、序列化、状态机做细粒度断言。

### 17. 采样/重绘性能

- `src/services/hydroadapter.cpp:110` 每 100ms 发 5 个信号 + 记录曲线点（`m_curvePoints` 无上限，长时试验内存增长）。建议对 `m_curvePoints` 限长/降采样。

### 18. 其他小问题

- `SimulatedDeviceProvider::currentPressure()` 为 `const` 却修改 `mutable m_pressure`（`src/core/simdevice.h:15`），语义不佳。
- `TestResultDao::count()` 中 `q.next()` 未检查返回值（`src/dao/testresultdao.cpp:113`）。
- `Main.qml` 登录成功欢迎语被 `Connections::onLoginSucceeded` 立即切页覆盖，一闪而过无意义（`qml/LoginPage.qml:164-167`）。
- `UserManagementPage` 新增用户成功时未清空 `newCompany` 字段（`qml/UserManagementPage.qml:29-33`）。

---

## 架构亮点

- ✅ **分层清晰**：models（纯标准库，跨平台）→ dao → services → UI，依赖方向正确。
- ✅ **状态机 State 模式**：`HydroStateBase` 每个状态独立类，`onTaskFinished/onTaskFailed` 明确，扩展新阶段容易。
- ✅ **硬件解耦**：`IHydroDeviceProvider` 接口 + `SimulatedDeviceProvider`，无硬件可全流程跑通。
- ✅ **Modbus RTU 实现规范**：CRC16、寄存器字节序、帧校验正确。
- ✅ **JSON 序列化完整**：`json_serializer` 覆盖所有模型字段，曲线点持久化并用于报告。
- ✅ **CI 友好的 headless 测试**：不依赖 GUI 可验证核心逻辑。

---

## 建议修复顺序

1. **立即**：密码哈希 + 移除硬编码默认密码（高）
2. **下一迭代**：`buildResult` 多样品支持 或 UI 限制单样品（高）；串口异步化（高）
3. **随后**：设备调用返回值传播、`stopTest` 状态切换清理、COM 自动识别、启动称重轮询、加压结果字段修正
4. **持续**：i18n 补全、单元测试框架引入、配置化阈值、PDF 跨平台路径
