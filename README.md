# SY1000-host-qt

`sy1000-host-wpf`（.NET 8 / WPF 气瓶水压试验上位机系统）的 **Qt 6 + CMake 跨平台重构工程**。

## 背景

- 原项目：`D:\LocalSoftware\sy1000-host-wpf`（约 1.17 万行 C#/XAML）。
- 动因：客户使用国产操作系统（麒麟 / 统信 UOS），需要跨平台（x86 / ARM64 / 龙芯）。
- 技术选型：Qt 6（官方内置国产系统各芯片 SDK），CMake 跨平台构建。
- 方案文档：见原仓库 `sy1000-host-wpf/Qt6_CMake_CrossPlatform_Implementation.md`。

## 目录分层（对应原 WPF 业务结构）

```
sy1000-host-qt/
├── src/
│   ├── main.cpp          # 入口（QML 窗口）
│   ├── core/             # 状态机（HydrostaticTestController） + 业务计算
│   ├── devices/          # TasIO + PrecisaScale（QSerialPort + Modbus RTU）
│   ├── dao/              # SQLite 数据访问（原 LiteDB）
│   ├── report/           # PDF 报告（QTextDocument::print / QPrinter）
│   └── ui/               # QML 界面 + 自定义控件（QQuickPaintedItem 实时曲线）
├── qml/                  # QML 资源
├── CMakeLists.txt
└── CMakePresets.json     # 多架构构建矩阵
```

## 构建

```bash
# Windows MSVC (Qt 6.8.3 msvc2022_64)
cmake --preset msvc
cmake --build --preset msvc

# 运行（需 Qt 的 bin 在 PATH，或从 Qt Creator 运行）
./build/msvc/Debug/SY1000.exe
```

> 后续按需加入 Qt 模块：Charts（报告图表）、Sql（数据库）、SerialPort（串口）、Multimedia（语音）。
