cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(OpenDoorsAcceptance LANGUAGES C CXX)
    include(CTest)
    set(OPENDOORS_VARIANT "Shared" CACHE STRING
        "Installed OpenDoors target to test")
    find_package(OpenDoors 6.3 CONFIG REQUIRED COMPONENTS ${OPENDOORS_VARIANT})
    set(OPENDOORS_ACCEPTANCE_LIBRARY "OpenDoors::${OPENDOORS_VARIANT}")
elseif(NOT DEFINED OPENDOORS_ACCEPTANCE_LIBRARY)
    if(TARGET OpenDoors::Static)
        set(OPENDOORS_ACCEPTANCE_LIBRARY OpenDoors::Static)
    else()
        set(OPENDOORS_ACCEPTANCE_LIBRARY OpenDoors::Shared)
    endif()
endif()

option(OPENDOORS_ACCEPTANCE_EXTENDED
    "Run socket-driven interactive acceptance scenarios" OFF)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

function(opendoors_acceptance_library_path name)
    if(WIN32)
        if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.22)
            set_tests_properties("${name}" PROPERTIES
                ENVIRONMENT_MODIFICATION
                    "PATH=path_list_prepend:$<TARGET_FILE_DIR:${OPENDOORS_ACCEPTANCE_LIBRARY}>")
        else()
            set_tests_properties("${name}" PROPERTIES
                ENVIRONMENT
                    "PATH=$<TARGET_FILE_DIR:${OPENDOORS_ACCEPTANCE_LIBRARY}>")
        endif()
    endif()
endfunction()

