cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt 包
find_package(Qt6 REQUIRED COMPONENTS Core Quick Gui QuickControls2 OpenGLWidgets OpenGL)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 添加资源文件
# qt6_add_resources(QRC_FILES resources/resources.qrc)

# 添加源文件
add_executable(${PROJECT_NAME}
    main.cpp
    OffscreenRenderer.cpp
    resources/resources.qrc
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Core Qt6::Quick Qt6::Gui Qt6::QuickControls2 Qt6::OpenGLWidgets Qt6::OpenGL
)
