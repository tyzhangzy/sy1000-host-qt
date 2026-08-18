# SY1000 原项目（WPF）数据结构与业务逻辑分析

> 目的：为 `sy1000-host-qt`（Qt6 重构）提供原项目的完整梳理，作为迁移依据。
> 来源：`D:\LocalSoftware\sy1000-host-wpf`（约 1.17 万行 C#/XAML）。

---

## 1. 整体架构与模块

```
App.xaml → LoginWindow → MenuWindow → MainTestWindow
              ├── TestPreparationPage（试验准备：标准 + 1~4 号样品信息）
              ├── HydroStaticTestPage（水压试验：状态机 + 实时曲线 + 结果显示）
              └── 管理窗口（用户 / 试验结果 / 报告 / 系统维护）
```

分层：
| 层 | 目录 | 职责 |
|----|------|------|
| Models | `Models/` | 数据模型 / 枚举 |
| Dao | `Dao/` | LiteDB 数据访问门面 + Repository |
| Services | `Services/` | 业务服务（结果聚合 / 生成 / 判定） |
| Devices | `Devices/` | 硬件通信（TasIO / PrecisaScale / DeviceManager） |
| Config | `Config/` | 配置模型 + 读取 |
| 状态机 | `HydroStaticTestPage/StateMachine/` | 水压试验宏观流程 + 子任务 |
| 界面 | `LoginWindow` `MenuWindow` `MainTestWindow` 等 | 窗口 + ViewModel |

---

## 2. 数据模型（核心实体）

### 2.1 用户 `User`
| 字段 | 类型 | 说明 |
|------|------|------|
| Id | int | 主键 |
| Username | string | 用户名 |
| Company | string | 单位 |
| Password | string | 密码（明文） |
| CreateDate | DateTime | 创建时间 |
| IsAdmin | int | 是否管理员 |

### 2.2 试验标准 `TestStandard`
| 字段 | 类型 |
|------|------|
| StandardName | string |
| WorkingPressure | double?（工作压力 MPa）|
| TestingPressure | double?（试验压力 MPa）|
| PressureHoldingTime | int（保压秒）|
| ResidualDeformationRate | double?（残余变形率 %）|
| ResidualDeformation | double?（残余变形量）|

### 2.3 样品外观检查 `SampleInspectionData`（INotifyPropertyChanged）
- **基本信息**：SampleId, SampleModel, Manufacturer, Volume(decimal,默认6.8), UserCompany, SerialNo, InspectionDate, InspectorName, InspectorCertNo, InspectionCompleted(bool)
- **结果**：ExternalResult / InternalResult / ThreadResult / ValveResult（`InspectionResult`）
- **外观 External**：ThermalDamage?, Scratch?, Wear?, Delamination?, Deformation?（bool?），ExternalDefectLocation, ExternalOther
- **内部 Internal**：InternalSmell?(bool?)，InternalDebris, InternalSurfaceCondition, InternalDefectLocation, InternalOther
- **螺纹 Thread**：ThreadSpecification, ThreadCondition, ThreadEvaluation, ThreadOther
- **瓶阀 Valve**：ValveNo, ValveThreadCondition, ValveAirTightness, ValveDiaphragmReplaced?(bool?)，ValveOther

### 2.4 可序列化版本 `SerializableSampleInspectionData`
- 与 `SampleInspectionData` 字段一致（去掉 INotifyPropertyChanged），用于 LiteDB 存储
- `FromSampleInspectionData(source)` 静态转换

### 2.5 统一试验结果 `UnifiedTestResult`
| 字段 | 类型 |
|------|------|
| Id | int |
| TestSerialNo | string（唯一）|
| TestDate | DateTime |
| TesterName / TesterCompany | string |
| TestStandard | TestStandard |
| Sample | Sample |
| TestEnvironment | TestEnvironmentData |

### 2.6 样品 `Sample`
| 字段 | 类型 |
|------|------|
| SampleId, SampleModel, Manufacturer, SerialNo | string |
| Volume | decimal |
| AppearanceInspection | SerializableSampleInspectionData |
| HydroStaticTest | HydroStaticTestData |
| OverallResult | TestResultStatus |
| Notes | string |

