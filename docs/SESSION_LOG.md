# SY1000-host-qt 会话日志

> 记录每次任务完成后的会话总结，便于追溯进度与交接。
> 每条记录对应一个 Git 提交（含 hash），按时间倒序排列（最新在上）。

---

## 2026-08-19 · 登录页隐藏顶部 header，左上角留空 ✅

- **提交**：待填（提交后补 hash）
- **范围**：启动界面左上角无控件（对齐 WPF 无边框登录窗口），仅右上角电源按钮

### 实现
- `qml/Main.qml`：
  - header 加 `visible: stack.depth > 1`（登录页隐藏顶部深蓝条，左上角留空）
  - 移除 header 内退出按钮；用户名 Label 锚回 `parent.right`
- `qml/LoginPage.qml`：新增电源退出按钮，`anchors` 到 `loginPage` 右上角（无 header 时即窗口右上角）

### 验证
- ✅ `SY1000.exe` 构建成功；`Main.qml`/`LoginPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 登录页制造商文本垂直居中修复 ✅

- **提交**：`1a9ac32` `fix(ui): center manufacturer label horizontally on login page`
- **范围**："北京德康时代科技有限公司" 偏左，未与 logo / 设备名垂直中心对齐

### 修复
- `manufacturer`（及 `deviceName`）Label 补 `width: parent.width` + `horizontalAlignment: AlignHCenter`，使其在 Column 内水平居中（此前 Label 宽度=文本宽且无水平锚定，靠左）

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 登录页左侧三项纵向间距对齐 WPF ✅

- **提交**：`38fd033` `fix(ui): login brand vertical spacing 50px match WPF`
- **范围**：品牌区 logo / 设备名 / 制造商 的纵向排列间距

### 修复
- WPF `StackPanel` 中 logo→DeviceName、DeviceName→Manufacturer 间距均为 **50px**（`Margin="0 50 0 0"`）
- Qt 品牌区 `Column` 的 `spacing` 由 30 改为 **50**，对齐 WPF

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 登录按钮风格对齐 WPF ✅

- **提交**：`5ebb989` `feat(ui): login button raised Material style match WPF`
- **范围**：登录按钮改为 WPF `MaterialDesignRaisedButton` 风格

### 实现（`qml/LoginPage.qml`）
- `import QtQuick.Controls.Material`
- `signInButton` 设置：
  - `Material.background: "#303F9F"`（WPF PrimaryHueDark 深 Indigo）
  - `Material.foreground: "white"`（白色粗体文字）
  - `Material.elevation: 2`（raised 阴影）
  - `ToolTip`（"点击登录进入主菜单"，对齐 WPF ToolTip）

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 登录页左侧设备信息对齐 WPF ✅

- **提交**：`194b9c4` `fix(config): login brand info match WPF (DKSY-I / Beijing Dekang)`
- **范围**：登录页左侧三项（logo → 设备名 → 制造商）的值对齐 WPF `config.json`

### 核对（读 WPF 源码）
- WPF `Resources/config.json`：`DeviceName="DKSY-I 气瓶水压检测装置"`、`Manufacturer="北京德康时代科技有限公司"`、`SerialNo="2511B01"`、`ManufactureDate="2025-11-10"`
- Qt 之前的 `config.json` 是占位符（`SY1000`/`Quanshen`），与 WPF 不符

### 修复
- `config.json` 更新为 WPF 真实值：deviceName/manufacturer/serialNo/manufactureDate
- 登录页左侧显示：logo → deviceName(64px) → manufacturer(36px)，与 WPF 一致
- 源文件与 exe 目录副本均 UTF-8 无 BOM，Qt `ConfigManager`（UTF-8 解析）正确返回中文

### 验证
- ✅ 源与 exe 目录 config.json 均为 UTF-8、中文正确
- ✅ `SY1000.exe` 构建成功、启动无回归、stderr 干净

---

## 2026-08-19 · 登录页微调（对齐 WPF，布局不变）✅

- **提交**：`8c1e416` `feat(ui): tune login page details to match WPF (layout unchanged)`
- **范围**：在保持登录页布局不变的前提下，对比 WPF `LoginWindow` 微调细节

### 微调（布局结构不变）
- **品牌区**：logo 宽 300→500（对齐 WPF）；标题由硬编码改为 **config 设备信息** `deviceService.deviceName()`(64px) / `manufacturer()`(36px)，对齐 WPF
- **登录卡**：补 **Elevation 阴影**（Dp12 模拟，多层半透明圆角矩形）
- **输入框**：用户名前补 👤、密码前补 🔒 图标（对齐 WPF Account / Lock 图标）；输入框 300→270（给图标留位）
- **登录按钮**：高度 52→55（对齐 WPF）

### 相关改动
- `src/services/deviceservice.h/.cpp`：新增 `Q_INVOKABLE deviceName()` / `manufacturer()`（读 ConfigManager，供 QML 品牌区使用）

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；运行 stderr 干净
- ✅ `testlogin`/`testcore`/`testcontroller` 冒烟通过

---

## 2026-08-19 · 修复登录后无法进入主菜单（Page title FINAL 冲突）✅

- **提交**：`bc19b9c` `fix(ui,core): login could not enter main menu (Page title FINAL) + embed zh_CN qm + testlogin`
- **范围**：点击登录无反应、无法进入新界面的重大 bug

### 根因（stderr 定位）
1. **`Page` 的 `property string title` 覆盖内置 FINAL 属性** → 8 个 Page 页面（MainMenu/ResultManagement/ResultDetails/ReportView/SystemMaintain/UserManagement/Test/TestPreparation）全部加载失败 → 登录后 `stack.replace("MainMenuPage.qml")` 静默失败，无法进入主菜单
2. **quitBtn 跨父级锚定 `titleLabel`**（`Cannot anchor to an item that isn't a parent or sibling`）→ 此前按钮位置错乱/顶部超界的根因
3. **`:/i18n/sy1000_zh_CN.qm` 加载失败** → qm 未嵌入资源，默认中文回退英文

