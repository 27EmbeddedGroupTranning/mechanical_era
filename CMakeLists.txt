cmake_minimum_required(VERSION 3.22)
project(mechanical_era LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
enable_language(ASM)
add_executable(${PROJECT_NAME})
add_subdirectory(cmake/stm32cubemx)
target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/ps2.cpp
    src/ps2_protocol.cpp
    src/drive_control.cpp
    src/arm_control.cpp
    src/outputs.cpp
    src/diagnostics.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE inc)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Werror>)
# 某些工具链错误地将 ob 加入 C 隐式链接库。
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
target_link_libraries(${PROJECT_NAME} stm32cubemx)
