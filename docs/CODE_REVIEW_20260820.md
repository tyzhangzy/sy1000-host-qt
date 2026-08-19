# SY1000-host-qt 代码审查报告（2026-08-20 复审）

> 审查日期：2026-08-20
> 审查范围：`D:\LocalSoftware\sy1000-host-qt` 全部源码 / QML / 构建配置（静态审查，未构建运行）
> 对照：`docs/CODE_REVIEW.md`（2026-08-19 首轮）——本文档会标注每项问题的状态（⚪ 遗留 / 🔴 新增）。

---

## 总体评价

架构仍维持首轮评价的高水准：models（纯标准库）/ dao / devices / core（State 模式）/ services / report / charts / QML 分层清晰，`IHydroDeviceProvider` 硬件解耦与 headless 冒烟测试是亮点。

但首轮报告中的**高优先级问题（密码明文、多样品数据丢失、串口阻塞）至今全部未修复**，且本轮新增发现 1 个直接影响试验数据可信度的高优先级缺陷（**试验标准/残变率限值未真正用于判定与落库**）。另外存在一条**监管风险**：未完成外观检查即可开始试验并生成"合格"存档记录。

---

## 🔴 高优先级问题

### H1（新增）准备页输入的试验标准未真正用于判定与落库 —— 界面与数据不一致

- **位置**：`src/services/hydroadapter.cpp:188-193`（`setTestStandard` 仅存成员）、`:250-293`（`buildResult` 硬编码）、`:80`（限值固定 3.0）
- **问题链**：
  1. 准备页标准卡"确定"确实调用了 `hydro.setTestStandard(standardName, holdTime, residualRate)` 与 `setWorkingPressure/setTestingPressure`。
  2. 但 `buildResult()` 中：`ts.standardName = QStringLiteral("Demo")`、`ts.residualDeformationRate = 3.0`——`m_standardName`（默认 GB/T9251-2022）、`m_residualRate`（默认 5）、`m_holdTime` 完全未使用。
  3. 控制器残变率限值在适配器构造时固定为 `setResidualDeformationRateLimit(3.0)`，`setTestStandard` 从不更新它 → **合格/不合格判定永远按 3.0 算**，与准备页输入的 5% 无关。
- **后果**：UI 显示"GB/T9251-2022 / 5%"；实际判定用 3.0；报告 PDF 打印标准名"Demo"、限值 3.0。这是试验数据可信度问题，属必改项。
- **修复**：`buildResult()` 改用 `m_standardName / m_residualRate / m_holdTime`；`setTestStandard()` 内同步调用 `m_controller.setResidualDeformationRateLimit(double(residualRate))`。

### H2（遗留 #1）密码明文存储 + 硬编码默认密码 admin/9999

- `src/dao/database.cpp:90` 种子密码明文；`userdao.cpp:40-50` 明文 SQL 比对；无任何哈希。
- 建议：`QCryptographicHash` 加盐哈希、登录改"按用户名取哈希再比对"、移除默认密码。

### H3（遗留 #2）多样品试验数据丢失

- `src/services/hydroadapter.cpp:250-293` `buildResult()` 只读取 `m_samples[1]` 与 `d.initialWeights[1]...`，第 2~4 个样品的重量/变形/检查数据全部丢弃。
- 4 条实时曲线与存档结果不一致。建议 `UnifiedTestResult` 支持多样品（vector\<Sample\>），或 UI 明确限制单样品。

### H4（遗留 #3、#7）串口阻塞主线程 + 称重轮询从未启动

- `src/devices/modbusrtu.cpp:119-136` `waitForBytesWritten/waitForReadyRead` 各 200ms 同步阻塞主线程；`precisa.cpp:43-53` 每轮 4 从站最坏阻塞 ~800ms，UI 卡顿。
- `PrecisaScale::startContinuousReading()` 无任何调用点 → 真机模式下电子秤读数恒为 0（当前被 `SimulatedDeviceProvider` 掩盖）。
- 建议：QSerialPort 异步 readyRead 信号驱动或移入工作线程；连接成功后启动连续轮询。