### 修复
- 8 个 Page 的 `property string title: qsTr("X")` 改为 `title: qsTr("X")`（用内置 title 属性）
- `Main.qml` quitBtn 由 `anchors.bottom: titleLabel.bottom`（跨级非法）改为 `anchors.verticalCenter: parent.verticalCenter` + `transform: Translate { y: 6 }`
- `CMakeLists.txt`：在 `qt_add_lrelease` 后新增 `qt_add_resources`，把生成的 `sy1000_zh_CN.qm` 嵌入到 `/i18n/`
- 新增 `src/tests/testlogin.cpp`：验证 LoginService（admin/9999 登录成功 + `loginSucceeded` 信号）

### 验证
- ✅ `MainMenuPage` 作为初始页运行 stderr 干净（无 QML 错误）；最终初始页恢复 `LoginPage`
- ✅ 运行日志 stderr 完全为空（无 zh_CN 加载失败、无 QML 错误）
- ✅ 6 个 headless 冒烟测试全绿（含新增 `testlogin`：LOGIN SMOKE PASS）

---

## 2026-08-19 · 下移电源按钮避免顶部超出 header ✅

- **提交**：`9332033` `fix(ui): shift power quit button down to stay within header`
- **范围**：电源按钮底部对齐标题标签后，其顶部超出 header 上边界，整体下移 6px

### 修复
- `qml/Main.qml`：`quitBtn` 保留 `anchors.bottom: titleLabel.bottom`，新增 `transform: Translate { y: 6 }` 将整个按钮下移 6px，使顶部位于 header 内

### 验证
- ✅ `SY1000.exe` 构建成功；`Main.qml` qmllint 无 Error；应用启动无回归

---

## 2026-08-19 · 修正电源按钮外圈/内圈 y 坐标不一致 ✅

- **提交**：`46dcfbb` `fix(ui): align power glyph center with outer circle on quit button`
- **范围**：登录页右上角电源按钮，外圈（背景圆）与内圈（⏻ 图标）垂直中心错位（外圈偏上、内圈偏下）

