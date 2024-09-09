cmake_minimum_required(VERSION 3.15)
project(LightG C CXX)

set(CMAKE_CXX_STANDARD 20)

set(glew-cmake_BUILD_SHARED FALSE)
add_definitions(-DGLEW_STATIC)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw" glfw)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glew-cmake" glew)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glm" glm)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/entt" entt)

set(DEPS_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/deps/glew-cmake/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/glm"
                             "${CMAKE_CURRENT_SOURCE_DIR}/deps/entt/src"
                             )
set(DEPS_LIBRARIES glfw libglew_static glm::glm)

file(GLOB_RECURSE SRC_FILES
	"src/*.cpp"
    "src/core/*.cpp"
    "src/core/*.h"
    "src/render/*.cpp"
    "src/render/*.h"
    "src/rtx_render/*.cpp"
    "src/rtx_render/*.h"
)

file(GLOB_RECURSE SHADERS
    "src/render/*.shader"
    "src/rtx_render/*.shader"
)

add_executable(LightG ${SRC_FILES})

target_link_libraries(LightG ${DEPS_LIBRARIES})
target_include_directories(LightG PRIVATE ${DEPS_INCLUDE_DIRECTORIES})

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
file(COPY ${SHADERS} DESTINATION "${CMAKE_BINARY_DIR}/shaders")