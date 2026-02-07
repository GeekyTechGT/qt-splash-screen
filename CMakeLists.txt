cmake_minimum_required(VERSION 3.16)

project(qt-splash-screen
    VERSION 1.0.0
    DESCRIPTION "Custom reusable splash screen widget for Qt applications"
    LANGUAGES CXX
)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 auto-generation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Svg
    SvgWidgets
    Concurrent
)

# Library sources
set(SPLASH_SOURCES
    src/SplashScreenWidget.cpp
)

set(SPLASH_HEADERS
    src/SplashScreenWidget.h
)

# Create shared library (DLL)
add_library(${PROJECT_NAME} SHARED
    ${SPLASH_SOURCES}
    ${SPLASH_HEADERS}
)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::Concurrent
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set output directory for build products
# Use the preset build directory to avoid duplicate folders under /build.
set(_build_output_dir "${CMAKE_BINARY_DIR}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${_build_output_dir}/$<CONFIG>"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${_build_output_dir}/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_build_output_dir}/release"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_build_output_dir}/relwithdebinfo"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${_build_output_dir}/minsizerel"
    LIBRARY_OUTPUT_DIRECTORY "${_build_output_dir}/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${_build_output_dir}/debug"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${_build_output_dir}/release"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_build_output_dir}/relwithdebinfo"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${_build_output_dir}/minsizerel"
    ARCHIVE_OUTPUT_DIRECTORY "${_build_output_dir}/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${_build_output_dir}/debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${_build_output_dir}/release"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_build_output_dir}/relwithdebinfo"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${_build_output_dir}/minsizerel"
)

# Example application (builds but is not deployed/installed)
add_executable(${PROJECT_NAME}-example WIN32
    example/main.cpp
    example/MainWindow.cpp
    example/MainWindow.h
)

set_target_properties(${PROJECT_NAME}-example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${_build_output_dir}/$<CONFIG>"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${_build_output_dir}/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_build_output_dir}/release"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_build_output_dir}/relwithdebinfo"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${_build_output_dir}/minsizerel"
)

target_link_libraries(${PROJECT_NAME}-example PRIVATE
    ${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::Concurrent
)

target_include_directories(${PROJECT_NAME}-example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/example
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Copy resources to output directory for the example app only
add_custom_command(TARGET ${PROJECT_NAME}-example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}-example>/resources"
    COMMENT "Copying resources to output directory..."
)

# Install rules: library + headers only (no example deploy)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${SPLASH_HEADERS}
    DESTINATION include
)
