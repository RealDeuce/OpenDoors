cmake_minimum_required(VERSION 3.20)

project(OpenDoorsExamples LANGUAGES C)

set(OPENDOORS_EXAMPLE_VARIANT "Shared" CACHE STRING
    "Installed OpenDoors target to use (Shared, Static, or StaticMT)")
set_property(CACHE OPENDOORS_EXAMPLE_VARIANT PROPERTY STRINGS
    Shared Static StaticMT)
if(NOT OPENDOORS_EXAMPLE_VARIANT STREQUAL "Shared" AND
        NOT OPENDOORS_EXAMPLE_VARIANT STREQUAL "Static" AND
        NOT OPENDOORS_EXAMPLE_VARIANT STREQUAL "StaticMT")
    message(FATAL_ERROR
        "OPENDOORS_EXAMPLE_VARIANT must be Shared, Static, or StaticMT")
endif()

find_package(OpenDoors 6.3 CONFIG REQUIRED
    COMPONENTS ${OPENDOORS_EXAMPLE_VARIANT})

function(opendoors_example name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name}
        PRIVATE OpenDoors::${OPENDOORS_EXAMPLE_VARIANT})
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/data/door.cfg"
            "$<TARGET_FILE_DIR:${name}>/door.cfg"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/data/DORINFO1.DEF"
            "$<TARGET_FILE_DIR:${name}>/DORINFO1.DEF")
endfunction()

foreach(example IN ITEMS ex_chat ex_diag ex_hello ex_music ex_ski)
    opendoors_example(${example} ${example}.c)
endforeach()

opendoors_example(ex_vote ex_vote.c ex_votio.c)
target_compile_definitions(ex_vote PRIVATE MULTINODE_AWARE)

if(WIN32)
    set_target_properties(ex_chat ex_hello ex_music ex_ski ex_vote PROPERTIES
        WIN32_EXECUTABLE TRUE)
endif()
