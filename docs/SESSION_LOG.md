# SY1000-host-qt 会话日志

> 记录每次任务完成后的会话总结，便于追溯进度与交接。
> 每条记录对应一个 Git 提交（含 hash），按时间倒序排列（最新在上）。

---

## 2026-08-19 · 外观检查改为子页面（用全局标题栏）✅

- **提交**：`b8390b8` `feat(ui): inspection page is a sub-page using global header`
- **范围**：`qml/AppearanceInspectionPage.qml`
- **实现**：去掉 `hideGlobalHeader` 和自绘标题栏，改用 Main.qml 全局标题栏（显示"复合气瓶外观检查评估表"），页面作为子页面在标题栏下方显示内容，不再覆盖整个窗口
- **验证**：✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 修复 FieldRow 字段横线宽度为 0 ✅

- **提交**：`4caf04b` `fix(ui): FieldRow width cycle made field/line width 0`
- **范围**：`qml/FieldRow.qml`
- **根因**：FieldRow（Row）未设 `width`，内部 `TextField width: root.width - 168` 与 Row 宽度形成循环依赖 → 输入框/横线宽度为 0，外观检查"缺陷位置描述:"/"其他:"等字段横线不显示
- **修复**：FieldRow 加 `width: parent.width`
- **验证**：✅ qmlscene 测得 `fieldWidth=632`（此前为 0）；构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 修复"外观检测评估"按钮无反应（无效属性）✅

- **提交**：`a15c069` `fix(ui): remove invalid Material.underlineColor (caused no-reaction)`
- **范围**：`FieldRow / InspectionInternal / InspectionThread / InspectionValve.qml`
- **根因**：`Material.underlineColor` 不是有效属性，导致 `FieldRow` 组件编译失败 → `InspectionExternal` 失败 → 页面 `push` 加载失败 → 点击按钮无反应
- **修复**：删除所有 `Material.underlineColor`，TextField 用透明背景 + Rectangle 画下划线横线
- **验证**：✅ qmlscene 页面加载无组件错误；构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 外观检查页全面核对 WPF，螺纹/阀并排 ✅

- **提交**：`2c41a23` `fix(ui): thread/valve fields side-by-side per WPF`
- **范围**：`qml/InspectionThread.qml` / `InspectionValve.qml`
- **实现**：对照 WPF 完整源码——螺纹"螺纹规格:+输入框 / 螺纹状况描述:+输入框"并排一行；阀"气瓶阀编号:+输入框 / 连接螺纹状况描述:+输入框"并排一行；其余（评估/气密/爆破片/其他/评定）不变
- **验证**：✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 内部检查第一行对齐 WPF ✅

- **提交**：`5fc3dd7` `fix(ui): internal inspection row1 = smell checkbox + debris line (WPF)`
- **范围**：`qml/InspectionInternal.qml`
- **实现**：第一行改为 WPF 布局——`有无气味` CheckBox + `杂物的种类和数量:` 标签 + 长横线输入框（同一行）；其后为内表面状况/缺陷位置/其他（FieldRow）+ 评定结果 3 单选框
- **验证**：✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 检查字段行改为"标签+长下划线"组件 ✅

- **提交**：`f3cddd9` `feat(ui): FieldRow label+long-underline component for inspection fields`
- **范围**：新建 `FieldRow.qml`（标签 + 一条长长的下划线横线，底部对齐），四个检查组件字段改用该组件
- **说明**：修复 TextField 透明背景后下划线横线不显示的问题，外部/内部/螺纹/阀的"缺陷位置描述:"/"其他:"等标签行都有长横线
- **验证**：✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 外观检查四个分组独立组件化 ✅

- **提交**：`9602941` `refactor(ui): inspection groups as four independent components`
- **范围**：新建 4 个检查分组组件，页面只负责加载

### 实现
- 新建 `InspectionExternal / InspectionInternal / InspectionThread / InspectionValve.qml`：每个是独立分组组件（深蓝标题条 + 字段"标签:下划线输入框" + 评定 3 单选框），各自封装 `load/save/reset`
- `AppearanceInspectionPage.qml` 只加载 4 个组件（传 `inspection` 对象），保存/重置调各组件方法
- CMake 资源加入 4 个组件

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 外观检查四组布局统一 ✅

