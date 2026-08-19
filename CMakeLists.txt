cmake_minimum_required(VERSION 3.21)
project(SY1000 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt 6: Quick (QML UI) + QuickControls2 + LinguistTools (i18n) + Sql (SQLite) + SerialPort
# qt_standard_project_setup() requires Qt >= 6.3 (L16).
find_package(Qt6 6.3 REQUIRED COMPONENTS Quick QuickControls2 LinguistTools Sql SerialPort PrintSupport)
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

# Report layer: PDF test report generation (QTextDocument + QPrinter)
add_library(report STATIC
  src/report/testreportgenerator.cpp
)
target_include_directories(report PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(report PUBLIC models Qt6::Core Qt6::Gui Qt6::PrintSupport)

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

# Headless smoke test for PDF report generation
qt_add_executable(testreport
  src/tests/testreport.cpp
)
target_link_libraries(testreport PRIVATE report models Qt6::Core Qt6::Gui Qt6::PrintSupport)

# Headless smoke test for ConfigManager (config.json loading)
qt_add_executable(testconfig
  src/tests/testconfig.cpp
)
target_link_libraries(testconfig PRIVATE services Qt6::Core)

# Headless smoke test for LoginService
qt_add_executable(testlogin
  src/tests/testlogin.cpp
)
target_link_libraries(testlogin PRIVATE services Qt6::Core)

# Services layer (Qt bridge: login + result + hydro adapter), depends on dao + models + core
add_library(services STATIC
  src/services/loginservice.cpp
  src/services/testresultservice.cpp
  src/services/hydroadapter.cpp
  src/services/resultservice.cpp
  src/services/userservice.cpp
  src/services/deviceservice.cpp
  src/services/configmanager.cpp
)
target_include_directories(services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(services PUBLIC dao models core devices report Qt6::Core Qt6::Sql Qt6::Gui Qt6::PrintSupport)

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
    qml/TestPreparationPage.qml
    qml/TestPage.qml
    qml/ResultManagementPage.qml
    qml/ResultDetailsPage.qml
    qml/UserManagementPage.qml
    qml/SystemMaintainPage.qml
    qml/AppearanceInspectionPage.qml
    qml/FieldRow.qml
    qml/InspectionExternal.qml
    qml/InspectionInternal.qml
    qml/InspectionThread.qml
    qml/InspectionValve.qml
    qml/HydroTestMessageDialog.qml
    qml/ReportViewPage.qml
    qml/ShadowCard.qml
    qml/assets/dklogo.png
    qml/assets/LOGO.png
    qml/assets/HX1.png
    qml/assets/HX2.png
    qml/assets/dk.ico
    qml/assets/QSI.ico
)

# i18n: lrelease compiles ts -> qm, embedded under /i18n/
set(TS_FILES
  i18n/sy1000_zh_CN.ts
)
qt_add_lrelease(SY1000 TS_FILES ${TS_FILES})

# Embed the compiled .qm at /i18n/ so LanguageHelper can load it at runtime.
qt_add_resources(SY1000 "translations"
  PREFIX "/i18n"
  BASE ${CMAKE_CURRENT_BINARY_DIR}
  FILES ${CMAKE_CURRENT_BINARY_DIR}/sy1000_zh_CN.qm
)

target_link_libraries(SY1000 PRIVATE core services models dao devices charts Qt6::Quick Qt6::QuickControls2 Qt6::Sql)

# Ship config.json next to the executable (ConfigManager loads it at startup).
add_custom_command(TARGET SY1000 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_CURRENT_SOURCE_DIR}/config.json $<TARGET_FILE_DIR:SY1000>/config.json)

# ---- Install / packaging rules (L16) ----
# `cmake --install` copies the app + config to the install prefix. For a
# self-contained bundle run windeployqt (Windows) / linuxdeployqt (Linux,
# Kylin/UOS) on the installed exe, e.g.:
#   windeployqt.exe --qmldir <src>/qml <install>/bin/SY1000.exe
install(TARGETS SY1000 RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config.json DESTINATION bin)