### 2.7 水压试验数据 `HydroStaticTestData`
| 字段 | 类型 |
|------|------|
| InitialWeight / PressureWeight / FinalWeight | double |
| FullDeformation / ResidualDeformation | double |
| ResidualDeformationRate | double |
| WorkingPressure / TestPressure | double |
| PressureWeightData | List\<PressureWeightPoint\> |
| TestResult | TestResultStatus |
| ResultDetails | string |

### 2.8 曲线点 `PressureWeightPoint`
`Timestamp`(DateTime), `Pressure`(double), `Weight`(double)

### 2.9 环境数据 `TestEnvironmentData`
`RoomTemperature`, `Humidity`, `EquipmentId`, `EquipmentModel`

---

## 3. 枚举

| 枚举 | 值 |
|------|----|
| `InspectionResult` | Qualified, ToRepair, ToReplace, Scrapped |
| `TestResultStatus` | NotTested, InProgress, Passed, Failed, Qualified, ToRepair, ToReplace, Scrapped |
| `HydroTestState` | Idle, Preparing, WaterJacketChecking, CylinderChecking, Initializing, PressurizingToWorking, HoldingAtWorking, PressurizingToTesting, HoldingAtTesting, ReleasingPressure, Stabilizing, CalculatingResult, Completed, Aborted |
| `ComConnectionStatus` | CONNECTING, CONNECTED, UNCONNECTED, ERROR |

---

## 4. 数据访问（DAO，LiteDB）

- **数据库文件**：`Documents/QuanshenAppData/SY1000/userInfo.db`
- **集合**：
  - `users`（索引：Username+Company 唯一；种子默认 admin/9999）
  - `unifiedTestResults`（索引：TestSerialNo 唯一, TestDate, TesterName）
- **入口**：
  - `Dao`（静态门面）：`_db` / `_users` / `_unifiedTestResults`，`Initialize()` / `Dispose()`，`InitializeOnce` 创建目录
  - `LiteDbRepository`：封装连接、集合、索引、种子
  - `UnifiedTestResultDao`：CRUD + 查询（FindById / FindBySerialNo / FindAll / FindByDateRange / FindByTester / FindByManufacturer / GetLatest / Count / InsertBulk）

---

## 5. 业务服务（Services）

### 5.1 `UnifiedTestResultService`（静态）
- `CreateUnifiedTestResult(testSerialNo, testerName, testerCompany, sample)` → UnifiedTestResult
- `CreateSampleTestData(sampleInspectionData, hydroStaticTestData)` → Sample（含外观检查转换）
- `SaveUnifiedTestResult(testResult)` → 新插入 / 旧更新
- `DetermineOverallResult(inspectionData, hydroStaticData)` → **判定总结果**（优先：任一报废→Scrapped → 水压结果 → 全合格→Qualified → 默认 NotTested）
- `GenerateTestSerialNo()` → `yyyyMMddHHmm + 3位随机字符`
- `GetLatestTestResults(count)` / `SearchTestResults(testerName, manufacturer, startDate, endDate)`

### 5.2 `TestDataCoordinator`（静态）
- `CollectAndCreateUnifiedTestResults(prepPage, hydroPage, testStandard, testerName, testerCompany)`
  → 遍历 `SavedSamples`(1~4)，每个生成一个 `UnifiedTestResult`（新流水号），附加 TestStandard
- `CreateHydroStaticTestData(sampleGridControl)`：从 `SampleGridUserControl` 取 InitialWeight / TestingPressureWeight / ReleasedPressureWeight / FullDeformation / ResidualDeformation / ResidualDeformationRate / TestResultStatus
- `SaveUnifiedTestResults(list)`：新结果批量插入，旧结果逐个更新
- `GetAllUnifiedTestResults` / `GetUnifiedTestResultById` / `DeleteUnifiedTestResult`

---

## 6. 配置（Config）

### 6.1 `AppConfig`
`DeviceName`, `Manufacturer`, `SerialNo`, `ManufactureDate`, `HP`(SensorConfig), `LP`(SensorConfig)

### 6.2 `SensorConfig`
`SensorId`, `Type`, `Output`(List\<double\>), `Range`(List\<double\>), `V0`, `K`, `Unit`, `SamplingRate`, `CalibrationHistory`(List\<string\>)