- **提交**：`b48f6de` `feat(ui): unify layout of all four inspection groups`
- **范围**：`qml/AppearanceInspectionPage.qml` 内部/螺纹/阀三组
- **实现**：三组改为与外部检查一致——字段全部用"标签:下划线输入框"竖排（标签 160、输入框填满、底部对齐），复选框单独一行，评定结果 3 单选框；仅标题文字不同
- **验证**：✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 外观检查改为页面（非对话框）✅

- **提交**：`5f30b8c` `feat(ui): appearance inspection as a page, not a dialog`
- **范围**：`AppearanceInspectionDialog.qml` → 新建 `AppearanceInspectionPage.qml`

### 实现
- "外观检测评估"由对话框改为**页面** `AppearanceInspectionPage.qml`，点击跳转（`stack.push`，传 sampleIndex/target）
- 页面有深蓝标题栏，标题"复合气瓶外观检查评估表"
- 四组（外部/内部/螺纹/阀）卡片：深蓝标题条 + 字段"标签:下划线输入框"底部对齐 + 评定结果 3 单选框（合格/待修复/判废）；外部 5 复选框水平均匀分布
- 底部"保存并关闭/重置"
- 删除旧 `AppearanceInspectionDialog.qml`，CMake 资源改为新页面

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 外观检查对话框重新布局（修复混乱）✅

- **提交**：`be8f282` `feat(ui): redesign inspection dialog layout (wider, tidy rows)`
- **范围**：`qml/AppearanceInspectionDialog.qml`（"复合气瓶外观检查评估表"）
- **实现**：重写布局解决混乱——对话框加宽 860、加大间距、字段"标签:输入框"一行对齐（标签 160 + 输入框填满）、每组评定 3 个单选框（合格/待修复/判废）、底部"保存并关闭/重置"
- **验证**：✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 外观检查对话框完整对齐 WPF 布局 ✅

- **提交**：`bbc9c60` `feat(ui): inspection dialog layout fully matches WPF`
- **范围**：`qml/AppearanceInspectionDialog.qml`（"外观检测评估"按钮加载的对话框）

### 实现（对照 WPF `AppearenceInspectionWindow.xaml`）
- 去掉"检查员信息"区块（WPF 无此区块）
- 四组（外部/内部/螺纹/阀）字段改为 WPF 式布局："标签:输入框"一行式、CheckBox 并排、螺纹规格/状况并排、瓶阀编号/连接螺纹并排
- 每组评定结果 3 个 RadioButton（合格/待修复/判废），加载/保存/重置逻辑同步（去掉检查员，设检查日期）

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 外观检查对话框评定结果修正为 3 个单选框 ✅

- **提交**：`d4a11c1` `fix(ui): inspection radios = 3 (Qualified/Repair/Scrapped) match WPF`
- **范围**：`qml/AppearanceInspectionDialog.qml` 四组评定结果
- **实现**：对照 WPF，每组评定结果由 4 个改为 **3 个 RadioButton（合格/待修复/判废）**，与 WPF 一致；加载/保存/重置逻辑同步
- **验证**：✅ 构建成功；qmllint 无 Error；exe 已确认包含新版 QML（Save and Close / Appearance Inspection）；启动无回归、stderr 干净

---

## 2026-08-19 · 外观检查对话框对齐 WPF ✅

- **提交**：`a808a0d` `feat(ui): inspection dialog radios + save/reset buttons match WPF`
- **范围**：`qml/AppearanceInspectionDialog.qml`（"外观检测评估"按钮加载的对话框）

### 实现（对照 WPF `AppearenceInspectionWindow.xaml`）
- 标题改为"复合气瓶外观检查评估表"
- 四个分组（外部/内部/螺纹/阀）评定结果由 `ComboBox` 改为 **`RadioButton`（合格/待修复/待更换/判废）**
- 底部按钮改为自定义 **"保存并关闭"（Accept）+ "重置"（Reset，清空全部字段）**

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 准备页右侧 5 卡高度撑满到底部 ✅

