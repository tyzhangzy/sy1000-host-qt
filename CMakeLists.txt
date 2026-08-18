cmake_minimum_required(VERSION 3.21)
project(SY1000 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt 6: Quick (QML UI) + QuickControls2 (控件) + LinguistTools (i18n) + Sql (SQLite) + SerialPort (串口)
find_package(Qt6 6.2 REQUIRED COMPONENTS Quick QuickControls2 LinguistTools Sql SerialPort)
qt_standard_project_setup()

# 数据模型层（纯 C++ 标准库，跨平台；不依赖 Qt）
add_library(models STATIC
  src/models/model.cpp
)
target_include_directories(models PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# DAO 层（SQLite，使用 models 的 std::string 结构）
add_library(dao STATIC
  src/dao/database.cpp
  src/dao/userdao.cpp
  src/dao/testresultdao.cpp
  src/dao/json_serializer.cpp
)
target_include_directories(dao PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(dao PUBLIC models Qt6::Sql)

# 状态机/业务核心层（手写 C++ 异步状态机），依赖 models
add_library(core STATIC
  src/core/subtask.cpp
  src/core/tasks.cpp
  src/core/controller.cpp
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(core PUBLIC models Qt6::Core)

# Headless smoke test for the data/services layer
qt_add_executable(testcore
  src/tests/testcore.cpp
)
target_link_libraries(testcore PRIVATE services models dao Qt6::Core Qt6::Sql)

# Headless smoke test for the devices layer (Modbus frame + CRC)
qt_add_executable(testdevices
  src/tests/testdevices.cpp
)
target_link_libraries(testdevices PRIVATE devices Qt6::Core Qt6::SerialPort)

# Headless smoke test for the hydrostatic state machine
qt_add_executable(testcontroller
  src/tests/testcontroller.cpp
)
target_link_libraries(testcontroller PRIVATE core models Qt6::Core)

# 服务层（Qt 桥接：登录 + 结果服务），依赖 dao + models
add_library(services STATIC
  src/services/loginservice.cpp
  src/services/testresultservice.cpp
)
target_include_directories(services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(services PUBLIC dao models Qt6::Core Qt6::Sql)

# 设备层（QSerialPort + Modbus RTU），跨平台
add_library(devices STATIC
  src/devices/modbusrtu.cpp
  src/devices/tasio.cpp
  src/devices/precisa.cpp
  src/devices/devicemanager.cpp
)
target_include_directories(devices PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(devices PUBLIC Qt6::Core Qt6::SerialPort)

qt_add_executable(SY1000
  src/main.cpp
  src/languagehelper.cpp
)

# 界面资源（QML）打包进可执行文件
qt_add_resources(SY1000 "app"
  PREFIX "/"
  FILES
    qml/Main.qml
    qml/LoginPage.qml
)

# 国际化：lrelease 编译 ts -> qm，并嵌入 /i18n/ 资源前缀
set(TS_FILES
  i18n/sy1000_zh_CN.ts
)
qt_add_lrelease(SY1000 TS_FILES ${TS_FILES})

target_link_libraries(SY1000 PRIVATE services models dao devices Qt6::Quick Qt6::QuickControls2 Qt6::Sql)


