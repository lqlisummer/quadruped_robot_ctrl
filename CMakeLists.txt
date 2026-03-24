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

add_library(qrc_core INTERFACE)
target_include_directories(
    qrc_core
    INTERFACE
        ${PROJECT_SOURCE_DIR}/src
)
target_compile_features(qrc_core INTERFACE cxx_std_17)

function(qrc_add_stage target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE qrc_core)
endfunction()

if(QRC_BUILD_STAGE1)
    qrc_add_stage(
        qrc_stage1_single_motor
        ${PROJECT_SOURCE_DIR}/stages/stage1_single_motor/app/main.cpp
    )
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

