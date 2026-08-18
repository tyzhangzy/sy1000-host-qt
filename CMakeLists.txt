cmake_minimum_required(VERSION 3.21)
project(SY1000 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt 6: Quick (QML UI) + QuickControls2 + LinguistTools (i18n) + Sql (SQLite) + SerialPort
find_package(Qt6 6.2 REQUIRED COMPONENTS Quick QuickControls2 LinguistTools Sql SerialPort)
qt_standard_project_setup()

# Data model layer (pure C++ standard library, cross-platform; no Qt dependency)
add_library(models STATIC
  src/models/model.cpp
)
target_include_directories(models PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# DAO layer (SQLite; uses the std::string models)
add_library(dao STATIC
  src/dao/database.cpp
  src/dao/userdao.cpp
  src/dao/testresultdao.cpp
  src/dao/json_serializer.cpp
)
target_include_directories(dao PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(dao PUBLIC models Qt6::Sql)

# Core layer: state machine (State pattern) + async sub-tasks
add_library(core STATIC
  src/core/subtask.cpp
  src/core/tasks.cpp
  src/core/states.cpp
  src/core/controller.cpp
  src/core/simdevice.h
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

# Services layer (Qt bridge: login + result + hydro adapter), depends on dao + models + core
add_library(services STATIC
  src/services/loginservice.cpp
  src/services/testresultservice.cpp
  src/services/hydroadapter.cpp
  src/services/resultservice.cpp
)
target_include_directories(services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(services PUBLIC dao models core Qt6::Core Qt6::Sql)

# Devices layer (QSerialPort + Modbus RTU), cross-platform
add_library(devices STATIC
  src/devices/modbusrtu.cpp
  src/devices/tasio.cpp
  src/devices/precisa.cpp
  src/devices/devicemanager.cpp
)
target_include_directories(devices PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(devices PUBLIC Qt6::Core Qt6::SerialPort)

# Realtime chart widget (reused QQuickPaintedItem component)
add_library(charts STATIC
  src/ui/charts/RealTimeChart.cpp
)
target_include_directories(charts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(charts PUBLIC Qt6::Core Qt6::Gui Qt6::Quick)

qt_add_executable(SY1000
  src/main.cpp
  src/languagehelper.cpp
)

# QML resources bundled into the executable
qt_add_resources(SY1000 "app"
  PREFIX "/"
  FILES
    qml/Main.qml
    qml/LoginPage.qml
    qml/MainMenuPage.qml
    qml/TestPage.qml
    qml/ResultManagementPage.qml
)

# i18n: lrelease compiles ts -> qm, embedded under /i18n/
set(TS_FILES
  i18n/sy1000_zh_CN.ts
)
qt_add_lrelease(SY1000 TS_FILES ${TS_FILES})

target_link_libraries(SY1000 PRIVATE core services models dao devices charts Qt6::Quick Qt6::QuickControls2 Qt6::Sql)