### H5（新增，监管风险）未完成外观检查也可直接开始试验并存档"合格"记录

- `qml/TestPreparationPage.qml:128` "开始水压试验"按钮只做 `setSampleInspection` + `stack.push("TestPage.qml")`，**不校验** `saveSample()` 是否成功、是否已做外观检查。
- 而 `buildResult()` 会把 `inspectionCompleted=false`、四项结果默认"合格"(0) 写入存档 → 可能产生"未经外观检查即判定合格"的正式记录。
- 建议："开始"前强制 4 个样品通过 `saveSample` 校验（至少要求已填信息 + `inspectionCompleted`）。


---

## 🟡 中优先级问题

### M1（遗留 #5）`stopTest` 状态切换依赖脆弱守卫 + 确认弹窗残留

- `src/core/controller.cpp:21-29`：`m_currentTask->stop()` 同步触发 `onSubTaskFinished(Cancelled)→transitionTo(Aborted)`，随后 `stopTest` 又 `transitionTo(Aborted)`，靠 `m_state==next` 守卫兜底。
- 停止后 `HydroTestMessageDialog`（泄压确认弹窗）不会自动关闭；操作员事后点"确定"会唤醒已停止任务的 `confirmResponse` 回调（`ReleaseTask` 回调里会启动一个空转 QTimer，tick 因 `stopped()` 提前返回但定时器持续存在直至任务被 `deleteLater`）。
- 建议：`stopTest` 中先置 `m_currentTask=nullptr` 再 stop；停止时主动关闭/忽略待处理确认；`finish()` 时清空 `m_confirmCallback`。

### M2（遗留 #4）设备写操作失败静默

- `src/devices/tasio.cpp:44-67`：`setWaterInlet/setFastPump/setSlowPump/setWaterJacketLock` 均不检查 `writeSingleCoil` 返回值，串口断开时 UI 无反馈、状态机继续运行（保压/泄压阶段尤其危险）。
- 另：`controller.cpp:117-123` `safeShutdown()` 只解锁 1 号水套，未释放 2~4 号。

### M3（遗留 #12）`TestResultDao::rowToResult` 列与 payload 双写冗余且互相覆盖

- `src/dao/testresultdao.cpp:15-27`：先读列（manufacturer/overall）再被 `unifiedTestResultFromJson(payload)` 整体覆盖；payload 缺字段时列数据丢失。建议只存 payload，列仅作索引/查询字段，或做回退合并。

### M4（新增）`ResultServiceAdapter` 按 id 查询为全表扫描 + 全量 JSON 解析

- `src/services/resultservice.cpp` `details()/reportData()/generatePdf()` 均遍历 `findAll()`（解析全部 payload）再匹配 `r.id`。
- 建议 DAO 增加 `findById(int)` 走主键查询。

### M5（新增）`TestPage`/`TestPreparationPage` 存在死按钮

- `qml/TestPage.qml:111-112` "保存试验结果 / 查看试验报告"无 onClicked（保存仅在 `testCompleted` 自动执行，报告按钮完全无效）；`qml/TestPreparationPage.qml:135-136` 同款按钮 `enabled:false`。
- 建议：接通"查看试验报告"→ `stack.push("ReportViewPage.qml", {resultId})`（需先记录自动保存返回的 id）。

### M6（遗留 #10）i18n 不完整：C++ 状态文本中英混杂

- `src/core/tasks.cpp:89-105`（WaitTask 英文 "Holding, %1 s"）、`:161-187`（ReleaseTask 中文"泄压中…"）、`states.cpp` 的 `statusChanged`（英文）；均不走翻译。语言切换只对 QML 生效。
- 建议 C++ 侧字符串统一走 `QCoreApplication::translate`。

### M7（新增）状态栏显示原始枚举整数

- `qml/TestPage.qml:56`、`qml/TestPreparationPage.qml:90`：`qsTr("Status: ") + hydro.state` 显示 `0..13` 数字而非状态名称。建议 QML 映射 `HydroTestState` 枚举到文案（或 adapter 提供 `stateName()`）。

