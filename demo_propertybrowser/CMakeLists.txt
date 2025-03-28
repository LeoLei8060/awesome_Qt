cmake_minimum_required(VERSION 3.16)

project(demo_propertybrowser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt包
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# qtpropertybrowser 源文件
set(QTPROPERTYBROWSER_SOURCES
    qtpropertybrowser/qtbuttonpropertybrowser.cpp
    qtpropertybrowser/qteditorfactory.cpp
    qtpropertybrowser/qtgroupboxpropertybrowser.cpp
    qtpropertybrowser/qtpropertybrowser.cpp
    qtpropertybrowser/qtpropertybrowserutils.cpp
    qtpropertybrowser/qtpropertymanager.cpp
    qtpropertybrowser/qttreepropertybrowser.cpp
    qtpropertybrowser/qtvariantproperty.cpp
)

set(QTPROPERTYBROWSER_HEADERS
    qtpropertybrowser/qtbuttonpropertybrowser.h
    qtpropertybrowser/qteditorfactory.h
    qtpropertybrowser/qtgroupboxpropertybrowser.h
    qtpropertybrowser/qtpropertybrowser.h
    qtpropertybrowser/qtpropertybrowserutils_p.h
    qtpropertybrowser/qtpropertymanager.h
    qtpropertybrowser/qttreepropertybrowser.h
    qtpropertybrowser/qtvariantproperty.h
)

# 源文件
set(SOURCES
    main.cpp
    mainwindow.cpp
    ${QTPROPERTYBROWSER_SOURCES}
)

set(HEADERS
    mainwindow.h
    ${QTPROPERTYBROWSER_HEADERS}
)

set(UI_FILES
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

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
