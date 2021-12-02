cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Zydis VERSION 1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# =============================================================================================== #
# Overridable options                                                                             #
# =============================================================================================== #

# Features
option(ZYDIS_MINIMAL_MODE
    "Enable minimal mode (forces ZYDIS_DECODER_MODE_MINIMAL runtime option)"
    OFF)
option(ZYDIS_FEATURE_DECODER
    "Enable instruction decoding functionality"
    ON)
option(ZYDIS_FEATURE_ENCODER
    "Enable instruction encoding functionality"
    ON)
option(ZYDIS_FEATURE_FORMATTER
    "Enable instruction formatting functionality"
    ON)
option(ZYDIS_FEATURE_AVX512
    "Enable support for AVX-512 instructions"
    ON)
option(ZYDIS_FEATURE_KNC
    "Enable support for KNC instructions"
    ON)

# Build configuration
option(ZYDIS_BUILD_SHARED_LIB
    "Build shared library"
    OFF)
option(ZYDIS_BUILD_EXAMPLES
    "Build examples"
    ON)
option(ZYDIS_BUILD_TOOLS
    "Build tools"
    ON)
option(ZYDIS_BUILD_MAN
    "Build manpages for the tools (requires Ronn-NG)"
    OFF)
option(ZYDIS_FUZZ_AFL_FAST
    "Enables AFL persistent mode and reduces prints in ZydisFuzzIn"
    OFF)
option(ZYDIS_LIBFUZZER
    "Enables LLVM libfuzzer mode and reduces prints in ZydisFuzzIn"
    OFF)
option(ZYDIS_SYSTEM_ZYCORE
    "Use system Zycore library"
    OFF)
set(ZYDIS_ZYCORE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/dependencies/zycore"
    CACHE
    PATH
    "The path to look for Zycore")

# =============================================================================================== #
# Dependencies                                                                                    #
# =============================================================================================== #

if (ZYDIS_SYSTEM_ZYCORE)
    find_package(Zycore)
else ()
    # Try to initialize the Zycore submodule using Git
    if (NOT EXISTS "${ZYDIS_ZYCORE_PATH}/CMakeLists.txt" AND 
        "${ZYDIS_ZYCORE_PATH}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/dependencies/zycore")
        find_package(Git QUIET)
        if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
            execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive 
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
            )
        endif()
    endif ()

    if (NOT EXISTS "${ZYDIS_ZYCORE_PATH}/CMakeLists.txt")
        message(
            FATAL_ERROR
            "Can't find zycore submodule. Please make sure to clone the repo recursively.\n"
            "You can fix this by running\n"
            "    git submodule update --init\n"
            "or by cloning using\n"
            "    git clone --recursive <url>\n"
            "Alternatively, you can manually clone zycore to some path and set ZYDIS_ZYCORE_PATH."
        )
    endif ()

    add_subdirectory(${ZYDIS_ZYCORE_PATH} "zycore" EXCLUDE_FROM_ALL)
endif ()

# =============================================================================================== #
# Library configuration                                                                           #
# =============================================================================================== #

if (ZYDIS_BUILD_SHARED_LIB)
    add_library("Zydis" SHARED)
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
    target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
else ()
    include_directories(${CMAKE_JS_INC})
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
    target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
endif ()

target_link_libraries("Zydis" PUBLIC "Zycore")
target_include_directories("Zydis"
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE "src")
target_compile_definitions("Zydis" PRIVATE "_CRT_SECURE_NO_WARNINGS" "ZYDIS_EXPORTS")
set_target_properties("Zydis" PROPERTIES
    VERSION ${Zydis_VERSION}
    SOVERSION ${Zydis_VERSION_MAJOR}.${Zydis_VERSION_MINOR})
zyan_set_common_flags("Zydis")
zyan_maybe_enable_wpo_for_lib("Zydis")
generate_export_header("Zydis" BASE_NAME "ZYDIS" EXPORT_FILE_NAME "ZydisExportConfig.h")

if (ZYDIS_FEATURE_FORMATTER AND NOT ZYDIS_FEATURE_DECODER)
    message(
        FATAL_ERROR
        "\nZYDIS_FEATURE_FORMATTER requires ZYDIS_FEATURE_DECODER to be enabled"
    )
endif ()