function(opendoors_acceptance_test name source)
    add_executable("opendoors_acceptance_${name}" "${source}")
    target_link_libraries("opendoors_acceptance_${name}"
        PRIVATE "${OPENDOORS_ACCEPTANCE_LIBRARY}")
    target_include_directories("opendoors_acceptance_${name}"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    set_target_properties("opendoors_acceptance_${name}" PROPERTIES
        C_STANDARD 90
        C_STANDARD_REQUIRED TRUE
        C_EXTENSIONS TRUE)
    add_test(NAME "acceptance.${name}"
        COMMAND "opendoors_acceptance_${name}" ${ARGN})
    set_tests_properties("acceptance.${name}" PROPERTIES TIMEOUT 20)
    opendoors_acceptance_library_path("acceptance.${name}")
endfunction()

opendoors_acceptance_test(public_surface public_surface_test.c)
opendoors_acceptance_test(lifecycle lifecycle_test.c)
opendoors_acceptance_test(lifecycle_init_exit lifecycle_init_exit_test.c)
opendoors_acceptance_test(command_line command_line_test.c)
opendoors_acceptance_test(screen screen_test.c)
if(WIN32)
    target_link_libraries(opendoors_acceptance_screen PRIVATE ws2_32)
    opendoors_acceptance_test(windows_screen windows_screen_test.c)
    target_link_libraries(opendoors_acceptance_windows_screen PRIVATE ws2_32)
endif()
opendoors_acceptance_test(components components_test.c)

add_executable(opendoors_acceptance_dropfiles dropfiles_test.c)
target_link_libraries(opendoors_acceptance_dropfiles
    PRIVATE "${OPENDOORS_ACCEPTANCE_LIBRARY}")
target_include_directories(opendoors_acceptance_dropfiles
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(opendoors_acceptance_dropfiles PROPERTIES
    C_STANDARD 90
    C_STANDARD_REQUIRED TRUE
    C_EXTENSIONS TRUE)
foreach(format IN ITEMS dorinfo chain sfdoors doorway gap wildcat callinfo
        tribbs door32 custom exitinfo qbbs275 ra1 ra2)
    add_test(NAME "acceptance.dropfiles.${format}"
        COMMAND opendoors_acceptance_dropfiles "${format}")
    set_tests_properties("acceptance.dropfiles.${format}" PROPERTIES
        TIMEOUT 20 RESOURCE_LOCK opendoors-dropfiles)
    opendoors_acceptance_library_path("acceptance.dropfiles.${format}")
endforeach()

add_executable(opendoors_acceptance_spawn_child spawn_child.c)
opendoors_acceptance_test(spawn spawn_test.c
    "$<TARGET_FILE:opendoors_acceptance_spawn_child>")

if(NOT CMAKE_CXX_COMPILER_LOADED)
    enable_language(CXX)
endif()
add_library(opendoors_acceptance_cpp_compile OBJECT cpp_surface_test.cpp)
target_link_libraries(opendoors_acceptance_cpp_compile
    PRIVATE "${OPENDOORS_ACCEPTANCE_LIBRARY}")

add_test(NAME acceptance.contract_manifest
    COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/check_public_contract.py")
set_tests_properties(acceptance.contract_manifest PROPERTIES
    ENVIRONMENT "PYTHONDONTWRITEBYTECODE=1")
add_test(NAME acceptance.symbols
    COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/check_public_symbols.py"
        "$<TARGET_FILE:${OPENDOORS_ACCEPTANCE_LIBRARY}>"
        --linker "${CMAKE_LINKER}")

if(OPENDOORS_ACCEPTANCE_EXTENDED)
    add_executable(opendoors_acceptance_socket_door socket_door.c)
    target_link_libraries(opendoors_acceptance_socket_door
        PRIVATE "${OPENDOORS_ACCEPTANCE_LIBRARY}")
    if(WIN32)
        target_link_libraries(opendoors_acceptance_socket_door PRIVATE ws2_32)
    endif()
    target_include_directories(opendoors_acceptance_socket_door
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    set_target_properties(opendoors_acceptance_socket_door PROPERTIES
        C_STANDARD 90
        C_STANDARD_REQUIRED TRUE
        C_EXTENSIONS TRUE)
    set(_opendoors_socket_launcher)
    foreach(_opendoors_launcher_arg IN LISTS CMAKE_CROSSCOMPILING_EMULATOR)
        list(APPEND _opendoors_socket_launcher
            --launcher "${_opendoors_launcher_arg}")
    endforeach()
    if(WIN32)
        list(APPEND _opendoors_socket_launcher --tcp)
    endif()
    add_test(NAME acceptance.socket
        COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/socket_controller.py"
            ${_opendoors_socket_launcher}
            "$<TARGET_FILE:opendoors_acceptance_socket_door>")
    set_tests_properties(acceptance.socket PROPERTIES
        TIMEOUT 30 LABELS "acceptance-extended")
    opendoors_acceptance_library_path(acceptance.socket)

    add_executable(opendoors_acceptance_input_progress
        input_progress_door.c)
    target_link_libraries(opendoors_acceptance_input_progress
        PRIVATE "${OPENDOORS_ACCEPTANCE_LIBRARY}")
    if(WIN32)
        target_link_libraries(opendoors_acceptance_input_progress PRIVATE ws2_32)
    endif()
    target_include_directories(opendoors_acceptance_input_progress
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    set_target_properties(opendoors_acceptance_input_progress PROPERTIES
        C_STANDARD 90
        C_STANDARD_REQUIRED TRUE
        C_EXTENSIONS TRUE)
    set(_opendoors_input_progress_launcher)
    foreach(_opendoors_launcher_arg IN LISTS CMAKE_CROSSCOMPILING_EMULATOR)
        list(APPEND _opendoors_input_progress_launcher
            --launcher "${_opendoors_launcher_arg}")
    endforeach()
    add_test(NAME acceptance.input_progress
        COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/input_progress_controller.py"
            ${_opendoors_input_progress_launcher}
            "$<TARGET_FILE:opendoors_acceptance_input_progress>")
    set_tests_properties(acceptance.input_progress PROPERTIES
        TIMEOUT 25 LABELS "acceptance-extended")
    opendoors_acceptance_library_path(acceptance.input_progress)
endif()
