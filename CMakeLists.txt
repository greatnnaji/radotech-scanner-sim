cmake_minimum_required(VERSION 3.16)
project(RaDoTechScannerSim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS src)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)

qt_add_executable(RaDoTechScannerSim
    src/main.cpp
    src/mainwindow.cpp src/mainwindow.h src/mainwindow.ui
    src/createuserdialog.cpp src/createuserdialog.h
    src/scanplotdialog.cpp src/scanplotdialog.h
    src/user.cpp src/user.h
    src/qcustomplot.cpp src/qcustomplot.h
)

target_include_directories(RaDoTechScannerSim PRIVATE src)
target_link_libraries(RaDoTechScannerSim PRIVATE Qt6::Widgets Qt6::PrintSupport)
set_target_properties(RaDoTechScannerSim PROPERTIES MACOSX_BUNDLE TRUE WIN32_EXECUTABLE TRUE)
qt_finalize_executable(RaDoTechScannerSim)
