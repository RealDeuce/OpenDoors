set(test_prefix "${OPENDOORS_BINARY_DIR}/pkg-config-install")
file(REMOVE_RECURSE "${test_prefix}")

set(install_command
    "${CMAKE_COMMAND}" --install "${OPENDOORS_BINARY_DIR}"
    --prefix "${test_prefix}")
if(OPENDOORS_TEST_CONFIG)
    list(APPEND install_command --config "${OPENDOORS_TEST_CONFIG}")
endif()
execute_process(
    COMMAND ${install_command}
    RESULT_VARIABLE install_result
    OUTPUT_VARIABLE install_output
    ERROR_VARIABLE install_error)
if(NOT install_result EQUAL 0)
    message(FATAL_ERROR
        "Installing OpenDoors failed:\n${install_output}${install_error}")
endif()

set(pkg_config_dir
    "${test_prefix}/${OPENDOORS_INSTALL_LIBDIR}/pkgconfig")
set(expected_packages opendoors)
if(OPENDOORS_SHARED)
    list(APPEND expected_packages opendoors-shared)
endif()
if(OPENDOORS_STATIC)
    list(APPEND expected_packages opendoors-static)
endif()
if(OPENDOORS_STATIC_MT)
    list(APPEND expected_packages opendoors-static-mt)
endif()
foreach(package IN LISTS expected_packages)
    set(pkg_config_file "${pkg_config_dir}/${package}.pc")
    if(NOT EXISTS "${pkg_config_file}")
        message(FATAL_ERROR "The install did not generate ${pkg_config_file}")
    endif()

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env
            "PKG_CONFIG_PATH=${pkg_config_dir}"
            "PKG_CONFIG_LIBDIR=${pkg_config_dir}"
            "${OPENDOORS_PKG_CONFIG}" --modversion "${package}"
        RESULT_VARIABLE version_result
        OUTPUT_VARIABLE installed_version
        ERROR_VARIABLE version_error
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT version_result EQUAL 0)
        message(FATAL_ERROR
            "pkg-config rejected ${package}.pc: ${version_error}")
    endif()
    if(NOT installed_version STREQUAL OPENDOORS_VERSION)
        message(FATAL_ERROR
            "pkg-config reported version '${installed_version}' for ${package}, expected '${OPENDOORS_VERSION}'")
    endif()
endforeach()

if(OPENDOORS_SHARED)
    set(link_package opendoors-shared)
else()
    set(link_package opendoors-static)
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "PKG_CONFIG_PATH=${pkg_config_dir}"
        "PKG_CONFIG_LIBDIR=${pkg_config_dir}"
        "${OPENDOORS_PKG_CONFIG}" --cflags --libs "${link_package}"
    RESULT_VARIABLE flags_result
    OUTPUT_VARIABLE pkg_config_flags
    ERROR_VARIABLE flags_error
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT flags_result EQUAL 0)
    message(FATAL_ERROR "pkg-config could not read opendoors flags: ${flags_error}")
endif()
separate_arguments(pkg_config_flags NATIVE_COMMAND "${pkg_config_flags}")

execute_process(
    COMMAND "${OPENDOORS_C_COMPILER}" "${OPENDOORS_SOURCE}"
        -o "${test_prefix}/pkg-config-link-smoke" ${pkg_config_flags}
    RESULT_VARIABLE compile_result
    OUTPUT_VARIABLE compile_output
    ERROR_VARIABLE compile_error)
if(NOT compile_result EQUAL 0)
    message(FATAL_ERROR
        "Compiling with pkg-config flags failed:\n${compile_output}${compile_error}")
endif()