- **提交**：`5eb664e` `feat(ui): prep cards fill height to window bottom`
- **范围**：`qml/TestPreparationPage.qml` 右侧 5 张卡（标准卡 + 4 样品卡）
- **实现**：卡高由固定 `720` 改为 `rightFlick.height`（右侧区域高度），与父窗口底部对齐
- **验证**：✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 修复卡片黄色标题栏文字不显示 ✅

- **提交**：`f548cb5` `fix(ui): amber card title text via parent chain`
- **范围**：`qml/TestPreparationPage.qml` 的 `TitleBar` 组件
- **实现**：文字 Label 由 `text: TitleBar.text` 改为 `text: parent.parent.text`（内联组件中经组件名引用自身属性在 Qt 6 解析不到实例值，导致只剩 ☰ 横杆、无文字）
- **验证**：✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 标准卡/样品卡黄色标题栏显示字段名（确认方案）✅

- **提交**：`24e96d8` `feat(ui): amber card titles show field names (confirmed)`
- **范围**：`qml/TestPreparationPage.qml` 右侧 5 卡黄色标题栏
- **实现**：黄色标题栏显示 `试验标准`（标准卡）/ `气瓶型号 N`（样品卡）；字段区第一个输入框不再单独显示标签
- **验证**：✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 回退 5 卡标题栏改动，恢复第一个字段标签 + 修复登录 ✅

- **提交**：`2533201` `revert(ui): restore prep card first-field labels; fix login username prop`
- **范围**：撤销 `f53f31b` 的 5 卡标题栏改动，恢复字段区第一个控件标签

### 实现
- **恢复** 标准卡第一个字段"试验标准"标签、样品卡第一个字段"气瓶型号"标签；标题栏回到"输入试验标准"/"输入 N 号气瓶信息"
- **修复**：`LoginPage.qml` 登录欢迎文字 `loginService.username()` → `loginService.username`（Q_PROPERTY 同名，调用报 TypeError）

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 准备页 5 卡黄色标题栏内容调整 ✅

- **提交**：`f53f31b` `feat(ui): prep card amber titles show Test Standard / Cylinder Model`
- **范围**：`qml/TestPreparationPage.qml` 右侧 5 卡的黄色标题栏

### 实现
- 标准卡标题栏：`输入试验标准` → **`试验标准`**；字段区去掉重复的"试验标准"标签（保留输入框）
- 样品卡标题栏：`输入 N 号气瓶信息` → **`气瓶型号 N`**；字段区去掉重复的"气瓶型号"标签（保留输入框）

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 水压试验页顶栏独立 + 高度统一 ✅

- **提交**：`69ea206` `feat(ui): hydro pages own top bar, hide global header, height 80`
- **范围**：水压试验页（准备页 + 执行页）顶栏独立，不混入主菜单 header 内容；所有页面标题栏高度一致（80）

### 实现
- `TestPreparationPage.qml` / `TestPage.qml`：加 `hideGlobalHeader: true`，隐藏 Main.qml 全局 header（设备名/用户/退出等主菜单内容）
- `Main.qml`：header 条件改为 `!isLoginPage && !hideGlobalHeader`
- 两页自绘顶栏 80 高（== Main.qml header 高度），内容左/中/右：
  - 左：水压试验 + **连接状态显示框**（`deviceService.connectDevices()`，可点击刷新）
  - 中：**测试状态显示框**
  - 右：气瓶压力标签 + 气瓶压力值显示框

### 验证
- ✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 水压试验页顶栏/左右布局修复 ✅

- **提交**：`4d9cac2` `fix(ui): prep page top bar center/right split + wrap in column`
- **范围**：`qml/TestPreparationPage.qml` 顶栏与主体重叠、左右高度不一致、顶栏内容靠左显宽

### 修复
- 用 `Column` 包裹顶栏 + 主体 `Row`，消除重叠，左右高度一致
- 顶栏改为 **左/中/右三区分布**（对齐 WPF MainTestWindow）：左=标题，中=测试状态，右=气瓶压力+值

### 验证
- ✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 测试准备页补 MainTestWindow 布局（修正）✅

