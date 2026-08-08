cmake_minimum_required(VERSION 3.20)

project(OpenDoorsExamples LANGUAGES C)

set(OPENDOORS_EXAMPLE_VARIANT "Shared" CACHE STRING
    "Installed OpenDoors target to use (Shared, Static, or StaticMT)")
set_property(CACHE OPENDOORS_EXAMPLE_VARIANT PROPERTY STRINGS
    Shared Static StaticMT)
set(OPENDOORS_XPDEV_DIR "" CACHE PATH
    "Path to xpdev; when empty, only ../xpdev is checked")

if(NOT OPENDOORS_EXAMPLE_VARIANT STREQUAL "Shared" AND
        NOT OPENDOORS_EXAMPLE_VARIANT STREQUAL "Static" AND
        NOT OPENDOORS_EXAMPLE_VARIANT STREQUAL "StaticMT")
    message(FATAL_ERROR
        "OPENDOORS_EXAMPLE_VARIANT must be Shared, Static, or StaticMT")
endif()

find_package(OpenDoors 6.3 CONFIG REQUIRED
    COMPONENTS ${OPENDOORS_EXAMPLE_VARIANT})

function(opendoors_example name)
    add_executable(${name} ${name}.c)
    target_link_libraries(${name}
        PRIVATE OpenDoors::${OPENDOORS_EXAMPLE_VARIANT})
endfunction()

foreach(example IN ITEMS ex_chat ex_diag ex_hello ex_music)
    opendoors_example(${example})
endforeach()

if(WIN32)
    set_target_properties(ex_chat ex_hello ex_music PROPERTIES
        WIN32_EXECUTABLE TRUE)
endif()

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
        message(FATAL_ERROR "The selected xpdev project did not define xpdev")
    endif()
    foreach(example IN ITEMS ex_ski ex_vote)
        opendoors_example(${example})
        target_link_libraries(${example} PRIVATE xpdev)
    endforeach()
    target_compile_definitions(ex_vote PRIVATE MULTINODE_AWARE)
    if(WIN32)
        set_target_properties(ex_ski ex_vote PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
else()
    message(STATUS "xpdev was not found; skipping ex_ski and ex_vote")
endif()
