cmake_minimum_required(VERSION 3.20)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" OPENDOORS_VERSION
    LIMIT_COUNT 1)
project(OpenDoors VERSION "${OPENDOORS_VERSION}" LANGUAGES C)

set(_opendoors_testing_explicit OFF)
if(DEFINED BUILD_TESTING)
    set(_opendoors_testing_explicit ON)
endif()
if(NOT _opendoors_testing_explicit)
    find_package(Python3 QUIET COMPONENTS Interpreter)
    if(NOT Python3_Interpreter_FOUND)
        set(BUILD_TESTING OFF CACHE BOOL "Build the testing tree.")
    endif()
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()

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

add_subdirectory(src)

if(OPENDOORS_BUILD_SHARED)
    set(OPENDOORS_DEFAULT_LINK_TARGET OpenDoors::Shared)
else()
    set(OPENDOORS_DEFAULT_LINK_TARGET OpenDoors::Static)
endif()

if(OPENDOORS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

configure_package_config_file(
    cmake/OpenDoorsConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/OpenDoorsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenDoors)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/OpenDoorsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion)

if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
    set(OPENDOORS_PC_PREFIX "${CMAKE_INSTALL_PREFIX}")
    set(OPENDOORS_PC_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
else()
    file(RELATIVE_PATH OPENDOORS_PC_PREFIX_FROM_PCFILEDIR
        "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig"
        "${CMAKE_INSTALL_PREFIX}")
    set(OPENDOORS_PC_PREFIX
        "\${pcfiledir}/${OPENDOORS_PC_PREFIX_FROM_PCFILEDIR}")
    set(OPENDOORS_PC_LIBDIR "\${prefix}/${CMAKE_INSTALL_LIBDIR}")
endif()
if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
    set(OPENDOORS_PC_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
else()
    set(OPENDOORS_PC_INCLUDEDIR
        "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()

function(opendoors_configure_pc filename name description libs cflags)
    set(OPENDOORS_PC_NAME "${name}")
    set(OPENDOORS_PC_DESCRIPTION "${description}")
    set(OPENDOORS_PC_LIBS "-L\${libdir} ${libs}")
    set(OPENDOORS_PC_CFLAGS "-I\${includedir}")
    if(cflags)
        string(APPEND OPENDOORS_PC_CFLAGS " ${cflags}")
    endif()
    configure_file(cmake/opendoors.pc.in "${filename}" @ONLY)
    set(OPENDOORS_PC_FILES ${OPENDOORS_PC_FILES}
        "${CMAKE_CURRENT_BINARY_DIR}/${filename}" PARENT_SCOPE)
endfunction()

if(WIN32)
    set(OPENDOORS_PC_SHARED_LIBS "-lODoorW")
    set(OPENDOORS_PC_STATIC_LIBS
        "-lODoors-static -lws2_32 -luser32 -lgdi32 -ladvapi32 -lshell32 -luuid -lcomctl32")
    set(OPENDOORS_PC_STATIC_MT_LIBS
        "-lODoors-static-mt -lws2_32 -luser32 -lgdi32 -ladvapi32 -lshell32 -luuid -lcomctl32")
    set(OPENDOORS_PC_STATIC_CFLAGS "-DOD_WIN32_STATIC")
else()
    set(OPENDOORS_PC_SHARED_LIBS "-lODoors")
    set(OPENDOORS_PC_STATIC_LIBS "-lODoors")
    set(OPENDOORS_PC_STATIC_MT_LIBS "-lODoors")
    set(OPENDOORS_PC_STATIC_CFLAGS "")
endif()

set(OPENDOORS_PC_FILES)
if(OPENDOORS_BUILD_SHARED)
    opendoors_configure_pc(opendoors.pc OpenDoors
        "OpenDoors shared library" "${OPENDOORS_PC_SHARED_LIBS}" "")
    opendoors_configure_pc(opendoors-shared.pc OpenDoors-Shared
        "OpenDoors shared library" "${OPENDOORS_PC_SHARED_LIBS}" "")
else()
    opendoors_configure_pc(opendoors.pc OpenDoors
        "OpenDoors static library" "${OPENDOORS_PC_STATIC_LIBS}"
        "${OPENDOORS_PC_STATIC_CFLAGS}")
endif()
if(OPENDOORS_BUILD_STATIC)
    opendoors_configure_pc(opendoors-static.pc OpenDoors-Static
        "OpenDoors static library" "${OPENDOORS_PC_STATIC_LIBS}"
        "${OPENDOORS_PC_STATIC_CFLAGS}")
endif()
if(OPENDOORS_BUILD_MSVC_STATIC_MT)
    opendoors_configure_pc(opendoors-static-mt.pc OpenDoors-StaticMT
        "OpenDoors static library using the MSVC static runtime"
        "${OPENDOORS_PC_STATIC_MT_LIBS}" "${OPENDOORS_PC_STATIC_CFLAGS}")
endif()
install(FILES ${OPENDOORS_PC_FILES}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/OpenDoorsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/OpenDoorsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenDoors)
install(FILES LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/OpenDoors)
