cmake_minimum_required(VERSION 3.16)

project(demo_listview VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt包
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# 源文件
set(SOURCES
    main.cpp
    mainwindow.cpp
    listitem.cpp
    listmodel.cpp
    listitemdelegate.cpp
)

set(HEADERS
    mainwindow.h
    listitem.h
    listmodel.h
    listitemdelegate.h
)

set(UI_FILES
    mainwindow.ui
)

set(RESOURCES
    resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCES})

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    utils
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/utils
)