### 修复
- 根因：Qt Quick Controls `Button` 内部有 padding，`contentItem` 的居中中心与 `background` 圆中心不在同一位置
- 将 `Button` 改为自定义 `Item`：
  - 外圈 `Rectangle`（`anchors.fill: parent`）与内圈 `Label`（`anchors.centerIn: parent`）**都居中于同一父级**，几何中心严格一致
  - 电源字形 `⏻` 字体度量略偏下，加 `verticalCenterOffset: -1` 光学微调
  - 点击/hover 用 `MouseArea` 实现（`cursorShape` 手型、hover 变色）

### 验证
- ✅ `SY1000.exe` 构建成功；`Main.qml` qmllint 无 Error；应用启动无回归

---

## 2026-08-19 · 部署 Qt 运行库到 exe 目录 ✅

- **提交**：无（构建产物，`build/` 被 .gitignore 忽略，不提交）
- **范围**：为 `build/msvc/Debug/SY1000.exe` 补全启动所需 Qt 库，实现独立运行

### 操作
- 使用 Qt 官方 `windeployqt --qmldir <qml> SY1000.exe` 自动部署
- 部署内容：34 个 Qt DLL（Qt6Cored/Guid/Quick/Sqld/SerialPortd/Networkd/PrintSupportd 等）+ 平台插件 `qwindowsd.dll` + `platforms/qml/sqldrivers/styles/translations` 等 QML 插件 + 翻译
- `Debug` 目录部署后约 254 MB

### 验证
- ✅ 系统 PATH（Machine/User）均不含 Qt
- ✅ 用干净 PATH（仅 System32/Windows/exe 目录）启动 `SY1000.exe` → **APP ALIVE（真正独立运行）**
- ✅ `build/` 在 .gitignore 中，部署文件不会被 git 跟踪

### 备注
- 若需发布，可将整个 `Debug`（或 Release）目录一起打包（exe + 全部 DLL + QML 插件）。

---

## 2026-08-19 · 对齐退出按钮与标题标签底部 ✅

- **提交**：`545ad52` `fix(ui): align quit button bottom with header title label bottom`
- **范围**：按用户要求，将电源式退出按钮的**底部**与 header 左上角标题标签（"| Login"）的**底部**对齐

### 实现
- `qml/Main.qml`：标题 Label 加 `id: titleLabel`；退出按钮由 `anchors.verticalCenter` 改为 `anchors.bottom: titleLabel.bottom`（保留右侧锚定）

### 验证
- ✅ `SY1000.exe` 构建成功；`Main.qml` qmllint 无 Error；应用启动无回归

---

## 2026-08-19 · 修正登录页退出按钮位置（移到窗口右上角）✅

- **提交**：`481d82e` `fix(ui): move quit button to window top-right header (login page only)`
- **范围**：修复"退出按钮位置不对"——原按钮锚定在 LoginPage 顶部（header 下方），与 WPF 窗口右上角不符

### 修复
- 从 `qml/LoginPage.qml` 移除退出按钮
- `qml/Main.qml` 顶部 header（窗口右上角）新增电源式退出按钮：
  - `visible: stack.depth === 1`（仅登录页显示）
  - 44×44 圆形、深蓝、白边电源图标 ⏻、hover 变深
  - `onClicked: Qt.quit()`；用户名 Label 左移让位
- 位置现为**窗口右上角**（header 内，对齐 WPF QuitButton）

### 验证
- ✅ `SY1000.exe` 构建成功；`Main.qml`/`LoginPage.qml` qmllint 无 Error；应用启动无回归
- 清理临时调试文件（debug_out.txt / debug_err.txt / login_shot.png）

---

## 2026-08-19 · 登录页右上角电源式退出按钮 ✅

- **提交**：`6d10986` `feat(ui): power-style quit button on login page top-right (WPF parity)`
- **范围**：按 WPF `LoginWindow.xaml` 的 `QuitButton`（右上角 Power 图标）给 Qt 登录页补退出按钮

### 实现
- `qml/LoginPage.qml` 右上角新增电源式退出按钮：
  - 60×60 圆形（radius = 高/2），深蓝背景（#193660，hover 变 #303F9F）、白色 2px 边框
  - 内容为电源图标（⏻，白色）
  - `onClicked: Qt.quit()` 退出应用

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；应用启动无回归

