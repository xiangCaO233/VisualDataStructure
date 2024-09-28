cmake_minimum_required(VERSION 3.28)

project(DataStructure)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC TRUE)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(INCLUDES headers)
set(SOURCES src/main.cpp src/ArrayWidget.cpp src/MainWidget.cpp
            src/ArrayWgtController.cpp)

find_package(
  Qt6
  COMPONENTS Core Gui Widgets
  REQUIRED)

add_executable(DataStructure ${SOURCES})

target_include_directories(DataStructure PUBLIC ${INCLUDES})
target_link_libraries(DataStructure Qt::Core Qt::Gui Qt::Widgets)