- **提交**：`726b535` `feat(ui): prep page gets MainTestWindow top bar + left column`
- **范围**：用户点"开始水压试验"进入的是准备页 `TestPreparationPage.qml`，上一版误把布局加到了执行页 `TestPage`；本版把 MainTestWindow 布局补到准备页

### 实现（`qml/TestPreparationPage.qml`）
- **顶栏**：测试状态显示框 + "气瓶压力"标签 + 压力值编辑框
- **左侧列**（WPF MainTestWindow 左列）：
  - 上半：使用标准 + 1..4 号气瓶摘要（确 定保存后刷新）+ "开始水压试验"按钮
  - 下半："保存试验结果 / 查看试验报告（禁用）/ 返回主菜单"
- **右侧**：原有 5 张准备卡（标准卡 + 4 样品卡），水平可滚动
- 顶部按钮/对话框移入页面级

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 水压试验页布局对齐 WPF MainTestWindow ✅

- **提交**：`421c3ad` `feat(ui): hydrostatic test page matches WPF MainTestWindow`
- **范围**：`qml/TestPage.qml` 重写 + hydroadapter 增加标准/样品信息接口

### 实现
- **顶栏**（WPF ColorZone）：测试状态显示框（hydro.status/state）+ "气瓶压力"标签 + 压力值编辑框（currentPressure + MPa）
- **左侧列**（WPF MainTestWindow 左列）：
  - 上半：使用标准（hydro.testStandardInfo()）+ 1..4 号气瓶（hydro.sampleInfo(i)）+ "开始水压试验"按钮
  - 下半："保存试验结果 / 查看试验报告 / 返回主菜单"
- 右侧：实时双轴曲线 + Start/Stop/Clear
- `hydroadapter`：新增 `setTestStandard/testStandardInfo/sampleInfo`；准备页确 定时保存标准名/保压/变形率

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 测试准备页控件/布局对齐 WPF ✅

- **提交**：`89c65e3` `feat(ui): test prep page matches WPF card layout/controls`
- **范围**：`qml/TestPreparationPage.qml` 重写，与 WPF TestPreparationPage 控件内容和布局一致

### 实现
- 一行 5 张卡（WPF Grid 5 列）：标准卡 + 4 张样品卡
- 每卡顶部 Amber 标题栏（ColorZone SecondaryMid + ☰ hamburger + 标题）
- **标准卡**（StandardCardUserControl）：试验标准(GB/T9251-2022)、公称工作压力(30 MPa)、试验压力(45 MPa)、保压时间(30 秒)、允许容积残余变形率(5%) + "确 定"
- **样品卡**（SampleCardUserControl）：气瓶型号、制造厂商、气瓶容积(L)、使用单位、产品编号 + "外观检测评估"/"确 定"
- 底部"开始试验"（转入 TestPage）/ "返回"
- 样品"确 定"校验对齐 WPF ValidateSampleData；标准卡确 定写入压力
- 复用 `AppearanceInspectionDialog`

### 验证
- ✅ 构建成功；qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 拷贝 WPF 图片资源到 Qt 并注册 ✅

- **提交**：`8171f83` `assets(wpf): copy WPF image resources into Qt and register in qrc`
- **范围**：把 `sy1000-host-wpf/Resources` 的图片资源拷贝到 `qml/assets/` 并注册进 CMake qrc
- **文件**：`dklogo.png`（已用）、`LOGO.png`、`HX1.png`、`HX2.png`、`dk.ico`、`QSI.ico`
- **说明**：WPF 中按钮图标为 MaterialDesignThemes PackIcon（字体，非图片）；图片资源仅 `dklogo` 被登录页实际使用，`HX2` 为登录背景（WPF 注释未启用），其余为备用资源
- **验证**：✅ 构建成功（qrc 重新生成）；启动无回归、stderr 干净

---

## 2026-08-19 · 用户管理卡片图标对齐 WPF ✅

- **提交**：`a6a62d0` `fix(ui): user-management card icon = account-multiple`
- **范围**：`qml/MainMenuPage.qml` 用户管理卡片图标由 `☺` 改为 `👥`（多用户）—— 对齐 WPF `AccountMultiple`
- **验证**：✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 登出菜单按钮标题居中 ✅

