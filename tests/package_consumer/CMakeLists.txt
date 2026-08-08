cmake_minimum_required(VERSION 3.20)

project(OpenDoorsPackageConsumer LANGUAGES C)

include(CTest)

set(OPENDOORS_VARIANT "Shared" CACHE STRING
    "Installed OpenDoors target to test (Shared or Static)")
set_property(CACHE OPENDOORS_VARIANT PROPERTY STRINGS Shared Static)

if(NOT OPENDOORS_VARIANT STREQUAL "Shared" AND
        NOT OPENDOORS_VARIANT STREQUAL "Static")
    message(FATAL_ERROR "OPENDOORS_VARIANT must be Shared or Static")
endif()

find_package(OpenDoors 6.30 CONFIG REQUIRED COMPONENTS ${OPENDOORS_VARIANT})

add_executable(opendoors_package_consumer ../link_smoke.c)
target_link_libraries(opendoors_package_consumer
    PRIVATE OpenDoors::${OPENDOORS_VARIANT})
add_test(NAME package_consumer COMMAND opendoors_package_consumer)
