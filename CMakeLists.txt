cmake_minimum_required(VERSION 3.20)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" OPENDOORS_VERSION
    LIMIT_COUNT 1)
project(OpenDoors VERSION "${OPENDOORS_VERSION}" LANGUAGES C)

include(CTest)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(OPENDOORS_BUILD_SHARED "Build the OpenDoors shared library" ON)
option(OPENDOORS_BUILD_STATIC "Build the OpenDoors static library" ON)
option(OPENDOORS_BUILD_MSVC_STATIC_MT
    "Build an additional MSVC static library using the static CRT" OFF)
option(OPENDOORS_BUILD_EXAMPLES "Build the OpenDoors example programs" ON)
option(OPENDOORS_WARNINGS_AS_ERRORS
    "Treat OpenDoors compiler and linker warnings as errors" OFF)
option(OPENDOORS_ENABLE_PTHREAD_KERNEL
    "Enable the experimental standard-pthread kernel on Unix" OFF)
set(OPENDOORS_XPDEV_DIR "" CACHE PATH
    "Path to xpdev; when empty, only ../xpdev is checked")

set(OPENDOORS_SOURCES
    ODAuto.c
    ODBlock.c
    ODCFile.c
    ODCmdLn.c
    ODCom.c
    ODCore.c
    ODDrBox.c
    ODEdit.c
    ODEdStr.c
    ODEmu.c
    ODFormat.c
    ODFmtFB.c
    ODGetIn.c
    ODGraph.c
    ODInEx1.c
    ODInEx2.c
    ODInQue.c
    ODKrnl.c
    ODList.c
    ODLog.c
    ODMulti.c
    ODPlat.c
    ODPCB.c
    ODPopup.c
    ODPrntf.c
    ODRA.c
    ODSafe.c
    ODScrn.c
    ODSpawn.c
    ODStand.c
    ODStat.c
    ODStr.c
    ODSync.c
    ODUtil.c
    ODWCat.c
    ODWin.c
)

if(WIN32)
    list(APPEND OPENDOORS_SOURCES ODFrame.c ODRes.rc)
endif()

set(OPENDOORS_PUBLIC_HEADERS
    ODStat.h
    OpenDoor.h
)

set(OPENDOORS_PRIVATE_HEADERS
    ODCom.h
    ODCore.h
    ODFormat.h
    ODFrame.h
    ODGen.h
    ODInEx.h
    ODInQue.h
    ODKrnl.h
    ODPlat.h
    ODRes.h
    ODSafe.h
    ODScrn.h
    ODStr.h
    ODSwap.h
    ODSync.h
    ODTypes.h
    ODUtil.h
    ODVScrn.h
)

if(NOT OPENDOORS_BUILD_SHARED AND NOT OPENDOORS_BUILD_STATIC)
    message(FATAL_ERROR
        "At least one of OPENDOORS_BUILD_SHARED or OPENDOORS_BUILD_STATIC must be enabled")
endif()

if(OPENDOORS_BUILD_MSVC_STATIC_MT)
    if(NOT MSVC)
        message(FATAL_ERROR
            "OPENDOORS_BUILD_MSVC_STATIC_MT is only supported with MSVC")
    endif()
    if(NOT OPENDOORS_BUILD_STATIC)
        message(FATAL_ERROR
            "OPENDOORS_BUILD_MSVC_STATIC_MT requires OPENDOORS_BUILD_STATIC")
    endif()
endif()

check_include_file(inttypes.h OPENDOORS_HAVE_INTTYPES_H)
check_symbol_exists(vsnprintf "stdio.h" OPENDOORS_HAVE_VSNPRINTF)
if(OPENDOORS_ENABLE_PTHREAD_KERNEL)
    if(WIN32)
        message(FATAL_ERROR
            "OPENDOORS_ENABLE_PTHREAD_KERNEL is only supported on Unix")
    endif()
    find_package(Threads REQUIRED)
endif()

