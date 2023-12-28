# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.18)

enable_testing()

set(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")

set(
    CMAKE_TOOLCHAIN_FILE
    "vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "vcpkg toolchain file")
set(QT_CREATOR_SKIP_VCPKG_SETUP ON)

set(VCPKG_FEATURE_FLAGS "versions")

project(ModelTea VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

option(MT_USE_ASAN "Enable the use of ASAN when compiling ModelTea")
option(MT_EXTRA_FLAGS "Enable the use of extra warning flags when compiling ModelTea" ON)

find_package(QT NAMES Qt6 COMPONENTS Core Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Charts REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

set(MTSTD_USE_STRING_FUNCS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib/mtstdlib)
add_subdirectory(lib/tmdl)

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
    src/exceptions/teafis_exception.h
    src/exceptions/block_object_exception.h
    src/exceptions/model_exception.h
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
)

qt_add_executable(ModelTea
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

set_property(TARGET ModelTea PROPERTY AUTOUIC ON)
set_property(TARGET ModelTea PROPERTY AUTOMOC ON)
set_property(TARGET ModelTea PROPERTY AUTORCC ON)

set_property(TARGET ModelTea PROPERTY CXX_STANDARD 20)
set_property(TARGET ModelTea PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(ModelTea PRIVATE src/)

target_link_libraries(ModelTea PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ModelTea PRIVATE tmdl mtstdlib)

target_link_libraries(ModelTea PRIVATE Qt::Widgets Qt::Charts)
target_link_libraries(ModelTea PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(ModelTea PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.teafis.ModelTea
    MACOSX_BUNDLE_BUNDLE_NAME ModelTea
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE macos.icns)
    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/rcs/icons/macos.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(ModelTea PRIVATE ${app_icon_macos})
elseif(WIN32)
    target_sources(ModelTea PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/rcs/icons/windows.rc")
endif()

if(APPLE)
    set(ICONFILE macos)
else()
    set(ICONFILE icon)
endif()

set_source_files_properties(rcs/icons/${ICONFILE}.png PROPERTIES QT_RESOURCE_ALIAS /icons/icon.png)
qt_add_resources(ModelTea "app_icon"
    PREFIX /
    FILES
        rcs/icons/${ICONFILE}.png
)

include(GNUInstallDirs)
install(TARGETS ModelTea
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(ModelTea)

# Add compiler options
if (MT_EXTRA_FLAGS)
    if (MSVC)
        add_compile_options(/W4 /WX)
    else()
        add_compile_options(-Wall -Wextra -pedantic -Werror)
    endif()
endif()

if (MT_USE_ASAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
endif()
