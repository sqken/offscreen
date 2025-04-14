cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

# set(CMAKE_PREFIX_PATH "/home/shiqing/Qt/6.5.6/gcc_64")

# set(CMAKE_SYSTEM_NAME QNX)
# set(CMAKE_SYSTEM_PROCESSOR aarch64le)
# set(arch gcc_ntoaarch64le)

# set(CMAKE_C_COMPILER qcc)
# set(CMAKE_C_COMPILER_TARGET ${arch})
# set(CMAKE_CXX_COMPILER q++)
# set(CMAKE_CXX_COMPILER_TARGET ${arch})

# set(CMAKE_FIND_ROOT_PATH $ENV{QNX_TARGET};$ENV{QNX_TARGET}/${CMAKE_SYSTEM_PROCESSOR})

# set(CMAKE_SYSROOT $ENV{QNX_TARGET})

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt 包
find_package(Qt6 REQUIRED COMPONENTS Core Quick)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 添加资源文件
# qt6_add_resources(QRC_FILES resources/resources.qrc)

# 添加源文件
add_executable(${PROJECT_NAME}
    main.cpp
    resources/resources.qrc
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Core Qt6::Quick
)