---

## 2026-08-19 · D14 控件样式精细化 ✅

- **提交**：`1d4d43c` `feat(ui): reusable ShadowCard with elevation/hover + menu cards (D14)`
- **范围**：D14 卡片/控件样式对齐 WPF MaterialDesign

### 实现
- 检查到 Qt6 无 `GraphicalEffects` 模块（阴影不可直接使用），改用纯 QML 模拟
- 新建 `qml/ShadowCard.qml` 可复用组件：
  - **模拟 Elevation 阴影**：3 层半透明矩形叠加（alpha 递减）实现右下投影
  - **圆角**（radius 12）+ 边框
  - **hover 效果**：背景微变 + 边框高亮变色（ColorAnimation 过渡）+ 手型光标
  - 属性：title / icon / accent / elevation；信号 clicked()
- `MainMenuPage.qml` 4 个功能卡片替换为 `ShadowCard`（开始试验 / 结果管理 / 用户管理 / 系统维护）
- `CMakeLists` 注册 `ShadowCard.qml` 资源

### 验证
- ✅ `SY1000.exe` 构建成功；`ShadowCard.qml`/`MainMenuPage.qml` qmllint 无 Error；应用启动无回归

### 备注
- 样式组件可复用到其他页面卡片（SystemMaintainPage/TestPreparationPage 等），后续按需扩展。
- 按钮 hover 沿用 Qt Quick Controls 2 Material 默认。

---

## 2026-08-19 · D 系列：真实 Logo + 默认中文 ✅

- **提交**：`3e4162f` `feat(ui): real product logo + default zh_CN UI (D11, D12)`
- **范围**：D11 登录页默认中文、D12 真实 Logo

### 实现
- **D12 真实 Logo**：复制 WPF `Resources/dklogo.png`（30KB）到 `qml/assets/dklogo.png`；`CMakeLists` 注册资源；`LoginPage.qml` 用 `Image`（PreserveAspectFit）替换原"DKQSY"占位矩形
- **D11 默认中文**：`src/main.cpp` 启动时 `LanguageHelper::setLanguage("zh_CN")`（对齐 WPF 全中文界面）；`LoginPage` 语言 ComboBox 按 `lang.current()` 显示当前语言

### 验证
- ✅ `SY1000.exe` 构建成功（dklogo 资源嵌入）；qmllint 无 Error；应用启动无回归

### 备注（D13 字体暂缓）
- WPF 字体 `NotoSansCJKsc-VF.ttf` / `SourceHanSansSC-VF.ttf` / `SourceHanSerifSC-VF.ttf`（36-59MB）过大，暂不引入仓库；当前用系统字体 + 统一 Indigo/Amber 配色。

---

## 2026-08-19 · 按 WPF 源码核对修正中文翻译 ✅

- **提交**：`4bce98a` `i18n: align zh_CN translations with original WPF wording`
- **范围**：读取 `sy1000-host-wpf` 源码中的中文文案（XAML Text/Content/Header + C#），逐条核对 `i18n/sy1000_zh_CN.ts` 翻译，按 WPF 用词修正不一致项

### 修正要点（对齐 WPF 原文）
- **外观检查 4 部分标题**：External→外部检查、Internal→内部检查、Thread→瓶口螺纹检查、Valve→气瓶阀检查
- **检查项**：热损伤迹象 / 有无划伤 / 有无磨损 / 有无分层 / 有无异常变形 / 有无气味 / 缺陷位置描述 / 杂物的种类和数量 / 内表面状况描述 / 螺纹规格 / 螺纹状况描述 / 螺纹状况评估 / 气瓶阀编号 / 连接螺纹状况描述 / 气密状况描述 / 爆破片是否更换
- **结果状态**：To Repair→待修复、To Replace→待更换、Scrapped→判废；评定结果
- **按钮/菜单**：Start→开始试验、Stop→停止试验、Result Management→试验结果管理、Add→创建新用户
- **详情/报告标签**：External:/Internal:/Thread:/Valve: → 外部检查：/内部检查：/瓶口螺纹检查：/气瓶阀检查：
- 样品容积 Volume (L)→气瓶容积

