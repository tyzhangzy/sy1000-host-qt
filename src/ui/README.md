# ui/

对应原 WPF 的窗口/页面。QML 界面层。

计划迁移内容：
- 登录窗口（原 `LoginWindow`）→ QML `LoginPage`
- 主菜单（原 `MenuWindow`）→ QML 导航
- 主测试窗口（原 `MainTestWindow` + `HydroStaticTestPage`）→ QML
- 管理窗口（用户 / 试验结果 / 报告 / 系统维护）→ QML
- 实时曲线控件：`QQuickPaintedItem` 子类（既有可复用 C++ 组件）