- **提交**：`7b610af` `fix(ui): center logout button title`
- **范围**：`qml/MainMenuPage.qml` 登出按钮内容改为 `Item + Row(anchors.centerIn)` 确保图标+标题整体居中
- **验证**：✅ 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 主菜单按钮配色/背景/图标对齐 WPF ✅

- **提交**：`2a9f3e8` `feat(ui): menu card colors + logout button bg/icon match WPF`
- **范围**：主菜单卡片配色 + 登出按钮背景与图标对齐 WPF

### 实现（`qml/MainMenuPage.qml`）
- **开始水压试验卡片**：accent 设为 `#FFC107`（Amber）—— 对齐 WPF `RaisedSecondaryButton`（Secondary 色）
- **登出菜单按钮**：补深色背景 `#283593`（hover `#1A237E`，对齐 `RaisedDarkButton`），白字
- **登出图标**：由 `→` 改为 ExitToApp 风格 `⇤`

### 验证
- ✅ `SY1000.exe` 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 主菜单右下角按钮文案改为"登出菜单" ✅

- **提交**：`157a207` `feat(ui): main menu bottom-right button = Log out Menu`
- **范围**：右下角按钮由"返回主菜单"改为"登出菜单"

### 实现
- `qml/MainMenuPage.qml`：`logoutBtn` 文案 `qsTr("Return to Main Menu")` → `qsTr("Log out Menu")`（"登出菜单"）
- 翻译：新增 `Log out Menu`→登出菜单（`tools/correct_translations.py` 补充）

### 验证
- ✅ `SY1000.exe` 构建成功；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 主菜单卡片风格对齐 WPF（MaterialDesignRaisedButton）✅

- **提交**：`16b192c` `feat(ui): menu cards match WPF MaterialDesignRaisedButton style`
- **范围**：主菜单功能卡片（测试功能 + 管理系统三按钮）风格对齐 WPF 的 MaterialDesignRaisedButton

### 调整（`qml/ShadowCard.qml`）
- 卡片面由**白色卡片 + 圆形 Indigo 图标**改为 **accent 填充背景（Indigo）+ 白色图标 + 白色文字**
- hover 用 `Qt.darker(accent, 1.25)` 变深（对齐 MaterialDesign 按钮 hover）
- 保留模拟阴影（Elevation）与点击