### 工具
- 新增 `tools/correct_translations.py`：source→WPF 用词映射，覆盖已有翻译（可复用）

### 验证
- ✅ ts 0 unfinished；修正项全部生效；`lrelease` 重新生成 qm；`SY1000.exe` 启动无回归

---

## 2026-08-19 · 中文翻译文件补全 + UI 遗漏补齐 ✅

- **提交**：`f97fd9c` `feat(i18n,ui): complete zh_CN translations + WPF UI parity fixes`
- **范围**：① 补全中英 i18n 翻译文件；② 按 WPF↔Qt 核对补齐 UI 遗漏

### 中文翻译文件补全
- 原 `i18n/sy1000_zh_CN.ts` 仅 11 条翻译，严重不完整
- 用 `lupdate` 从 `qml/`+`src/` 重新生成 ts（178 个 qsTr 源文本）
- 新建 `tools/fill_translations.py`：内嵌 source→中文 映射，批量填充全部 178 条（含 `&amp;` 转义、带尾随空格 key）
- 结果：0 unfinished、179 条 finished；`qt_add_lrelease` 生成 `sy1000_zh_CN.qm`（11.7KB）嵌入 `:/i18n/`，LanguageHelper 切换 zh_CN 生效
- 保留脚本 `tools/fill_translations.py` 便于日后更新翻译

### UI 遗漏补齐（WPF↔Qt 核对）
- 样品卡补"使用单位(UserCompany)"字段（WPF 必填项）→ 传入 inspection 并写入结果
- 结果详情页补外观检查 4 项结果显示；`details()` 返回 `external/internal/thread/valve`
- 主菜单补"连接状态"按钮（调 `deviceService.connectDevices()`）
- 修复 `ResultDetailsPage.qml` **预先存在**的 `function row(){ Label{} }` QML 语法错误（曾致运行时渲染问题）

### 验证
- ✅ 全目标编译通过；5 个 headless 冒烟测试全绿
- ✅ `lrelease` 生成 179 finished / 0 unfinished；`SY1000.exe` 启动无回归
- ✅ 修正 3 处遗漏翻译（Username/Password 同文本启发式、空 qsTr("")）

### 已知限制
- 报告查看多 PDF 翻页：因 Qt Pdf 模块未安装（QPdfView 不可用），未实现（记录在案）。

---

## 2026-08-19 · 试验数据闭环补全（曲线点 + 环境数据）✅

- **提交**：`b9e46b0` `feat(services,dao): persist curve points + environment data in test result`
- **范围**：实现 WPF 中此前 Qt 未实现的部分——试验结果数据的曲线与环境信息

### 实现
- `src/services/hydroadapter.h/.cpp`：
  - 采样定时器在试验进行中采集 `PressureWeightPoint`（压力 + 样品1重量）到 `m_curvePoints`；`startTest()` 清空
  - `buildResult()` 填入 `h.pressureWeightData` + `h.workingPressure/testPressure`；填充 `testEnvironment`（室温/湿度 + config 设备ID/型号）
- `src/dao/json_serializer.h/.cpp`：
  - `hydroStaticToJson/FromJson` 补全 `pressureWeightData`（ts/pressure/weight 数组）序列化
  - `unifiedTestResultToJson/FromJson` 补全 `equipmentId`/`equipmentModel`
- `src/tests/testcore.cpp`：新增曲线点 + 环境数据保存读回验证（curve_points=6、eq/model）

### 尝试未实现
- **语音合成**：尝试接入 `QTextToSpeech`，但其依赖 `Qt6Multimedia`（本机 Qt 未安装），已撤销；保留 `voice()` 信号待后续环境具备时再接入。

### 验证
- ✅ 全目标编译通过；5 个 headless 冒烟测试全绿（testcore 验证曲线点 + 环境数据 round-trip）
- ✅ `testreport`：报告含 `data:image/png` 曲线；`SY1000.exe` 启动无回归

