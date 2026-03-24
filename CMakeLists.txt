cmake_minimum_required(VERSION 3.16)

project(QuadrupedRobotCtrl VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(QRC_BUILD_STAGE1 "Build stage 1 executable." ON)
option(QRC_BUILD_STAGE2 "Build stage 2 executable." ON)
option(QRC_BUILD_STAGE3 "Build stage 3 executable." ON)
option(QRC_BUILD_STAGE4 "Build stage 4 executable." ON)
option(QRC_ENABLE_PHYARC_SDK "Enable vendor SDK integration on supported targets." OFF)

find_package(Threads REQUIRED)

set(QRC_PHYARC_SDK_ACTIVE OFF)
if(QRC_ENABLE_PHYARC_SDK)
    if(UNIX AND NOT APPLE)
        set(QRC_PHYARC_SDK_ACTIVE ON)
    else()
        message(WARNING "PhyArc SDK binaries in this repository are intended for Linux targets. Falling back to stub implementations on this host.")
    endif()
endif()

if(QRC_PHYARC_SDK_ACTIVE)
    set(QRC_PHYARC_SDK_ACTIVE_VALUE 1)
else()
    set(QRC_PHYARC_SDK_ACTIVE_VALUE 0)
endif()

add_library(qrc_core INTERFACE)
target_include_directories(
    qrc_core
    INTERFACE
        ${PROJECT_SOURCE_DIR}/src
)
target_compile_features(qrc_core INTERFACE cxx_std_17)
target_compile_definitions(qrc_core INTERFACE QRC_ENABLE_PHYARC_SDK=${QRC_PHYARC_SDK_ACTIVE_VALUE})

function(qrc_add_stage target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE qrc_core)
endfunction()

if(QRC_BUILD_STAGE1)
    add_library(
        qrc_stage1_single_motor_core
        ${PROJECT_SOURCE_DIR}/src/runtime/config_loader.cpp
        ${PROJECT_SOURCE_DIR}/src/runtime/telemetry_monitor.cpp
        ${PROJECT_SOURCE_DIR}/src/platform/phyarc/board_client.cpp
        ${PROJECT_SOURCE_DIR}/src/platform/phyarc/motor_driver.cpp
        ${PROJECT_SOURCE_DIR}/src/control/motor/single_motor_controller.cpp
        ${PROJECT_SOURCE_DIR}/src/safety/safety_manager.cpp
        ${PROJECT_SOURCE_DIR}/stages/stage1_single_motor/app/cli_app.cpp
    )
    target_link_libraries(qrc_stage1_single_motor_core PUBLIC qrc_core PRIVATE Threads::Threads)

    if(QRC_PHYARC_SDK_ACTIVE)
        target_include_directories(
            qrc_stage1_single_motor_core
            PRIVATE
                ${PROJECT_SOURCE_DIR}/third_party/phyarc/linux_sdk/include/c
        )
        target_link_directories(
            qrc_stage1_single_motor_core
            PUBLIC
                ${PROJECT_SOURCE_DIR}/third_party/phyarc/linux_sdk/lib/output
        )
        target_link_libraries(qrc_stage1_single_motor_core PUBLIC MotorDrive)
        target_link_options(
            qrc_stage1_single_motor_core
            PUBLIC
                -Wl,-rpath,${PROJECT_SOURCE_DIR}/third_party/phyarc/linux_sdk/lib/output
        )
    endif()

    add_executable(
        qrc_stage1_single_motor
        ${PROJECT_SOURCE_DIR}/stages/stage1_single_motor/app/main.cpp
    )
    target_link_libraries(qrc_stage1_single_motor PRIVATE qrc_stage1_single_motor_core)
endif()

if(QRC_BUILD_STAGE2)
    qrc_add_stage(
        qrc_stage2_parallel_drive_unit
        ${PROJECT_SOURCE_DIR}/stages/stage2_parallel_drive_unit/app/main.cpp
    )
endif()

if(QRC_BUILD_STAGE3)
    qrc_add_stage(
        qrc_stage3_single_leg
        ${PROJECT_SOURCE_DIR}/stages/stage3_single_leg/app/main.cpp
    )
endif()

if(QRC_BUILD_STAGE4)
    qrc_add_stage(
        qrc_stage4_quadruped
        ${PROJECT_SOURCE_DIR}/stages/stage4_quadruped/app/main.cpp
    )
endif()