### M8（遗留 #6）串口 COM1/COM2 硬编码

- `src/services/deviceservice.cpp:36-49`。在麒麟/UOS（ttyS0/ttyUSB0）上失效。建议枚举串口 + config.json 配置。

### M9（遗留 #8、#17）加压结果字段无意义 + 曲线点无上限

- `states.cpp:69-74` 读取 `r.pressureT10/pressureT30` 存 `workingPressureT10/T30`，但 `PressurizeTask` 从不填充这两个字段（恒 0）。
- `hydroadapter.cpp:110-128` 每 100ms 追加 `m_curvePoints` 无上限 → 长时试验 JSON payload 持续膨胀。

### M10（新增）`RealTimeChart` 线程安全与缓存死代码

- `QQuickPaintedItem` + `FramebufferObject` 时 `paint()` 在渲染线程执行；`m_cacheValid`（`cpp:358`）、`m_yMin/m_yMax/m_rightYMin/m_rightYMax/m_isPaused` 等成员在 GUI 线程无锁写入、渲染线程持锁读取 → 数据竞争。
- 另：`addSeriesValueAt` 每次置 `m_cacheValid=false`，导致 `drawCurveOptimized` 的"单序列增量绘制"分支（`cpp:606-643`）永远走不到（每次都是全量重绘）——死代码且内含陈旧像素未清除的潜在 bug。
- 建议：所有共享状态统一在 `m_dataMutex` 下读写；删除或修好增量分支。

---

## 🟢 低优先级问题 / 改进建议

| # | 项目 | 位置 | 状态 |
|---|------|------|------|
| L1 | 泄漏判定阈值 0.5 硬编码，应配置化 | `src/core/tasks.cpp:146-150` | 遗留 #9 |
| L2 | DB 初始化失败仅 qWarning 不退出，后续全部功能静默失败 | `src/main.cpp:34-36` | 遗留 #11 |
| L3 | PDF 路径 `homePath()/Documents` 非跨平台；文件名未 sanitize | `src/report/testreportgenerator.cpp:194-199` | 遗留 #13 |
| L4 | 报告无分页控制，内容超一页表格可能被切 | `testreportgenerator.cpp:202-210` | 遗留 #14 |
| L5 | 死代码：`DeviceManager::connectAll()`；`voicePrompt` 信号无任何连接（语音功能不存在却发信号） | `devicemanager.cpp:13` / `controller.cpp:46` | 遗留 #15 |
| L6 | 测试为冒烟级；`testcontroller.cpp:85` 依赖 `stateCount >= 10` 脆弱断言 | `src/tests/` | 遗留 #16 |
| L7 | 检验员信息（inspectorName/inspectorCertNo）模型与序列化均支持，但 QML 无录入入口 → 报告"检验员"恒空 | `qml/AppearanceInspectionPage.qml` 等 | 新增 |
| L8 | 新用户 `create_date` 硬编码 "2026-01-01 00:00:00"；users 表无唯一用户名约束；最后一个 admin 可被删除 | `src/dao/userdao.cpp:60` | 新增 |
| L9 | `TestResultDao::count()`、`Database::seed()` 的 `q.next()` 未检查返回值 | `testresultdao.cpp:113-117` / `database.cpp:82-84` | 遗留 #18 |
| L10 | `SimulatedDeviceProvider::currentPressure()` 为 const 却修改 `mutable m_pressure`，演示流程依赖该 hack | `src/core/simdevice.h:15-20` | 遗留 #18 |
| L11 | 登录成功欢迎语被 `Connections::onLoginSucceeded` 立即切页覆盖，永远看不见 | `qml/LoginPage.qml:164-167` | 遗留 #18 |
| L12 | 新增用户成功未清空 company 输入框 | `qml/UserManagementPage.qml:29-33` | 遗留 #18 |
| L13 | `main.cpp` engine 先声明、服务后声明 → 析构时服务先于 engine 销毁，QML 销毁期绑定可能引用已销毁对象 | `src/main.cpp:41-85` | 新增 |
| L14 | `ReportViewPage.qml:151` `Qt.openUrlExternally("file:///"+p)` 未转义空格/中文，建议 `QUrl::fromLocalFile` | `qml/ReportViewPage.qml` | 新增 |
| L15 | headless 测试共享真实数据库（Documents 下），多次运行污染数据、并行互扰 | `src/tests/testcore.cpp` / `testlogin.cpp` | 新增 |
| L16 | 无 install 规则、无 windeployqt/linuxdeployqt 打包步骤（麒麟/UOS 交付需补齐）；`qt_standard_project_setup()` 需 Qt≥6.3 而 `find_package` 声明 6.2 | `CMakeLists.txt:9-10` | 新增 |
| L17 | include 卫生：`tasio.cpp` 用 `memcpy` 未含 `<cstring>`；`testresultservice.cpp` 用 `std::time/localtime` 未含 `<ctime>` | `tasio.cpp:20` / `testresultservice.cpp` | 新增 |
| L18 | 准备页默认压力 30/45 与 hydro 默认 25/37.5 不一致；不点标准卡"确定"则实际按 25/37.5 试验；"开始"不校验 4 样品已填 | `qml/TestPreparationPage.qml:23-27` | 新增 |
| L19 | `ReleaseTask` 只等倒计时，不监测压力是否实际下降；压力未释放仍继续出结果 | `src/core/tasks.cpp:157-188` | 新增 |
| L20 | 主菜单/侧栏可重复 push 同一页面（无去重）；Drawer `modal:false` 点击遮罩不关闭 | `qml/Main.qml:143-181` | 新增 |

