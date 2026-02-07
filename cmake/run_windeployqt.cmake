# Runs windeployqt using the helper defined in ProjectOptions.cmake
# Usage: cmake -P cmake/run_windeployqt.cmake -DTARGET_EXE=path/to/app.exe

if(NOT DEFINED TARGET_EXE)
    message(FATAL_ERROR "TARGET_EXE not provided")
endif()

if(NOT EXISTS "${TARGET_EXE}")
    message(FATAL_ERROR "TARGET_EXE not found: ${TARGET_EXE}")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/ProjectOptions.cmake")

qt-splash-screen_find_windeployqt(_windeployqt_exe)
if(NOT _windeployqt_exe)
    message(FATAL_ERROR "windeployqt6/windeployqt not found")
endif()

get_filename_component(_windeployqt_bin_dir "${_windeployqt_exe}" DIRECTORY)
get_filename_component(_qt_prefix "${_windeployqt_bin_dir}/.." ABSOLUTE)
set(_qt_plugins_dir "${_qt_prefix}/plugins")
set(_qt_platforms_dir "${_qt_plugins_dir}/platforms")

set(_windeployqt_args --no-translations)
if(EXISTS "${_qt_platforms_dir}/qwindowsd.dll")
    list(APPEND _windeployqt_args --debug)
else()
    list(APPEND _windeployqt_args --release)
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "PATH=${_windeployqt_bin_dir};$ENV{PATH}"
        "QT_PLUGIN_PATH=${_qt_plugins_dir}"
        "QT_QPA_PLATFORM_PLUGIN_PATH=${_qt_platforms_dir}"
        "${_windeployqt_exe}" ${_windeployqt_args} "${TARGET_EXE}"
    RESULT_VARIABLE _windeployqt_result
)

if(NOT _windeployqt_result EQUAL 0)
    message(FATAL_ERROR "windeployqt failed with code ${_windeployqt_result}")
endif()
