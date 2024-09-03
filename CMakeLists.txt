cmake_minimum_required(VERSION 3.15)
project(LightG C CXX)

set(CMAKE_CXX_STANDARD 20)

set(glew-cmake_BUILD_SHARED FALSE)
add_definitions(-DGLEW_STATIC)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw" glfw)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glew-cmake" glew)

set(DEPS_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/deps/glew-cmake/include")
set(DEPS_LIBRARIES glfw libglew_static)

file(GLOB_RECURSE SRC_FILES
	"src/*.cpp"
    "src/core/*.cpp"
    "src/core/*.h"
)

add_executable(LightG ${SRC_FILES})

target_link_libraries(LightG ${DEPS_LIBRARIES})
target_include_directories(LightG PRIVATE ${DEPS_INCLUDE_DIRECTORIES})