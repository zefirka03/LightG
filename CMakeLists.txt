cmake_minimum_required(VERSION 3.15)
project(Game C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/air_engine" air)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/game" game)
