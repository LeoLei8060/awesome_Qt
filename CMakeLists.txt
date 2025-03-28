cmake_minimum_required(VERSION 3.16)

project(awesome_Qt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt包
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# 设置全局包含路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 添加utils库
add_subdirectory(utils)

# 添加各个子项目
add_subdirectory(demo_qcustomplot)
add_subdirectory(demo_listview)
add_subdirectory(demo_tableview)
# add_subdirectory(demo_propertybrowser)
# add_subdirectory(demo_dockwidget)
add_subdirectory(demo_listview_custom)
