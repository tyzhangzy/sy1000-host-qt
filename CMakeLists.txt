cmake_minimum_required(VERSION 3.21)
project(SY1000 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt 6: Quick (QML UI) + QuickControls2 (控件) + LinguistTools (i18n)。Charts/Sql/SerialPort 后续按需加入
find_package(Qt6 6.2 REQUIRED COMPONENTS Quick QuickControls2 LinguistTools)
qt_standard_project_setup()

qt_add_executable(SY1000
  src/main.cpp
  src/languagehelper.cpp
)

# 数据模型层（纯 C++ 数据结构，依赖 QtCore）
add_library(models STATIC
  src/models/model.cpp
)
target_include_directories(models PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(models PUBLIC Qt6::Core)

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

target_link_libraries(SY1000 PRIVATE models Qt6::Quick Qt6::QuickControls2)