---

## 首轮报告问题核对（2026-08-19 → 2026-08-20）

| 首轮 # | 问题 | 状态 |
|--------|------|------|
| 1 | 密码明文 | ⚪ 未修复 |
| 2 | 多样品数据丢失 | ⚪ 未修复 |
| 3 | 串口阻塞主线程 | ⚪ 未修复 |
| 4 | 设备写失败静默 | ⚪ 未修复 |
| 5 | stopTest 重复状态切换 | ⚪ 未修复 |
| 6 | COM 硬编码 | ⚪ 未修复 |
| 7 | 称重轮询未启动 | ⚪ 未修复 |
| 8 | 加压结果字段空值 | ⚪ 未修复 |
| 9 | 泄漏阈值硬编码 | ⚪ 未修复 |
| 10 | i18n 不完整 | ⚪ 未修复 |
| 11 | DB 失败仅告警 | ⚪ 未修复 |
| 12 | rowToResult 覆盖 | ⚪ 未修复 |
| 13 | PDF 路径跨平台 | ⚪ 未修复 |
| 14 | 报告分页 | ⚪ 未修复 |
| 15 | 死代码 | ⚪ 未修复（`connectAll` 仍在） |
| 16 | 冒烟测试 | ⚪ 未修复 |
| 17 | 采样性能/曲线点膨胀 | ⚪ 未修复 |
| 18 | 其他小问题 | ⚪ 未修复（4 项均确认仍存在） |

> 结论：首轮 18 项问题在最近 5 个提交（均为 UI 样式微调：边框/下划线/标题栏背景色）后**无一项得到修复**。建议把修复提上日程，优先 H1/H2/H3/H5。

---

## 建议修复顺序

1. **立即（数据可信度/合规）**：H1 试验标准真正落库与判定；H5 强制检查完成才可开始试验；H2 密码哈希。
2. **下一迭代（功能完整）**：H3 多样品存档（或限单样品）；M5 接通报告查看按钮；M4 按 id 查询；M7 状态枚举显示。
3. **随后（硬件可靠性）**：H4 串口异步化 + 启动称重轮询；M2 设备写失败传播 + `safeShutdown` 解锁全部；M8 串口自动识别。
4. **持续**：M10 图表线程安全清理、M6 i18n、L 系列小项、单测框架引入。

