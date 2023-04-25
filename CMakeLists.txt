# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.18)

set(
    CMAKE_TOOLCHAIN_FILE
    "vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "vcpkg toolchain file")
set(QT_CREATOR_SKIP_VCPKG_SETUP ON)

set(VCPKG_FEATURE_FLAGS "versions")

project(ModelTea VERSION 0.1 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp
    src/blocks/block_object.h src/blocks/block_object.cpp
    src/blocks/connector_object.h src/blocks/connector_object.cpp
    src/blocks/connector_block_object.h src/blocks/connector_block_object.cpp
    src/dialogs/model_parameters_dialog.h src/dialogs/model_parameters_dialog.cpp src/dialogs/model_parameters_dialog.ui
    src/dialogs/model_diagnostics_dialog.h src/dialogs/model_diagnostics_dialog.cpp src/dialogs/model_diagnostics_dialog.ui
    src/dialogs/block_selector_dialog.h src/dialogs/block_selector_dialog.cpp src/dialogs/block_selector_dialog.ui
    src/dialogs/block_parameters_dialog.h src/dialogs/block_parameters_dialog.cpp src/dialogs/block_parameters_dialog.ui
    src/dialogs/connection_parameters_dialog.h src/dialogs/connection_parameters_dialog.cpp src/dialogs/connection_parameters_dialog.ui
    src/events/sim_event.h src/events/sim_event.cpp
    src/managers/executor_manager.h src/managers/executor_manager.cpp
    src/managers/window_manager.h src/managers/window_manager.cpp
    src/models/plot_variable_selection_model.h src/models/plot_variable_selection_model.cpp
    src/widgets/block_graphics_view.h src/widgets/block_graphics_view.cpp
    src/widgets/state/mouse/mouse_state_base.h
    src/widgets/state/mouse/block_drag_state.h src/widgets/state/mouse/block_drag_state.cpp
    src/widgets/state/mouse/port_drag_state.h src/widgets/state/mouse/port_drag_state.cpp
    src/widgets/parameters/parameter_boolean_widget.h src/widgets/parameters/parameter_boolean_widget.cpp src/widgets/parameters/parameter_boolean_widget.ui
    src/widgets/parameters/parameter_datatype_widget.h src/widgets/parameters/parameter_datatype_widget.cpp src/widgets/parameters/parameter_datatype_widget.ui
    src/widgets/parameters/parameter_numeric_widget.h src/widgets/parameters/parameter_numeric_widget.cpp src/widgets/parameters/parameter_numeric_widget.ui
    src/widgets/parameters/parameter_unknown_widget.h src/widgets/parameters/parameter_unknown_widget.cpp src/widgets/parameters/parameter_unknown_widget.ui
    src/windows/model_window.h src/windows/model_window.cpp src/windows/model_window.ui
    src/windows/plot_window.h src/windows/plot_window.cpp src/windows/plot_window.ui
    src/ui/window_icon.h src/ui/window_icon.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ModelTea
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(ModelTea
        ${PROJECT_SOURCES}
    )
endif()

set_property(TARGET ModelTea PROPERTY AUTOUIC ON)
set_property(TARGET ModelTea PROPERTY AUTOMOC ON)
set_property(TARGET ModelTea PROPERTY AUTORCC ON)

set_property(TARGET ModelTea PROPERTY CXX_STANDARD 20)
set_property(TARGET ModelTea PROPERTY CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/tmdl)
add_subdirectory(lib/tmdlstd)

target_include_directories(ModelTea PRIVATE src/)

target_link_libraries(ModelTea PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ModelTea PRIVATE tmdl)

target_link_libraries(ModelTea PRIVATE Qt::Widgets Qt::Charts)
target_link_libraries(ModelTea PUBLIC nlohmann_json::nlohmann_json)

set_target_properties(ModelTea PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.teafis.modeltea
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_finalize_executable(ModelTea)
endif()

# Add compiler options
if (MSVC)
    target_compile_options(ModelTea PUBLIC /W4 /WX)
else()
    target_compile_options(ModelTea PUBLIC -Wall -Wextra -pedantic -Werror)

    if (MODELTEA_USE_ASAN)
        set_property(TARGET ModelTea PROPERTY CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()

    if (MODELTEA_USE_LSAN)
        set_property(TARGET ModelTea PROPERTY CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()
endif()