### 验证
- ✅ `SY1000.exe` 构建成功；`ShadowCard.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 补全主菜单右上角登录者信息（对齐 WPF）✅

- **提交**：`0902d96` `feat(ui): show logged-in user in header top-right (Q_PROPERTY NOTIFY + Account icon)`
- **范围**：主菜单顶栏右上角缺少登录者信息（WPF 有 Account 图标 + 登录用户）

### 根因
- `LoginService::username()` 是 `Q_INVOKABLE` 方法，QML 绑定 `text: loginService.username()` 在登录前求值为空后**不自动更新**（无 NOTIFY）→ 登录后用户名不显示

### 修复
- `src/services/loginservice.h/.cpp`：`username` 改为 `Q_PROPERTY(... NOTIFY usernameChanged)`，登录成功 `emit usernameChanged()`
- `qml/Main.qml`：header 右侧改为 **👤 图标 + `loginService.username`**（属性绑定自动更新），对齐 WPF Account 图标 + 登录用户

### 验证
- ✅ `SY1000.exe` 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 连接状态显示两 COM 口 + 右下角按钮加箭头 ✅

- **提交**：`650dead` `feat(ui,services): conn status shows two COM ports + arrow on return button`
- **范围**：①连接状态框显示软件 + TasIO/天平 两 COM 口状态；②主菜单右下角按钮加箭头

### 实现
- `src/services/deviceservice.cpp`：`connectDevices()` 返回 `软件名\nTasIO(COM1): 已连接/未连接\n天平(COM2): 已连接/未连接`
- `qml/Main.qml`：header 新增连接状态框（`connStatus`），连接状态按钮点击后在框内显示状态（原 ToolTip）；修复 Rectangle 无 padding 的 QML 错误
- `qml/MainMenuPage.qml`：右下角按钮 `text: "→  " + qsTr("Return to Main Menu")`（带箭头 + 返回主菜单）

### 验证
- ✅ `SY1000.exe` 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 主菜单右下角按钮改为"返回主菜单" ✅

- **提交**：`6d25121` `feat(ui): main menu bottom-right button label = Return to Main Menu`
- **范围**：主菜单右下角按钮文案由"退出登录"改为"返回主菜单"

### 实现
- `qml/MainMenuPage.qml`：`logoutBtn` 文案 `qsTr("Log out")` → `qsTr("Return to Main Menu")`（"返回主菜单"），宽 200→220
- 翻译：新增 `Return to Main Menu`→返回主菜单；清理 6 个 Drawer 文案的 unfinished 标记（Connection Status 等）
- `tools/correct_translations.py` 补充这些词条

### 验证
- ✅ `SY1000.exe` 构建成功；ts 0 unfinished；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 修复登录后主菜单标题栏不显示 ✅

- **提交**：`6930f06` `fix(ui): show title bar after login (header visible based on isLoginPage)`
- **范围**：登录后主菜单缺标题栏（header）——因 `stack.replace` 后 `depth===1`，header 的 `visible: stack.depth > 1` 恒为假

### 修复
- `qml/LoginPage.qml`：加 `readonly property bool isLoginPage: true`
- `qml/Main.qml`：header `visible` 由 `stack.depth > 1` 改为 `stack.currentItem ? !stack.currentItem.isLoginPage : false`
  - 登录页隐藏 header（无边框），MainMenu 及其它页显示标题栏

### 验证
- ✅ `SY1000.exe` 构建成功；qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 修复主菜单分区不显示（登录后误显示管理系统）✅

- **提交**：`fa14648` `fix(ui): main menu section rectangles now sized by content (was showing only management)`
- **范围**：登录后进入主菜单却只显示"管理系统"区——根因是分区 Rectangle 高度异常

### 根因
- 主菜单两个分区 `Rectangle` 未设显式高度，`Column anchors.fill` 无法撑开 → 分区高度异常、内容错位，登录后看起来像只显示"管理系统"

### 修复（`qml/MainMenuPage.qml`）
- 两个分区 Rectangle 加 `implicitHeight: <col>.implicitHeight + 40`，由内容撑开
- 内层 Column 由 `anchors.fill` 改为 `anchors.left/right/top + margins 20`
- 用 console.log 验证：`testRect h=245, mgmtRect h=245`（两分区均正常显示）

### 验证
- ✅ `SY1000.exe` 构建成功；`MainMenuPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 按规范文档实现主菜单（补汉堡按钮 + 侧边导航）✅

- **提交**：`9304c2e` `feat(ui): main menu per WPF spec + hamburger/drawer nav; add spec doc`
- **范围**：新建 `docs/MAIN_MENU_WPF.md`（WPF 登录后页面内容与布局规范），并按其补全 Qt 实现

### 新增文档
- `docs/MAIN_MENU_WPF.md`：记录 WPF `MenuWindow` 的窗口/顶栏/测试功能/管理系统/退出登录/按钮行为，及 Qt 对照清单

### 补全实现
- **顶栏汉堡按钮**（WPF HamburgerToggleButton）：header 最左加 "☰" 按钮
- **侧边导航 Drawer**：汉堡按钮打开，列出主菜单功能（开始试验/结果管理/用户管理/系统维护/退出登录），点击 `stack.push` 跳转

