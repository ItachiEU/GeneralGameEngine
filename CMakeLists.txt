cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(game-engine)

include_directories(".")
file(GLOB SOURCES "*.cpp")

add_compile_options(-Wall -O2 -std=c++17 -g -pthread)
add_link_options(-lm -g)

find_package(Threads REQUIRED)
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(game-engine ${SOURCES})
target_link_libraries(game-engine "${TORCH_LIBRARIES}")
set_property(TARGET game-engine PROPERTY CXX_STANDARD 17)