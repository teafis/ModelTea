# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.18)

project(TSim VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

set(PROJECT_SOURCESS
    src/main.cpp
    src/windows/main_window.h src/windows/main_window.cpp src/windows/main_window.ui
    src/windows/block_library.h src/windows/block_library.cpp src/windows/block_library.ui
    src/windows/plot_window.h src/windows/plot_window.cpp src/windows/plot_window.ui
    src/windows/parameter_dialog.h src/windows/parameter_dialog.cpp src/windows/parameter_dialog.ui
    src/windows/model_error_dialog.h src/windows/model_error_dialog.cpp src/windows/model_error_dialog.ui
    src/windows/main_window_icon.h src/windows/main_window_icon.cpp
    src/widgets/block_graphics_view.h src/widgets/block_graphics_view.cpp
    src/widgets/state/mouse_state_base.h
    src/widgets/state/block_drag_state.h src/widgets/state/block_drag_state.cpp
    src/widgets/state/port_drag_state.h
    src/widgets/parameters/parameter_boolean_widget.h src/widgets/parameters/parameter_boolean_widget.cpp src/widgets/parameters/parameter_boolean_widget.ui
    src/widgets/parameters/parameter_numeric_widget.h src/widgets/parameters/parameter_numeric_widget.cpp src/widgets/parameters/parameter_numeric_widget.ui
    src/widgets/parameters/parameter_datatype_widget.h src/widgets/parameters/parameter_datatype_widget.cpp src/widgets/parameters/parameter_datatype_widget.ui
    src/widgets/parameters/parameter_unknown_widget.h src/widgets/parameters/parameter_unknown_widget.cpp src/widgets/parameters/parameter_unknown_widget.ui
    src/blocks/block_object.h src/blocks/block_object.cpp
    src/blocks/connector_object.h src/blocks/connector_object.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TSim
        MANUAL_FINALIZATION
        ${PROJECT_SOURCESS}
    )
else()
    add_executable(TSim
        ${PROJECT_SOURCESS}
    )
endif()

# Add library directories
add_subdirectory(lib/tmdl)
add_subdirectory(lib/json)

target_include_directories(TSim PRIVATE src/)

target_link_libraries(TSim PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(TSim PRIVATE TeaFIS-Model)

#target_compile_options(TSim PRIVATE -fsanitize=address)
#target_link_options(TSim PRIVATE -fsanitize=address)

#target_compile_options(TSim PRIVATE -fsanitize=leak)
#target_link_options(TSim PRIVATE -fsanitize=leak)

target_link_libraries(TSim PRIVATE Qt::Widgets Qt::Charts)

set_target_properties(TSim PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.teafis.tsim
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TSim)
endif()