### 验证
- ✅ `SY1000.exe` 构建成功；`Main.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 补全主菜单顶栏（DeviceName + 连接状态 + 退出按钮）✅

- **提交**：`0a0f339` `feat(ui): complete main-menu header (deviceName + conn status + quit button)`
- **范围**：核对 WPF `MenuWindow` 顶栏后补全 Qt 顶栏缺失项

### 核对发现的缺失
| WPF 顶栏 | Qt 之前 |
|------|------|
| DeviceName（config） | 硬编码 "SY1000" |
| 连接状态按钮 | 无（放 MainMenuPage 右下） |
| 退出按钮（Power） | 无 |

### 补全（`qml/Main.qml` header）
- 左侧：**DeviceName**（`deviceService.deviceName()`，config）+ **连接状态按钮**（点击 `ToolTip.show(connectDevices())`）
- 中间：当前页标题（titleLabel）
- 右侧：用户名 + **电源退出按钮**（⏻）
- header 高度 60→80（对齐 WPF）
- `MainMenuPage`：移除重复的连接状态按钮（改在顶栏），保留右下退出登录

### 验证
- ✅ `SY1000.exe` 构建成功；`Main.qml`/`MainMenuPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 主菜单卡片组件化（Repeater + 数据驱动）✅

- **提交**：`856ef9c` `refactor(ui): main menu cards via Repeater + reusable ShadowCard`
- **范围**：主菜单功能卡片用 `ShadowCard` 组件 + `Repeater` + 数据模型序号化，避免重复代码

### 实现（`qml/MainMenuPage.qml`）
- 测试功能 / 管理系统区的卡片改为 `Repeater` + 数据 model（`{icon, text, page}`）
- `ShadowCard` 作为唯一复用组件，通过 `modelData` 传入不同 icon/text/page，按序号渲染

### 验证
- ✅ `SY1000.exe` 构建成功；`MainMenuPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 主菜单对齐 WPF MenuWindow（分区 + 布局）✅

- **提交**：`0d265e2` `feat(ui): main menu two-section layout matching WPF MenuWindow + zh_CN`
- **范围**：对照 WPF `MenuWindow` 重写 Qt `MainMenuPage` 的控件内容与布局

### 实现（`qml/MainMenuPage.qml`）
- 由单一 2×2 Grid 改为 **两个分区**（对照 WPF）：
  - **测试功能**：标题 + 分隔线 + "开始水压试验"卡片（对应 WPF StartTestButton）
  - **管理系统**：标题 + 分隔线 + "试验结果管理 / 用户管理 / 系统维护"3 卡片一行（对应 WPF 3 列管理卡）
- 右下角：退出登录（WPF LogoutButton）+ 连接状态按钮（WPF ConnectionStatusButton）

### 翻译
- 新增 `Test Functions`→测试功能、`Management System`→管理系统、`Click to sign in...`→点击登录进入主菜单

### 验证
- ✅ `SY1000.exe` 构建成功；`MainMenuPage.qml` qmllint 无 Error；ts 0 unfinished；启动无回归、stderr 干净

---

## 2026-08-19 · 登录卡高度调小 ✅

- **提交**：`3f24331` `feat(ui): reduce login card height to 500`
- **范围**：用户登录框高度 545→500

### 实现（`qml/LoginPage.qml`）
- `loginCard` 高度 `545` → `500`（更紧凑，仍完整容纳标题/用户名/密码/语言/按钮/状态）

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 登录卡布局调整（语言行移到登录按钮上方）✅

- **提交**：`e8a0104` `feat(ui): reorder login card - language row above login button, WPF-aligned spacing`
- **范围**：用户登录框重排——语言行移到登录按钮上方、风格与用户名/密码一致、纵向间距合理

### 实现（`qml/LoginPage.qml`）
- 顺序调整为：标题 → 用户名 → 密码 → **语言行** → 登录按钮 → 状态
- **语言行风格与用户名/密码一致**：🌐 图标 + 270×44 ComboBox（原来是无图标的 "Language:" 标签 + 150 ComboBox）
- **间距参照 WPF**：标题下方 spacer 20（WPF Margin bottom 25）、按钮上方 spacer 22（WPF Margin top 40）
- 登录卡高度 460→545 以容纳内容

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 登录卡阴影四周可见（补左侧）✅

- **提交**：`e57b649` `feat(ui): login card shadow visible on all sides (incl. left)`
- **范围**：登录卡阴影此前只向下，左侧无阴影；改为四周可见（MaterialDesign DropShadowEffect 特性）

### 修复（`qml/LoginPage.qml`）
- 阴影层改用**负 margin**（`leftMargin/rightMargin: -6..-1`、`topMargin: -2..0`、`bottomMargin: -18..-3`），使阴影向**四周**延伸，底部最明显
- 截图验证：卡片左侧 x1300→1340 出现阴影渐变（R=178→254），此前为纯白

### 验证
- ✅ `SY1000.exe` 构建成功；qmllint 无 Error；截图确认左/下阴影可见
- ⚠️ 临时截图 `login_shot4/5/6.png` 可能被进程占用（untracked，未提交），重启后建议删除

---

## 2026-08-19 · 登录卡阴影增强（确保可见）✅

- **提交**：`73fbbb0` `feat(ui): strengthen login card downward shadow to be clearly visible`
- **范围**：此前阴影 alpha 过弱在浅灰背景上不可见，增强为明显的向下阴影

### 修复（`qml/LoginPage.qml`）
- 阴影改为 **4 层向下**（`anchors.bottomMargin` 负值 -4/-9/-15/-22，向下露出 22px）
- alpha 提高至 `#14/#20/#2A/#33`（8%–20%），在浅灰背景上清晰可见
- 截图验证：卡片底部下方 y750→765 渐变（R=131→179），层次感明显（此前纯背景 224 无阴影）

