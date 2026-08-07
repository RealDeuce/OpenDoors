cmake_minimum_required(VERSION 3.20)

project(OpenDoorsDOS LANGUAGES C)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "DOS")
    message(FATAL_ERROR
        "This project is only for DOS cross-builds; set CMAKE_SYSTEM_NAME=DOS")
endif()

if(NOT CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    message(FATAL_ERROR "The DOS build requires Open Watcom")
endif()

set(OPENDOORS_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(OPENDOORS_DOS_SOURCES
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
    ODScrn.c
    ODSpawn.c
    ODStand.c
    ODStat.c
    ODUtil.c
    ODWCat.c
    ODWin.c
)
list(TRANSFORM OPENDOORS_DOS_SOURCES PREPEND "${OPENDOORS_ROOT}/")

find_program(OPENDOORS_WASM wasm REQUIRED)
set(OPENDOORS_SWAP_OBJECT "${CMAKE_CURRENT_BINARY_DIR}/ODSwap.obj")
add_custom_command(
    OUTPUT "${OPENDOORS_SWAP_OBJECT}"
    COMMAND "${OPENDOORS_WASM}" -q -dLCODE -dLDATA
        "-fo=${OPENDOORS_SWAP_OBJECT}" "${OPENDOORS_ROOT}/ODSwap.asm"
    DEPENDS "${OPENDOORS_ROOT}/ODSwap.asm"
    VERBATIM)
set_source_files_properties("${OPENDOORS_SWAP_OBJECT}" PROPERTIES
    EXTERNAL_OBJECT TRUE
    GENERATED TRUE)

add_library(opendoors_dos STATIC
    ${OPENDOORS_DOS_SOURCES}
    "${OPENDOORS_SWAP_OBJECT}")
target_include_directories(opendoors_dos PUBLIC "${OPENDOORS_ROOT}")
target_compile_options(opendoors_dos PRIVATE -ml -zq -za99)
set_target_properties(opendoors_dos PROPERTIES
    OUTPUT_NAME ODoorl
    PREFIX "")

add_executable(dos_link_smoke "${OPENDOORS_ROOT}/tests/link_smoke.c")
target_compile_options(dos_link_smoke PRIVATE -ml -zq -za99)
target_link_libraries(dos_link_smoke PRIVATE opendoors_dos)
