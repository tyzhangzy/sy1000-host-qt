# SY1000-host-qt 待办清单（明日实现计划）

> 基于当前已完成进度，整理接下来的实现计划，按优先级排列。
> 项目：`D:\LocalSoftware\sy1000-host-qt`（Qt6 + CMake，重构自 `sy1000-host-wpf`）

---

## 已完成（现状）

- 五层架构：`models`（纯 C++ 标准库）/ `dao`（SQLite）/ `services` / `devices`（QSerialPort+Modbus）/ `core`（状态机 State 模式）
- 9 个 QML 页面：登录、主菜单、试验准备（含外观检查对话框）、试验页（状态机+实时压力曲线）、结果管理、结果详情、用户管理、系统维护
- 中英 i18n、Material 风格（Indigo/Amber）、全流程闭环（登录→准备→试验→存结果→查详情）
- 3 个 headless 冒烟测试全绿；已全部同步 GitHub
- **A1 外观检查补全**：`AppearanceInspectionDialog.qml` 已重写为四部分完整表单（检验员信息 + External/Internal/Thread/Valve 逐项检查、缺陷位置、其他备注）
- **C6 外观检查接入结果保存**：`HydroTestControllerAdapter::setSampleInspection()` 接收 QML 检查对象，`buildResult()` 写入 `sample.appearanceInspection` 持久化
- **B4 4 样品双轴曲线**：`RealTimeChart` 扩为多序列 + 双 Y 轴（左轴压力 MPa、右轴重量 g），试验页显示压力曲线 + 1-4 号样品曲线（`addSeries`/`addSeriesValue`/`weightSample` 信号）
- **A3 水压试验消息对话框**：`HydroSubTask` 支持阻塞式确认请求（`requestConfirmation`/`confirmResponse`），`ReleaseTask` 泄压前弹窗"打开泄压阀并确认"；`HydroTestMessageDialog.qml` 接通 `hydro.respondConfirm()`

---

## 明日待办（按优先级）

### A. 未实现的独立界面（3 个）
| 界面 | 对应原 WPF | 说明 |
|------|-----------|------|
| 1. ~~**外观检查窗口补全**~~ | `AppearenceInspectionWindow` | ✅ 已完成：四部分逐项 bool 检查、检验员信息（姓名/证书号/日期）、缺陷位置、"其他"备注 |
| 2. **测试报告查看窗口** | `TestReportWindow` | 用 QML 内嵌 PDF 查看（`QPdfView`）显示生成的报告，支持多报告翻页 |
| 3. ~~**水压试验消息对话框**~~ | `HydroTestMessageWindow` | 状态机弹出操作指令 + OK/取消确认（如"打开泄压阀"、"是否继续"），需接通 `HydroSubTask` 的指令/确认请求 | ✅ 已完成：阻塞式确认请求 + 泄压确认弹窗 |

### B. 页面增强
| 项 | 说明 |
|----|------|
| 4. ~~**4 样品双轴曲线**~~（试验页） | 把 `RealTimeChart` 扩为双 Y 轴（压力 MPa / 变形量 g），试验页显示 1-4 号样品曲线，对齐 WPF `HydroStaticTestPage`/`SampleGridUserControl` | ✅ 已完成：多序列双轴图 + 4 样品曲线 |
| 5. **试验结果管理拆分** | 对齐 WPF，将结果管理拆为"试验结果管理 + 统一试验结果管理"（或在一页内分区）|

### C. 功能 / 数据接入
| 项 | 说明 |
|----|------|
| 6. ~~**外观检查接入结果保存**~~ | 把外观检查结果传给 `HydroTestControllerAdapter.buildResult()`，试验完成入库时带上检查数据 | ✅ 已完成：`setSampleInspection()` → `buildResult()` 写入 `sample.appearanceInspection` |
| 7. **PDF 报告生成** | 用 `QTextDocument`/`QPrinter` 生成试验报告（标题/表单/曲线图），对齐 WPF `TestReportGenerator` |
| 8. **真实 DeviceManager 接入** | 把 `main.cpp` 的 `SimulatedDeviceProvider` 换成真实 `DeviceManager`（需真机 COM 口），并做 COM 口自动识别 |
| 9. **config.json 加载** | 建 `ConfigManager`，从 `config.json` 读设备名/厂商/序列号，登录页与系统维护页显示真实信息 |
| 10. **数据库迁移工具** | LiteDB → SQLite 数据迁移（历史用户/试验结果导入 `sy1000_qt.db`）|

### D. 细节微调（布局/样式对齐 WPF）
| 项 | 说明 |
|----|------|
| 11. 登录页中文 | WPF 为中文界面（"用 户 登 录 / 登 录"），微调默认语言或补充中文文案 |
| 12. 真实 Logo | 引入 `dklogo.png`（原 WPF Resources），替换占位矩形 |
| 13. 配色/字体统一 | 统一 Indigo/Amber 用色；接入思源/Noto 字体（Noto Sans SC / Noto Serif SC），对齐字号字重 |
| 14. 控件样式 | 按钮/卡片 hover、圆角、内边距、卡片阴影（Elevation）精细化 |

---

## 建议实施顺序

1. **A1 外观检查补全**（试验主流程必经，价值最高）
2. **C6 外观检查接入结果保存**（和 A1 一起闭环）
3. **B4 4 样品双轴曲线**（水压试验核心界面）
4. **A3 水压试验消息对话框**（状态机交互）
5. **C7 PDF 报告生成 + A2 报告查看**（报告闭环）
6. 其余（C8/C9/C10、B5、D 系列）按需推进

---

## 备注

- 布局定位要点：Qt Quick 中并排的 `Item`/`Rectangle` 需显式 `anchors.left` 定位，否则重叠（登录页已修复，其余页面注意）。
- 代码注释统一英文（避免编码兼容问题）。
- 每一步改动后：构建 + 运行验证 + headless 冒烟测试 + push 到 GitHub。