### 验证
- ✅ `SY1000.exe` 构建成功；qmllint 无 Error；截图确认阴影可见；启动无回归

### 备注
- 临时截图文件 `login_shot4.png` 曾被系统进程占用未能删除（untracked，未提交），重启后建议手动删除。

---

## 2026-08-19 · 登录卡阴影参照 WPF Elevation Dp12 ✅

- **提交**：`e37d469` `feat(ui): login card shadow mimics WPF Elevation Dp12 (downward soft)`
- **范围**：按 WPF 登录卡阴影（`ElevationAssist.Elevation="Dp12"` 的 `DropShadowEffect`）调整 Qt 阴影

### 核对
- WPF 登录卡：MaterialDesign `Card` + `ElevationAssist.Elevation="Dp12"`，内部为 `DropShadowEffect`（Direction 270 向下、BlurRadius≈20、ShadowDepth≈10、柔和渐变）

### 调整（`qml/LoginPage.qml`）
- Qt 无 `DropShadowEffect`（GraphicalEffects 不可用），用 **4 层半透明圆角矩形**近似：阴影改为**主要向下**（topMargin 2/5/9/13 递增）+ 左右轻微对称扩散，模拟 Dp12 柔和向下阴影

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 登录卡加多层阴影（层次感）✅

- **提交**：`52ca56b` `feat(ui): layered shadow on login card for depth`
- **范围**：用户登录框阴影增强，模拟 WPF Elevation Dp12

### 实现
- `qml/LoginPage.qml`：登录卡阴影由单层改为 **3 层半透明圆角矩形**（alpha 递减、偏移递增：14/9/4px），营造柔和层次感

### 验证
- ✅ `SY1000.exe` 构建成功；`LoginPage.qml` qmllint 无 Error；启动无回归、stderr 干净

---

## 2026-08-19 · 启动界面无边框（对齐 WPF）✅

- **提交**：`e011ff2` `feat(ui): frameless maximized window to match WPF login`
- **范围**：登录页去掉系统标题栏，对齐 WPF `LoginWindow`（`WindowStyle=None` 无边框、最大化）

### 实现
- `qml/Main.qml`：`ApplicationWindow` 加 `flags: Qt.Window | Qt.FramelessWindowHint`（无边框窗口）
- 结合此前改动：登录页无 header、最大化全屏浅灰、左上角留空、右上角电源退出按钮 —— 与 WPF 启动界面一致

### 验证
- ✅ `SY1000.exe` 构建成功；`Main.qml` qmllint 无 Error；启动无回归、stderr 干净

### 备注
- 无边框窗口在非登录页由自定义深蓝 header 充当标题栏；窗口最大化固定（对齐 WPF）。

---

## 2026-08-19 · 登录页隐藏顶部 header，左上角留空 ✅

- **提交**：`ea4da75` `feat(ui): hide header on login page (frameless look) + quit button on login page`
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
