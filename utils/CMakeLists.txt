cmake_minimum_required(VERSION 3.16)

project(utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt包
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# 源文件
set(SOURCES
    # 添加源文件
)

set(HEADERS
    # 添加头文件
)

# 创建库
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# 包含目录
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
