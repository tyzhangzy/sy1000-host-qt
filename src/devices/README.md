# devices/

对应原 WPF 的 `Devices/`。硬件通信层。

计划迁移内容：
- `TasIO`：Modbus RTU 主站（从站 17，数字量 I/O 控制水泵/水阀）→ `QSerialPort` + libmodbus / 自研 Modbus 帧
- `PrecisaScale`：多个电子天平（Modbus RTU 从站，25ms 轮询）→ `QSerialPort`
- `DeviceManager`：设备连接/枚举 → `QSerialPortInfo::availablePorts()`（跨平台）

> 硬件轮询线程 + 环形缓冲 + 定时批量刷新，参考原 WPF 已优化策略。