### 6.3 `ConfigManager`
- `LoadConfig()` / `SaveConfig()`，读写 `Resources/config.json`（Newtonsoft.Json）
- 登录时 `UserSession.DeviceName = config.DeviceName`

---

## 7. 设备层（Devices）

### 7.1 `TasIO`（Modbus RTU 主站，从站 17）
- 控制：`WaterInlet`、`FastPump`、`SlowPump`、`WaterJacketLock(index, onOff)`（写线圈 DO）
- 压力：读从站 1 寄存器 0~1，`ConvertFrom2143Format` 转换；`PressureUpdated` 事件
- 连续读取线程 + `ReaderWriterLockSlim` 保护；Baud 19200

### 7.2 `PrecisaScale`（多电子天平）
- 按 `availableScales`(HashSet) 轮询多个从站（25ms 间隔 / 50ms 周期）
- `WeightUpdated`（sender = scaleNo）事件；失败降级（MAX_SLAVE_FAILURES）

### 7.3 `DeviceManager`（单例）
- `ConnectTasIO` / `ConnectPrecisaScale` / `ConnectAllDevices`（枚举 COM 口识别设备）
- `IsTasIOConnected` / `IsPrecisaScaleConnected`

### 7.4 `HydroDeviceProvider`（适配器）
- 实现 `IHydroDeviceProvider`，解耦状态机与 DeviceManager
- 维护 `CurrentPressure` / `CurrentWeights[]`（下标=1~4），订阅设备事件
- 暴露 `SetWaterInlet / SetFastPump / SetSlowPump / SetWaterJacketLock`

---

## 8. 水压试验状态机（核心业务）

### 8.1 上层控制器 `HydrostaticTestController`
两层结构：上层宏观流程 + 下层子任务。状态枚举见 §3（14 态）。

流程（EnterState 驱动）：
`Preparing` → `WaterJacketChecking`(LeakCheckTask) → `CylinderChecking`(CylinderCheckTask)
→ `Initializing`(记录初始压力/重量) → `PressurizingToWorking`(PressurizeTask 到工作压力)
→ `HoldingAtWorking`(HoldTask 保压+泄漏检测) → `PressurizingToTesting`(PressurizeTask 到试验压力)
→ `HoldingAtTesting`(WaitTask 保压) → `ReleasingPressure`(ReleaseTask 泄压)
→ `Stabilizing`(StabilizeTask 稳定) → `CalculatingResult`(残余变形率判定) → `Completed`/`Aborted`

### 8.2 子任务 `HydroSubTask`（基类）
- `Start(parameters)` / `Stop()` / `Reset()` / `Dispose()`
- 事件：`StatusChanged` / `RequestInstructionDialog` / `RequestMessageBoxConfirm` / `Finished(success, error, result)`
- 异步 `RunAsync()`；对话框等待 `ShowInstructionDialogAsync` / `RequestConfirmAsync`
- 子任务：`PressurizeTask`（快泵→慢泵，超时）、`HoldTask`（保压采样+泄漏）、`LeakCheckTask`、`ReleaseTask`、`WaitTask`、`StabilizeTask`、`CylinderCheckTask`

### 8.3 结果判定（`CalculateResult`）
残余变形率 = (试验压力重量 - 初始重量) / (泄压后重量 - 初始重量) ... 按 `ResidualDeformationRate` 阈值判定合格/不合格。

---

## 9. 关键数据流

1. **登录**：加载用户列表 → `ValidateLogin`（查库校验用户名+密码）→ 写 `UserSession`（Username/Company/IsAdmin/DeviceName）
2. **试验准备**：`TestPreparationPage` 填 1~4 号 `SampleInspectionData` + `TestStandard` → `SavedSamples` + 更新 `MainViewModel`
3. **水压试验**：`HydroStaticTestPage` → 控制器跑状态机 → 设备读压力/重量 → 实时曲线（`SampleGridUserControl` 双轴）+ `UpdateResults` 显示
4. **结果保存**：`TestDataCoordinator` 收集 → `UnifiedTestResultService` 生成 → `TestReportGenerator` 生成 PDF + 图表 → DAO 保存
5. **会话** `UserSession`：DeviceName, Username, Company, IsAdmin, AL_OFF（静态全局）

