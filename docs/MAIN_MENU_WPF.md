# WPF 登录后主菜单（MenuWindow）内容与布局规范

> 来源：`sy1000-host-wpf/MenuWindow/MenuWindow.xaml` + `.xaml.cs`
> 用途：作为 Qt `MainMenuPage.qml` + `Main.qml` header 的实现依据。

---

## 1. 窗口

- 尺寸 1920×1080，`WindowState=Maximized`，`ResizeMode=NoResize`，`WindowStyle=None`（无边框），`Title="DKSY"`
- 背景默认（浅色），整体 4 行 Grid：`0.5*`（顶部留白）/ `auto`（顶栏）/ `auto`（功能区）/ `*`（其他）

## 2. 顶栏（ColorZone，Height=80，PrimaryDark，Elevation Dp4）

| 位置 | 控件 | 说明 |
|------|------|------|
| 左（宽 800） | **汉堡 ToggleButton** | MaterialDesignHamburgerToggleButton（导航抽屉开关） |
| 左 | **DeviceName** | 来自 config（28px Bold，白色） |
| 左 | **连接状态按钮** | ConnectionStatusButton，300×40，FlatButton 白背景，"连接状态"；点击重试连接并显示状态 |
| 中 | **当前页标题** | "主菜单"（28px Bold，白色，居中） |
| 右（宽 400，右对齐） | **登录用户** | Account 图标(32) + 用户名(22px) |
| 右 | **退出按钮** | QuitButton（Power 电源图标 30×30，透明背景白字）→ `Application.Shutdown()` |

## 3. 测试功能 分区（Card，Elevation Dp4）

- 标题 **"测试功能"**（24px Bold，PrimaryHueMidBrush）+ 分隔线（Separator）
- **开始水压试验** 卡片（StartTestButton，`IsEnabled=False`，PlayCircle 图标 40 + "开始水压试验"，RaisedSecondaryButton，Height 120）
  - 点击 → 打开 `MainTestWindow`（试验准备 + 水压试验）
- （"开始准备试验" 已注释隐藏）

## 4. 管理系统 分区（Card，Elevation Dp4）

- 标题 **"管理系统"**（24px Bold，PrimaryHueMidBrush）+ 分隔线（Separator）
- Grid 3 列，每卡片 Height 120，图标 40 + 文字：
  | 卡片 | 图标 | 点击 → |
  |------|------|--------|
  | **试验结果管理** | FileDocument | `UnifiedTestResultManagementWindow` |
  | **用户管理** | AccountMultiple | `UserManagementWindow` |
  | **系统维护** | FileDocumentMultiple | `SystemMaintainWindow` |
- （"被测呼吸器管理" 已注释隐藏）

## 5. 右下角

- **退出登录** 按钮（LogoutButton，200×80，RaisedDarkButton，ExitToApp 图标 + "退出登录"）
  - 点击：清空 `UserSession`（Username/IsAdmin），关闭本窗口，回到 `LoginWindow`

## 6. 各按钮行为

| 控件 | 行为 |
|------|------|
| 连接状态按钮 | 重试连接设备（`ConnectDevicesAsync`），按钮显示"连接成功/连接错误" |
| 开始水压试验 | 打开 MainTestWindow（模态） |
| 试验结果管理 | 打开 UnifiedTestResultManagementWindow（模态） |
| 用户管理 | 打开 UserManagementWindow（模态） |
| 系统维护 | 打开 SystemMaintainWindow（模态） |
| 退出按钮 | 关闭应用 |
| 退出登录 | 清会话，回登录窗口 |

---

## 7. Qt 实现对照（MainMenuPage + Main.qml header）

| WPF | Qt 实现 | 状态 |
|-----|---------|------|
| 顶栏 DeviceName | `deviceService.deviceName()` | ✅ |
| 顶栏 连接状态按钮 | header 白色按钮 + `ToolTip.show(connectDevices())` | ✅ |
| 顶栏 当前页标题 | header titleLabel | ✅ |
| 顶栏 登录用户 | header username | ✅ |
| 顶栏 退出按钮 | header 电源按钮 ⏻ | ✅ |
| **顶栏 汉堡按钮** | **缺失，需补** | ⬜ |
| 测试功能分区（标题+分隔+开始水压试验） | MainMenuPage 测试功能区 | ✅ |
| 管理系统分区（标题+分隔+3卡片） | MainMenuPage 管理系统区（Repeater） | ✅ |
| 右下退出登录 | MainMenuPage Logout 按钮 | ✅ |