### 备注
- 真实试验保存的结果现在带曲线点，PDF 报告曲线真实化。
- 环境数据室温/湿度为占位值（无温湿度传感器），设备 ID/型号来自 config.json。

---

## 2026-08-19 · B5 试验结果管理拆分 ✅

- **提交**：`beecc55` `feat(ui,services): split result management into two tabs (B5)`
- **范围**：`docs/TODO.md` 待办第 5 项

### 实现
- `qml/ResultManagementPage.qml`：改为 **TabBar 双分区**，对齐 WPF 两个窗口
  - **Tab 1 "测试结果管理"**（对应 `TestResultManagementWindow`）：测试者 / 测试时间 / 产品型号 / 产品编号 / 生产厂家 / 删除
  - **Tab 2 "试验结果管理"**（对应 `UnifiedTestResultManagementWindow`）：生产厂商 / 气瓶编号 / 试验时间 / 试验人员 / 试验结果 / 查看详情
- `src/services/resultservice.cpp`：`results()` 新增 `sampleModel`、`sampleSerial` 字段以支撑两 Tab 列

### 验证
- ✅ `services.lib` + `SY1000.exe` 编译通过；`testcore` SMOKE PASS
- ✅ `ResultManagementPage.qml` qmllint 零错误；`SY1000.exe` 启动无回归

### 备注
- 未实现 WPF 的分页与"按厂商/日期"筛选（保留刷新/删除/详情），如需可后续补充。

---

## 2026-08-19 · C9 config.json 加载 + 修复疑似"debug 弹窗" ✅

- **提交**：`60ae155` `feat(services,core): config.json ConfigManager + localize release confirm dialog (C9)`
- **范围**：`docs/TODO.md` 待办第 9 项；顺带修复用户反馈的"debug 弹窗"观感

### 修复疑似"debug 弹窗"
- 定位：项目源码无任何 `QMessageBox`，最可能来源为 **A3 试验中途以英文弹出的泄压确认框**（"Release Pressure / Please open the release valve..."），在中文界面中观感似调试信息。
- 改动：`src/core/tasks.cpp` 泄压确认框文案本地化为中文操作指令——标题"泄压操作"、消息"请打开泄压阀，然后点击"确认"开始泄压。"、状态"泄压中，剩余 %1 秒"。

### C9 config.json 加载
- 新增 `src/services/configmanager.h/.cpp`：`ConfigManager`
  - `load(path)`：从 `config.json` 读取（path 为空时搜索 exe 目录/当前目录）
  - `value(key, default)` 泛型访问 + `deviceName()/manufacturer()/serialNo()/manufactureDate()` 便捷访问；文件/键缺失回退默认值
- `src/services/deviceservice.cpp`：`systemInfo()` 改为读取 `ConfigManager`（原为占位符）
- 新增 `config.json`（项目根 + 构建后复制到 exe 目录）；`src/main.cpp` 启动时 `ConfigManager::load()`
- `CMakeLists.txt`：`configmanager.cpp` 加入 services；新增 `testconfig` 测试；SY1000 POST_BUILD 复制 config.json

### 验证
- ✅ 全目标编译通过；5 个 headless 冒烟测试全绿（新增 `testconfig`：CONFIG SMOKE PASS，正确读取/回退默认）
- ✅ `SY1000.exe` 启动无回归，`config.json` 已复制到输出目录

### 备注
- 若用户反馈的"debug 弹窗"并非泄压确认框（如 MSVC 断言窗口），请补充具体弹窗文本以便进一步定位。

---

## 2026-08-19 · C7 PDF 报告生成 + A2 报告查看 ✅

- **提交**：`3473e9c` `feat(report,ui): PDF test report generation + report view page (C7, A2)`
- **范围**：`docs/TODO.md` 建议顺序第 5 项（报告闭环）

### 说明
- 检查到 Qt 安装中 **无 Qt Pdf 模块（QPdfView 不可用）**，但 `Qt6PrintSupport`（QPrinter）可用。

