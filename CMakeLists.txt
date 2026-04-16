cmake_minimum_required(VERSION 3.16)
project(SmartInventoryManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    database.cpp
    database.h
)

qt_add_executable(SmartInventoryManagementSystem
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(SmartInventoryManagementSystem PRIVATE Qt6::Widgets Qt6::Sql Qt6::Charts)
