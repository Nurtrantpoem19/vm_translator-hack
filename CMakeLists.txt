cmake_minimum_required(VERSION 3.12)
project(Hack_assembler VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

add_library(Parser src/Parser.cpp)
add_library(Code src/Code.cpp)
add_executable(Main src/Translator.cpp)

target_link_libraries(Main PRIVATE Code Parser)

set_target_properties(Main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

target_include_directories(Parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Code PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
