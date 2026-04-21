set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
cmake_minimum_required(VERSION 3.20)

# This project always targets a bare-metal RISC-V MCU rather than the host OS.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

set(COMMON_ENV_FILE "${CMAKE_SOURCE_DIR}/CMakeEnvs.txt")

if(WIN32)
    set(PLATFORM_ENV_FILE "${CMAKE_SOURCE_DIR}/CMakeEnvs-Win.txt")
elseif(APPLE)
    set(PLATFORM_ENV_FILE "${CMAKE_SOURCE_DIR}/CMakeEnvs-Mac.txt")
else()
    message(FATAL_ERROR "Unsupported host OS: ${CMAKE_HOST_SYSTEM_NAME}. Only Windows and macOS are supported.")
endif()

foreach(ENV_FILE IN ITEMS "${COMMON_ENV_FILE}" "${PLATFORM_ENV_FILE}")
    if(NOT EXISTS "${ENV_FILE}")
        message(FATAL_ERROR "Missing CMake environment file: ${ENV_FILE}")
    endif()

    file(STRINGS "${ENV_FILE}" ENV_LINES)
    foreach(ENV_LINE IN LISTS ENV_LINES)
        string(STRIP "${ENV_LINE}" ENV_LINE)
        if(ENV_LINE STREQUAL "" OR ENV_LINE MATCHES "^#")
            continue()
        endif()

        string(REGEX MATCH "^([^=]+)=(.*)$" _ "${ENV_LINE}")
        if(CMAKE_MATCH_COUNT GREATER 0)
            set(KEY "${CMAKE_MATCH_1}")
            set(VALUE "${CMAKE_MATCH_2}")
            string(STRIP "${KEY}" KEY)
            string(STRIP "${VALUE}" VALUE)
            string(REPLACE "\\ " " " VALUE "${VALUE}")
            set(${KEY} "${VALUE}")
        endif()
    endforeach()
endforeach()

foreach(REQUIRED_VAR TOOLCHAIN TOOLCHAIN_INCLUDE PROJECT_NAME CMAKE_BUILD_TYPE)
    if(NOT DEFINED ${REQUIRED_VAR} OR "${${REQUIRED_VAR}}" STREQUAL "")
        message(FATAL_ERROR "Missing required variable in CMakeEnvs files: ${REQUIRED_VAR}")
    endif()
endforeach()

set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING "Build type" FORCE)

if(WIN32)
    set(CMAKE_C_COMPILER "${TOOLCHAIN}gcc.exe")
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN}g++.exe")
    set(CMAKE_ASM_COMPILER "${TOOLCHAIN}gcc.exe")
    set(CMAKE_AR "${TOOLCHAIN}ar.exe")
    set(CMAKE_OBJCOPY "${TOOLCHAIN}objcopy.exe")
    set(CMAKE_OBJDUMP "${TOOLCHAIN}objdump.exe")
    set(SIZE "${TOOLCHAIN}size.exe")
else()
    set(CMAKE_C_COMPILER "${TOOLCHAIN}gcc")
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN}g++")
    set(CMAKE_ASM_COMPILER "${TOOLCHAIN}gcc")
    set(CMAKE_AR "${TOOLCHAIN}ar")
    set(CMAKE_OBJCOPY "${TOOLCHAIN}objcopy")
    set(CMAKE_OBJDUMP "${TOOLCHAIN}objdump")
    set(SIZE "${TOOLCHAIN}size")
endif()

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

project(${PROJECT_NAME} C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(COMMON_OPTIONS
    -march=rv32imac
    -mabi=ilp32
    -mcmodel=medany
    -msmall-data-limit=8
    -mno-save-restore
    -fmax-errors=20
    -Os
    -fmessage-length=0
    -fsigned-char
    -ffunction-sections
    -fdata-sections
    -fno-common
    -g
)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Ld/Link.ld")

file(
    GLOB_RECURSE
    SOURCES
    APP/*.c
    HAL/*.c
    LIB/ble_task_scheduler.S
    RVMSIS/*.c
    Startup/startup_CH592.S
    StdPeriphDriver/*.c
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    "${TOOLCHAIN_INCLUDE}"
    "${CMAKE_SOURCE_DIR}/APP/include"
    "${CMAKE_SOURCE_DIR}/HAL/include"
    "${CMAKE_SOURCE_DIR}/LIB"
    "${CMAKE_SOURCE_DIR}/RVMSIS"
    "${CMAKE_SOURCE_DIR}/Startup"
    "${CMAKE_SOURCE_DIR}/StdPeriphDriver/inc"
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${COMMON_OPTIONS}
    $<$<COMPILE_LANGUAGE:C>:-std=gnu99>
    $<$<COMPILE_LANGUAGE:C>:-MMD>
    $<$<COMPILE_LANGUAGE:C>:-MP>
    $<$<COMPILE_LANGUAGE:ASM>:-x>
    $<$<COMPILE_LANGUAGE:ASM>:assembler-with-cpp>
)

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
if(BUILD_TYPE_LOWER STREQUAL "debug")
    target_compile_definitions(${PROJECT_NAME}.elf PRIVATE DEBUG=1)
endif()

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${COMMON_OPTIONS}
    "-T${LINKER_SCRIPT}"
    -nostartfiles
    "-Wl,--gc-sections"
    "-Wl,--print-memory-usage"
    "-Wl,-Map,${PROJECT_NAME}.map"
    --specs=nano.specs
    --specs=nosys.specs
)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    "${CMAKE_SOURCE_DIR}/LIB/libCH59xBLE.a"
    "${CMAKE_SOURCE_DIR}/StdPeriphDriver/libISP592.a"
)

set(HEX_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex")
set(BIN_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin")
set(LST_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lst")

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND "${CMAKE_OBJCOPY}" -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> "${HEX_FILE}"
    COMMAND "${CMAKE_OBJCOPY}" -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> "${BIN_FILE}"
    COMMAND "${CMAKE_OBJDUMP}" --source --all-headers --demangle --line-numbers --wide
            $<TARGET_FILE:${PROJECT_NAME}.elf> > "${LST_FILE}"
    COMMAND "${SIZE}" --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    VERBATIM
)

message(STATUS "TOOLCHAIN: ${TOOLCHAIN}")
message(STATUS "TOOLCHAIN_INCLUDE: ${TOOLCHAIN_INCLUDE}")
message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