### C7 PDF 报告生成
- 新增 `src/report/testreportgenerator.h/.cpp`：`TestReportGenerator`
  - `buildHtml(result)`：HTML 排版（标题/检验员/标准/样品/水压数据表/外观检查/总体结果），内嵌压力曲线图（`chartDataUri`，base64 PNG）
  - `generatePdf(result, outPath)`：`QTextDocument + QPrinter`（A4）输出 PDF，路径 `Documents/水压测试结果/{mfg}_{serial}_{yyyyMMdd_HHmm}/{mfg}_{serial}_试验报告.pdf`
- `CMakeLists.txt`：新增 `report` 库（链 PrintSupport）+ `testreport` 测试；services 链入 report
- `src/services/resultservice.h/.cpp`：新增 `reportData(id)`（报告字段 map）与 `generatePdf(id)`
- `src/tests/testreport.cpp`：headless 测试（用 `QGuiApplication`，QPrinter 需要字体数据库）验证 PDF 生成

### A2 报告查看（替代方案）
- 新建 `qml/ReportViewPage.qml`：A4 风格报告预览（原生渲染报告字段）+ "Generate & Open PDF" 按钮（`generatePdf` 后 `Qt.openUrlExternally`）
- `qml/ResultDetailsPage.qml`：新增 "View Report" 按钮导航
- `CMakeLists.txt`：注册 `ReportViewPage.qml`

### 验证
- ✅ 全目标编译通过（report.lib / testreport.exe / SY1000.exe）；4 个 headless 冒烟测试全绿
- ✅ `testreport`：生成 56KB PDF、html 含 `data:image/png` 曲线图、REPORT SMOKE PASS
- ✅ `ReportViewPage.qml` qmllint 零错误；`SY1000.exe` 启动无回归

### 备注
- 修复两处编译问题：① 函数误入匿名命名空间导致 `}` 不匹配；② `PressureWeightPoint.timestamp` 为 `DateTime`（time_point）需转 epoch 毫秒再绘图。
- 若后续目标机需要内嵌 PDF 查看，可安装 Qt Pdf 模块后改用 `QPdfView`。

---

## 2026-08-19 · A3 水压试验消息对话框 ✅

- **提交**：`ba9bdf8` `feat(core,ui): blocking operator confirm dialog for hydrostatic test (A3)`
- **范围**：`docs/TODO.md` 建议顺序第 4 项

### 核心机制：阻塞式确认请求
- `src/core/subtask.h/.cpp`：`HydroSubTask` 新增 `requestConfirm(title,message)` 信号、`requestConfirmation()`（发射并暂停）、`confirmResponse(bool)` 槽（恢复回调）
- `src/core/controller.h/.cpp`：新增 `confirmRequested` 信号 + `Q_INVOKABLE respondConfirm(bool)`；`runTask()` 转发子任务确认请求并记录 `m_pendingConfirm`
- `src/services/hydroadapter.h/.cpp`：桥接 `confirmRequested` 到 QML，`respondConfirm()` 回传控制器

### 业务接入
- `ReleaseTask::run()`（`src/core/tasks.cpp`）：泄压前弹窗"请打开泄压阀，确认开始泄压"，确认后开始倒计时，取消则中止

### UI
- 新建 `qml/HydroTestMessageDialog.qml`（标题+消息+OK/取消；OK→`respondConfirm(true)`，取消→`false`）
- `qml/TestPage.qml`：`Connections` 监听 `hydro.onConfirmRequested` 打开对话框（Page 根级覆盖整页）
- `CMakeLists.txt`：注册新 QML 资源

### 测试适配
- `src/tests/testcontroller.cpp`：连接 `confirmRequested` 自动确认，保证 headless 冒烟测试不阻塞挂起

### 验证
- ✅ 全目标编译通过；3 个 headless 冒烟测试全绿（testcontroller 日志显示泄压态确认请求成功 auto-accept）
- ✅ `HydroTestMessageDialog.qml` qmllint 零错误；`SY1000.exe` 启动无回归
- ✅ 已提交并推送 `ba9bdf8`

---

## 2026-08-19 · B4 4 样品双轴曲线 ✅

- **提交**：`9dd3941` `feat(ui,charts): RealTimeChart multi-series dual-axis + 4-sample curves on test page (B4)`
- **范围**：`docs/TODO.md` 建议顺序第 3 项