---

## 10. Qt6 重构映射建议

| 原（WPF/C#） | Qt6 重构 |
|------|------|
| Models（实体） | C++ 数据结构（`Q_GADGET` 或普通 struct/class）|
| Dao（LiteDB） | `QSqlDatabase` + SQLite（含数据迁移工具）|
| Devices（NModbus/SerialPort） | `QSerialPort` + libmodbus / 自研 Modbus 帧 |
| HydrostaticTestController + 子任务 | 手写 C++ 异步状态机（`enum class + switch`，不用 QStateMachine）|
| SampleGridUserControl（LiveCharts 双轴） | `QQuickPaintedItem` 子类实时曲线（复用既有组件）|
| TestReportGenerator（PDFsharp） | `QTextDocument::print` / `QPrinter` |
| LoginWindow/ViewModel | QML 登录页 + `QObject` 服务暴露 |
| UserSession（静态全局） | 全局单例（如 `UserSession` C++ 类）|
| SpeechSynthesizer | `QTextToSpeech` |

---

## 11. 报告生成与界面细节（补充）

### 11.1 报告生成 `TestReportGenerator`（`Utils/`）
- **输出目录**：`Documents/水压测试结果/`
- **文件夹命名**：`{manufacturer}_{serialNo}_{yyyyMMdd_HHmm}`
- **PDF 命名**：`{manufacturer}_{serialNo}_试验报告.pdf`
- **流程**：`SavePdfTestReport` → `CreateTestResultFolder` → `SaveChartImages`（LiveCharts 曲线截图 PNG）→ `GeneratePDFReport`（PDFsharp 手写排版，A4 竖版，嵌入图表图片）
- **PDF 元信息**：Title=水压测试报告-流水号, Author=检验员, Subject

### 11.2 外观检查 `AppearenceInspectionWindow`（`TestPreparationPage/`）
- **四部分**：External（外观）/ Internal（内部）/ Thread（螺纹）/ Valve（瓶阀）
- 每部分：检查项（`bool?`/文本）+ 结果(`InspectionResult`) + 缺陷位置 + 其他
- **检验员信息**：InspectorName, InspectorCertNo, InspectorDate
- `LoadFromSampleData`：从 `SampleInspectionData` 载入全部字段
- **确认后回写**：4 个结果 + `InspectionCompleted=true` + `InspectionDate=Now`

### 11.3 样品卡片 `SampleCardUserControl`（`TestPreparationPage/`）
- DataContext 绑定 `SampleInspectionData`
- 按钮：**外观检测**（打开外观检查窗口）、**保存**
- **保存校验**：气瓶型号/制造厂商/容积>0/使用单位/产品编号 非空 + 外观检测已完成
- 保存成功 → `SampleSaved` 事件 → `TestPreparationPage` → `MainViewModel.UpdateSampleInfo`

### 11.4 标准卡片 `StandardCardUserControl`
- 填 `TestStandard` → `TestStandardSaved` → `MainViewModel.UpdateTestStandardInfo`

### 11.5 系统维护 `SystemMaintainWindow`（`ManagementWindows/`）
- 显示系统信息（设备名/厂商/日期/序列号，来自 `config.json`）
- 连接设备：`ConnectAllDevicesAsync` → 显示水压机 + 天平组连接状态
- 手动控制：水套锁(4 个 Toggle)、进水、快泵、慢泵 Toggle（调 `TasIO`）

### 11.6 报告查看 `TestReportWindow`（`ManagementWindows/`）
- **WebView2** 加载 PDF；支持多 PDF 前后翻页（Previous/Next + 页码）

### 11.7 Qt 映射补充
| 原（WPF/C#） | Qt6 重构 |
|------|------|
| TestReportGenerator（PDFsharp + LiveCharts 截图） | `QTextDocument`/`QPrinter` + `QQuickPaintedItem` 曲线导出图片 |
| AppearenceInspectionWindow | QML 对话框（四部分检查表单）|
| SystemMaintainWindow（WMI/SerialPort） | QML + `QSerialPortInfo` |
| TestReportWindow（WebView2 显示 PDF） | `QPdfView` / QML 内嵌 PDF 查看 |