if (ZYDIS_FEATURE_ENCODER AND (ZYDIS_MINIMAL_MODE OR
                               NOT ZYDIS_FEATURE_DECODER OR
                               NOT ZYDIS_FEATURE_AVX512 OR
                               NOT ZYDIS_FEATURE_KNC))
    message(
        FATAL_ERROR
        "\nZYDIS_FEATURE_ENCODER requires ZYDIS_FEATURE_DECODER in full mode (ZYDIS_MINIMAL_MODE \
        disabled) with all ISA extensions (ZYDIS_FEATURE_AVX512 and ZYDIS_FEATURE_KNC enabled)"
    )
endif ()

if (ZYDIS_MINIMAL_MODE)
    target_compile_definitions("Zydis" PUBLIC "ZYDIS_MINIMAL_MODE")
endif ()
if (NOT ZYDIS_FEATURE_DECODER)
    target_compile_definitions("Zydis" PUBLIC "ZYDIS_DISABLE_DECODER")
endif ()
if (NOT ZYDIS_FEATURE_ENCODER)
    target_compile_definitions("Zydis" PUBLIC "ZYDIS_DISABLE_ENCODER")
endif ()
if (NOT ZYDIS_FEATURE_FORMATTER)
    target_compile_definitions("Zydis" PUBLIC "ZYDIS_DISABLE_FORMATTER")
endif ()
if (NOT ZYDIS_FEATURE_AVX512)
    target_compile_definitions("Zydis" PUBLIC "ZYDIS_DISABLE_AVX512")
endif ()
if (NOT ZYDIS_FEATURE_KNC)
    target_compile_definitions("Zydis" PUBLIC "ZYDIS_DISABLE_KNC")
endif ()

target_sources("Zydis"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/MetaInfo.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Mnemonic.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Register.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/SharedTypes.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/ShortString.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Status.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Utils.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Zydis.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Internal/SharedData.h"
        "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Internal/String.h"
        "src/MetaInfo.c"
        "src/Mnemonic.c"
        "src/Register.c"
        "src/SharedData.c"
        "src/String.c"
        "src/Utils.c"
        "src/Zydis.c"
        "node/addon.cpp"
        "node/encode.cpp"
        "node/encode.h"
        )

if (ZYDIS_FEATURE_DECODER)
    target_sources("Zydis"
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Decoder.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/DecoderTypes.h"
            "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Internal/DecoderData.h"
            "src/Decoder.c"
            "src/DecoderData.c")
    if (ZYDIS_FEATURE_ENCODER)
        target_sources("Zydis"
            PRIVATE
                "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Encoder.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Internal/EncoderData.h"
                "src/Encoder.c"
                "src/EncoderData.c")
    endif ()
    if (ZYDIS_FEATURE_FORMATTER AND (NOT ZYDIS_MINIMAL_MODE))
        target_sources("Zydis"
            PRIVATE
                "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Formatter.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/FormatterBuffer.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Internal/FormatterATT.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Internal/FormatterBase.h"
                "${CMAKE_CURRENT_LIST_DIR}/include/Zydis/Internal/FormatterIntel.h"
                "src/Formatter.c"
                "src/FormatterBuffer.c"
                "src/FormatterATT.c"
                "src/FormatterBase.c"
                "src/FormatterIntel.c")
    endif ()
endif ()

if (ZYDIS_BUILD_SHARED_LIB AND WIN32)
    target_sources("Zydis" PRIVATE "resources/VersionInfo.rc")
endif ()

zyan_set_source_group("Zydis")

configure_package_config_file(cmake/zydis-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/zydis-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/zydis"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/zydis-config-version.cmake"
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/zydis-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zydis-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/zydis"
)

install(TARGETS "Zydis"
    EXPORT "zydis-targets"
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(EXPORT "zydis-targets"
    NAMESPACE "Zydis::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/zydis")
install(FILES
    "${PROJECT_BINARY_DIR}/ZydisExportConfig.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

function (_maybe_set_emscripten_cfg target)
    if (EMSCRIPTEN)
        # Yep, that madness below is how Emscripten likes its quotes.
        set_target_properties("${target}"
            PROPERTIES COMPILE_FLAGS
            "-s \"EXPORT_NAME='${target}'\" -s MODULARIZE=1")
        set_target_properties("${target}"
            PROPERTIES LINK_FLAGS_RELEASE
            "-s \"EXPORT_NAME='${target}'\" -s MODULARIZE=1")
    endif ()
endfunction ()