function(opendoors_configure_library target)
    target_include_directories(${target} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    target_compile_features(${target} PUBLIC c_std_99)
    if(OPENDOORS_HAVE_INTTYPES_H)
        target_compile_definitions(${target} PRIVATE HAS_INTTYPES_H)
    endif()
    if(OPENDOORS_HAVE_VSNPRINTF)
        target_compile_definitions(${target} PRIVATE
            OPENDOORS_HAVE_VSNPRINTF=1)
    endif()

    if(OPENDOORS_WARNINGS_AS_ERRORS)
        if(MSVC)
            target_compile_options(${target} PRIVATE /WX)
            target_link_options(${target} PRIVATE /WX)
        else()
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()

    if(APPLE)
        target_compile_definitions(${target} PRIVATE __unix__)
    endif()

    if(OPENDOORS_ENABLE_PTHREAD_KERNEL)
        target_compile_definitions(${target} PRIVATE
            OPENDOORS_ENABLE_PTHREAD_KERNEL=1)
        target_link_libraries(${target} PRIVATE Threads::Threads)
    endif()

    if(WIN32)
        target_link_libraries(${target} PRIVATE
            ws2_32 user32 gdi32 advapi32 shell32 uuid comctl32)
    endif()

    set_target_properties(${target} PROPERTIES
        C_VISIBILITY_PRESET hidden
        POSITION_INDEPENDENT_CODE ON)
endfunction()

set(OPENDOORS_LIBRARY_TARGETS)

if(OPENDOORS_BUILD_SHARED)
    add_library(opendoors_shared SHARED
        ${OPENDOORS_SOURCES}
        ${OPENDOORS_PUBLIC_HEADERS}
        ${OPENDOORS_PRIVATE_HEADERS})
    add_library(OpenDoors::Shared ALIAS opendoors_shared)
    opendoors_configure_library(opendoors_shared)
    set_target_properties(opendoors_shared PROPERTIES EXPORT_NAME Shared)
    list(APPEND OPENDOORS_LIBRARY_TARGETS opendoors_shared)
endif()

if(OPENDOORS_BUILD_STATIC)
    add_library(opendoors_static STATIC
        ${OPENDOORS_SOURCES}
        ${OPENDOORS_PUBLIC_HEADERS}
        ${OPENDOORS_PRIVATE_HEADERS})
    add_library(OpenDoors::Static ALIAS opendoors_static)
    opendoors_configure_library(opendoors_static)
    set_target_properties(opendoors_static PROPERTIES EXPORT_NAME Static)
    list(APPEND OPENDOORS_LIBRARY_TARGETS opendoors_static)
endif()

if(OPENDOORS_BUILD_MSVC_STATIC_MT)
    add_library(opendoors_static_mt STATIC
        ${OPENDOORS_SOURCES}
        ${OPENDOORS_PUBLIC_HEADERS}
        ${OPENDOORS_PRIVATE_HEADERS})
    add_library(OpenDoors::StaticMT ALIAS opendoors_static_mt)
    opendoors_configure_library(opendoors_static_mt)
    set_target_properties(opendoors_static_mt PROPERTIES
        EXPORT_NAME StaticMT
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    list(APPEND OPENDOORS_LIBRARY_TARGETS opendoors_static_mt)
endif()

if(WIN32)
    if(OPENDOORS_BUILD_SHARED)
        set_target_properties(opendoors_shared PROPERTIES
            PREFIX ""
            RUNTIME_OUTPUT_NAME ODoors63
            ARCHIVE_OUTPUT_NAME ODoorW)

        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            if(MSVC)
                target_sources(opendoors_shared PRIVATE OpenDoor.def)
                target_compile_definitions(opendoors_shared PRIVATE
                    OPENDOORS_USE_DEF_EXPORTS)
            elseif(MINGW)
                target_sources(opendoors_shared PRIVATE OpenDoorMinGW.def)
            endif()
        else()
            target_sources(opendoors_shared PRIVATE OpenDoor64.def)
            target_compile_definitions(opendoors_shared PRIVATE
                OPENDOORS_USE_DEF_EXPORTS)
        endif()
        if(MINGW)
            target_link_options(opendoors_shared PRIVATE -static-libgcc)
        endif()
    endif()

    if(OPENDOORS_BUILD_STATIC)
        set_target_properties(opendoors_static PROPERTIES
            ARCHIVE_OUTPUT_NAME ODoors-static)
        target_compile_definitions(opendoors_static PUBLIC OD_WIN32_STATIC)
    endif()
    if(OPENDOORS_BUILD_MSVC_STATIC_MT)
        set_target_properties(opendoors_static_mt PROPERTIES
            ARCHIVE_OUTPUT_NAME ODoors-static-mt)
        target_compile_definitions(opendoors_static_mt PUBLIC OD_WIN32_STATIC)
    endif()
else()
    if(OPENDOORS_BUILD_SHARED)
        set_target_properties(opendoors_shared PROPERTIES
            OUTPUT_NAME ODoors
            VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
    endif()
    if(OPENDOORS_BUILD_STATIC)
        set_target_properties(opendoors_static PROPERTIES OUTPUT_NAME ODoors)
    endif()
endif()

foreach(target IN LISTS OPENDOORS_LIBRARY_TARGETS)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endforeach()

install(TARGETS ${OPENDOORS_LIBRARY_TARGETS}
    EXPORT OpenDoorsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${OPENDOORS_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/OpenDoors)
install(EXPORT OpenDoorsTargets
    FILE OpenDoorsTargets.cmake
    NAMESPACE OpenDoors::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenDoors)

configure_package_config_file(
    cmake/OpenDoorsConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/OpenDoorsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenDoors)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/OpenDoorsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/OpenDoorsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/OpenDoorsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenDoors)

if(OPENDOORS_BUILD_SHARED)
    set(OPENDOORS_DEFAULT_LINK_TARGET OpenDoors::Shared)
else()
    set(OPENDOORS_DEFAULT_LINK_TARGET OpenDoors::Static)
endif()

function(opendoors_add_example name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE ${OPENDOORS_DEFAULT_LINK_TARGET})
    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endfunction()

if(OPENDOORS_BUILD_EXAMPLES)
    opendoors_add_example(ex_chat ex_chat.c)
    opendoors_add_example(ex_diag ex_diag.c)
    opendoors_add_example(ex_hello ex_hello.c)
    opendoors_add_example(ex_music ex_music.c)

    if(WIN32)
        set_target_properties(ex_chat ex_hello ex_music PROPERTIES
            WIN32_EXECUTABLE TRUE)
    endif()

    set(OPENDOORS_HAVE_XPDEV FALSE)
    if(TARGET xpdev)
        set(OPENDOORS_HAVE_XPDEV TRUE)
        message(STATUS "Using the existing xpdev target")
    else()
        set(opendoors_xpdev_source "")
        if(OPENDOORS_XPDEV_DIR)
            get_filename_component(opendoors_xpdev_source
                "${OPENDOORS_XPDEV_DIR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
            if(NOT EXISTS "${opendoors_xpdev_source}/CMakeLists.txt")
                message(FATAL_ERROR
                    "OPENDOORS_XPDEV_DIR does not contain CMakeLists.txt: "
                    "${opendoors_xpdev_source}")
            endif()
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../xpdev/CMakeLists.txt")
            get_filename_component(opendoors_xpdev_source
                "${CMAKE_CURRENT_SOURCE_DIR}/../xpdev" ABSOLUTE)
        endif()

        if(opendoors_xpdev_source)
            # These examples only need xpdev's file and timer wrappers.
            set(WITHOUT_OSS ON)
            set(WITHOUT_SDL_AUDIO ON)
            set(WITHOUT_ALSA ON)
            set(WITHOUT_PORTAUDIO ON)
            set(WITHOUT_PULSEAUDIO ON)
            set(WITHOUT_PIPEWIRE ON)
            set(WITHOUT_COREAUDIO ON)
            add_subdirectory("${opendoors_xpdev_source}"
                "${CMAKE_BINARY_DIR}/xpdev" EXCLUDE_FROM_ALL)
            if(NOT TARGET xpdev)
                message(FATAL_ERROR
                    "The selected xpdev project did not define an xpdev target")
            endif()
            set(OPENDOORS_HAVE_XPDEV TRUE)
            message(STATUS "Using xpdev from ${opendoors_xpdev_source}")
        endif()
    endif()

    if(OPENDOORS_HAVE_XPDEV)
        opendoors_add_example(ex_ski ex_ski.c)
        opendoors_add_example(ex_vote ex_vote.c)
        target_link_libraries(ex_ski PRIVATE xpdev)
        target_link_libraries(ex_vote PRIVATE xpdev)
        target_compile_definitions(ex_vote PRIVATE MULTINODE_AWARE)
        if(WIN32)
            set_target_properties(ex_ski ex_vote PROPERTIES
                WIN32_EXECUTABLE TRUE)
        endif()
    else()
        message(STATUS
            "xpdev was not found at ../xpdev; skipping ex_ski and ex_vote")
    endif()
endif()

if(BUILD_TESTING)
    # These white-box tests exercise intentionally hidden implementation
    # symbols, so they require the static library.
    if(OPENDOORS_BUILD_STATIC)
        if(OPENDOORS_ENABLE_PTHREAD_KERNEL)
            add_executable(opendoors_pthread_kernel_test
                tests/pthread_kernel_test.c)
            target_link_libraries(opendoors_pthread_kernel_test
                PRIVATE OpenDoors::Static Threads::Threads)
            target_compile_definitions(opendoors_pthread_kernel_test PRIVATE
                OPENDOORS_ENABLE_PTHREAD_KERNEL=1)
            set_target_properties(opendoors_pthread_kernel_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME pthread_kernel
                COMMAND opendoors_pthread_kernel_test)
        endif()

        add_executable(opendoors_safety_test tests/safety_test.c)
        target_link_libraries(opendoors_safety_test PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_safety_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME safety COMMAND opendoors_safety_test)

        add_executable(opendoors_command_line_values_test
            tests/command_line_values_test.c)
        target_link_libraries(opendoors_command_line_values_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_command_line_values_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME command_line_values
            COMMAND opendoors_command_line_values_test)

        add_executable(opendoors_exitinfo_traffic_test
            tests/exitinfo_traffic_test.c)
        target_link_libraries(opendoors_exitinfo_traffic_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_exitinfo_traffic_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME exitinfo_traffic COMMAND opendoors_exitinfo_traffic_test)

        add_executable(opendoors_draw_box_bounds_test
            tests/draw_box_bounds_test.c)
        target_link_libraries(opendoors_draw_box_bounds_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_draw_box_bounds_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME draw_box_bounds COMMAND opendoors_draw_box_bounds_test)

        add_executable(opendoors_window_title_test
            tests/window_title_test.c)
        target_link_libraries(opendoors_window_title_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_window_title_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME window_title COMMAND opendoors_window_title_test)

        add_executable(opendoors_get_key_poll_test
            tests/get_key_poll_test.c)
        target_link_libraries(opendoors_get_key_poll_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_get_key_poll_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME get_key_poll COMMAND opendoors_get_key_poll_test)
        set_tests_properties(get_key_poll PROPERTIES TIMEOUT 5)

        add_executable(opendoors_input_str_range_test
            tests/input_str_range_test.c)
        target_link_libraries(opendoors_input_str_range_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_input_str_range_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME input_str_range COMMAND opendoors_input_str_range_test)

        add_executable(opendoors_edit_rectangle_test
            tests/edit_rectangle_test.c)
        target_link_libraries(opendoors_edit_rectangle_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_edit_rectangle_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME edit_rectangle COMMAND opendoors_edit_rectangle_test)

        add_executable(opendoors_edit_format_test
            tests/edit_format_test.c)
        target_link_libraries(opendoors_edit_format_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_edit_format_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME edit_format COMMAND opendoors_edit_format_test)

        add_executable(opendoors_display_path_test
            tests/display_path_test.c)
        target_link_libraries(opendoors_display_path_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_display_path_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME display_path COMMAND opendoors_display_path_test)

        add_executable(opendoors_ra_substitution_test
            tests/ra_substitution_test.c)
        target_link_libraries(opendoors_ra_substitution_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_ra_substitution_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME ra_substitution COMMAND opendoors_ra_substitution_test)

        add_executable(opendoors_virtual_screen_test
            tests/virtual_screen_test.c)
        target_link_libraries(opendoors_virtual_screen_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_virtual_screen_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME virtual_screen COMMAND opendoors_virtual_screen_test)

        add_executable(opendoors_multiline_format_test
            tests/multiline_format_test.c)
        target_link_libraries(opendoors_multiline_format_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_multiline_format_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME multiline_format COMMAND opendoors_multiline_format_test)

        add_executable(opendoors_multiline_growth_test
            tests/multiline_growth_test.c)
        target_link_libraries(opendoors_multiline_growth_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_multiline_growth_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME multiline_growth COMMAND opendoors_multiline_growth_test)

        add_executable(opendoors_multiline_bounds_test
            tests/multiline_bounds_test.c)
        target_link_libraries(opendoors_multiline_bounds_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_multiline_bounds_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME multiline_bounds COMMAND opendoors_multiline_bounds_test)

        add_executable(opendoors_popup_bounds_test
            tests/popup_bounds_test.c)
        target_link_libraries(opendoors_popup_bounds_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_popup_bounds_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME popup_bounds COMMAND opendoors_popup_bounds_test)

        add_executable(opendoors_printf_color_test
            tests/printf_color_test.c)
        target_link_libraries(opendoors_printf_color_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_printf_color_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME printf_color COMMAND opendoors_printf_color_test)

        add_executable(opendoors_list_line_test
            tests/list_line_test.c)
        target_link_libraries(opendoors_list_line_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_list_line_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME list_line COMMAND opendoors_list_line_test)

        add_executable(opendoors_dir_attributes_test
            tests/dir_attributes_test.c)
        target_link_libraries(opendoors_dir_attributes_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_dir_attributes_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME dir_attributes COMMAND opendoors_dir_attributes_test)

        add_executable(opendoors_log_open_test
            tests/log_open_test.c)
        target_link_libraries(opendoors_log_open_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_log_open_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME log_open COMMAND opendoors_log_open_test)

        if(UNIX)
            add_executable(opendoors_config_callback_test
                tests/config_callback_test.c)
            target_link_libraries(opendoors_config_callback_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_config_callback_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME config_callback COMMAND opendoors_config_callback_test)
            set_tests_properties(config_callback PROPERTIES TIMEOUT 10)

            add_executable(opendoors_config_paths_test
                tests/config_paths_test.c)
            target_link_libraries(opendoors_config_paths_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_config_paths_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME config_paths COMMAND opendoors_config_paths_test)
            set_tests_properties(config_paths PROPERTIES TIMEOUT 10)

            add_executable(opendoors_config_directory_failure_test
                tests/config_directory_failure_test.c)
            target_link_libraries(opendoors_config_directory_failure_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_config_directory_failure_test
                PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME config_directory_failure
                COMMAND opendoors_config_directory_failure_test)
            set_tests_properties(config_directory_failure PROPERTIES
                TIMEOUT 10)

            add_executable(opendoors_custom_drop_lines_test
                tests/custom_drop_lines_test.c)
            target_link_libraries(opendoors_custom_drop_lines_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_custom_drop_lines_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME custom_drop_lines
                COMMAND opendoors_custom_drop_lines_test)
            set_tests_properties(custom_drop_lines PROPERTIES TIMEOUT 10)

            add_executable(opendoors_config_reinit_test
                tests/config_reinit_test.c)
            target_link_libraries(opendoors_config_reinit_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_config_reinit_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME config_reinit COMMAND opendoors_config_reinit_test)
            set_tests_properties(config_reinit PROPERTIES TIMEOUT 10)

            add_executable(opendoors_default_config_test
                tests/default_config_test.c)
            target_link_libraries(opendoors_default_config_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_default_config_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME default_config COMMAND opendoors_default_config_test)
            set_tests_properties(default_config PROPERTIES TIMEOUT 10)

            add_executable(opendoors_sfdoors_login_time_test
                tests/sfdoors_login_time_test.c)
            target_link_libraries(opendoors_sfdoors_login_time_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_sfdoors_login_time_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME sfdoors_login_time
                COMMAND opendoors_sfdoors_login_time_test)
            set_tests_properties(sfdoors_login_time PROPERTIES TIMEOUT 10)
        endif()

        add_executable(opendoors_text_drop_writer_test
            tests/text_drop_writer_test.c)
        target_link_libraries(opendoors_text_drop_writer_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_text_drop_writer_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME text_drop_writer COMMAND opendoors_text_drop_writer_test)

        if(WIN32)
            add_executable(opendoors_windows_dir_test
                tests/windows_dir_test.c)
            target_link_libraries(opendoors_windows_dir_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_windows_dir_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME windows_dir COMMAND opendoors_windows_dir_test)
            set_tests_properties(windows_dir PROPERTIES TIMEOUT 10)

            add_executable(opendoors_windows_chat_failure_test
                tests/windows_chat_failure_test.c)
            target_link_libraries(opendoors_windows_chat_failure_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_windows_chat_failure_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME windows_chat_failure
                COMMAND opendoors_windows_chat_failure_test)
        endif()

        add_executable(opendoors_list_files_test
            tests/list_files_test.c)
        target_link_libraries(opendoors_list_files_test
            PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_list_files_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME list_files COMMAND opendoors_list_files_test)

        if(UNIX)
            add_executable(opendoors_dir_basename_test
                tests/dir_basename_test.c)
            target_link_libraries(opendoors_dir_basename_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_dir_basename_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME dir_basename COMMAND opendoors_dir_basename_test)

            add_executable(opendoors_dir_stat_test
                tests/dir_stat_test.c)
            target_link_libraries(opendoors_dir_stat_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_dir_stat_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME dir_stat COMMAND opendoors_dir_stat_test)

            add_executable(opendoors_dir_file_type_test
                tests/dir_file_type_test.c)
            target_link_libraries(opendoors_dir_file_type_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_dir_file_type_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME dir_file_type COMMAND opendoors_dir_file_type_test)

            add_executable(opendoors_unix_spawn_test
                tests/unix_spawn_test.c)
            target_link_libraries(opendoors_unix_spawn_test
                PRIVATE OpenDoors::Static)
            set_target_properties(opendoors_unix_spawn_test PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
            add_test(NAME unix_spawn COMMAND opendoors_unix_spawn_test)
            set_tests_properties(unix_spawn PROPERTIES TIMEOUT 10)
        endif()
    endif()

    if(NOT WIN32)
        add_executable(opendoors_fallback_format_test
            tests/format_fallback_test.c ODFmtFB.c)
        target_include_directories(opendoors_fallback_format_test PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}")
        target_compile_options(opendoors_fallback_format_test PRIVATE
            -std=c90 -pedantic-errors -Werror)
        target_link_libraries(opendoors_fallback_format_test PRIVATE m)
        add_test(NAME fallback_format COMMAND opendoors_fallback_format_test)
    endif()

    if(OPENDOORS_BUILD_SHARED)
        add_executable(opendoors_shared_smoke tests/link_smoke.c)
        target_link_libraries(opendoors_shared_smoke PRIVATE OpenDoors::Shared)
        set_target_properties(opendoors_shared_smoke PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME shared_link_smoke COMMAND opendoors_shared_smoke)
    endif()

    if(OPENDOORS_BUILD_STATIC)
        add_executable(opendoors_static_smoke tests/link_smoke.c)
        target_link_libraries(opendoors_static_smoke PRIVATE OpenDoors::Static)
        set_target_properties(opendoors_static_smoke PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        add_test(NAME static_link_smoke COMMAND opendoors_static_smoke)
    endif()

    set(OPENDOORS_ACCEPTANCE_LIBRARY "${OPENDOORS_DEFAULT_LINK_TARGET}")
    add_subdirectory(tests/acceptance)
endif()