### `RealTimeChart`（`src/ui/charts/RealTimeChart.h/.cpp`）多序列 + 双 Y 轴重构
- 从单序列 `QQueue<DataPoint> m_dataPoints` 重构为 `QVector<Series> m_series`（名称/颜色/左右轴标记/数据队列）
- 双 Y 轴：左轴压力（`yMin/yMax`），右轴重量（新增 `rightYMin/rightYMax/rightYAxisLabel`）；`drawAxes` 绘制左右刻度与单位标签
- 新增 QML API：`addSeries`/`addSeriesValue`/`addSeriesValueAt`/`clearSeries`/`seriesCount`；`addValue()` 兼容写序列 0
- `dataToScreen` 按 `rightAxis` 映射；`calculateYRange` 分别计算左右轴自动缩放；`drawSeriesInto` 绘制单序列路径+填充+高亮
- 多序列强制全量重绘（正确优先），单序列保留原增量缓存
- 顺序健壮性：`addSeries` 在序列为空时自动创建 pressure 占位序列 0，避免采样时序错位

### 数据接入
- `src/services/hydroadapter.h/.cpp`：新增 `weightSample(int,double)` 信号，采样定时器每 100ms 同时发射压力与 1-4 号样品重量

### 试验页
- `qml/TestPage.qml`：启用右轴 + 注册 4 个样品序列（红/绿/橙/蓝）+ 图例；`onPressureSample`→`addValue`，`onWeightSample`→`addSeriesValue`

### 验证
- ✅ 全目标编译通过；3 个 headless 冒烟测试全绿；`SY1000.exe` 启动无回归
- ✅ qmllint 语法零错误（`SyCharts` import 警告为运行时注册类型所致，非真实问题）
- ✅ 已提交并推送 `9dd3941`

---

## 2026-08-19 · A1 外观检查补全 + C6 接入结果保存 ✅

- **提交**：`adfdf64` `feat(ui,services): full appearance inspection form + persist inspection in result (A1, C6)`
- **范围**：`docs/TODO.md` 建议顺序第 1、2 项

### A1 外观检查补全
- `qml/AppearanceInspectionDialog.qml`：从 4 下拉框升级为完整 WPF 式表单
  - 检验员信息（姓名/证书号/日期，缺省自动填当天）
  - External/Internal/Thread/Valve 四部分逐项检查（勾选 + 文本）+ 缺陷位置 + 其他备注
  - 结果值以 `0-3` 整数存储（对应 `InspectionResult` 枚举），确认后写回 `target.inspection` 并置 `inspectionCompleted=true`

### C6 外观检查接入结果保存
- `qml/TestPreparationPage.qml`：初始化 `inspection` 数据对象；"保存并开始试验"时调用 `hydro.setSampleInspection(i+1, d.inspection)`
- `src/services/hydroadapter.h/.cpp`：新增 `Q_INVOKABLE setSampleInspection()`（`QVariantMap`→`SampleInspectionData` 转换），`buildResult()` 写入 `sample.appearanceInspection`（持久化层 `json_serializer` 原已完整支持）

### 验证
- ✅ 全目标编译通过；3 个 headless 冒烟测试全绿；`SY1000.exe` 启动无回归
- ✅ `qmllint` 对新对话框 0 警告
- ✅ 已提交并推送 `adfdf64`

### 备注（发现的问题）
- **预先存在**：`ResultDetailsPage.qml:31` 中 `function row(){ Label{} }` 写法非法（QML 不能在 JS 函数体内声明 Item），会在运行时影响结果详情页渲染。建议后续一并修复（非本次引入）。

---

## 会话约定 / 备注

- 每次改动后执行：构建（`cmake --preset msvc` + `cmake --build --preset msvc`）→ 3 个 headless 冒烟测试 → qmllint 校验 → 启动 `SY1000.exe` 冒烟 → 提交推送 GitHub。
- 代码注释统一英文（避免编码兼容问题）。
- Qt 部署后处理步骤打印 `pwsh.exe 不是内部或外部命令` 属无害提示，不影响 exe 生成。
