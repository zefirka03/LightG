cmake_minimum_required(VERSION 3.15)
project(air C CXX)

set(CMAKE_CXX_STANDARD 20)

set(glew-cmake_BUILD_SHARED FALSE)
add_definitions(-DGLEW_STATIC)

find_package(Vulkan REQUIRED)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../deps/glfw" glfw)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../deps/glew-cmake" glew)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../deps/glm" glm)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../deps/entt" entt)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../deps/soil" soil)

set(DEPS_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../deps/glfw/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../deps/glew-cmake/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../deps/glm/glm"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../deps/entt/src"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../deps/soil/inc"
                             )

set(DEPS_LIBRARIES glfw libglew_static glm::glm soil Vulkan::Vulkan)

file(GLOB_RECURSE SRC_FILES
	"src/*.cpp"
    "src/*.h"
    "src/core/*.cpp"
    "src/core/*.h"
    "src/core/utils/*.cpp"
    "src/core/utils/*.h"
    "src/core/vk/*.cpp"
    "src/core/vk/*.h"
    "src/render/*.cpp"
    "src/render/*.h"
    "src/rtx_render/*.cpp"
    "src/rtx_render/*.h"
    "src/scripting/*.cpp"
    "src/scripting/*.h"
    "src/debug/*.cpp"
    "src/debug/*.h"
    "src/scenes/*.h"
)

file(GLOB_RECURSE SHADERS
    "src/render/shaders/*.shader"
    "src/rtx_render/shaders/*.shader"
    "src/debug/shaders/*.shader"
)
set(SHADERS ${SHADERS} PARENT_SCOPE)

add_library(air STATIC ${SRC_FILES})

set_target_properties(air PROPERTIES
        VERSION 1.0
        SOVERSION 1)

target_compile_definitions(glm PUBLIC GLM_ENABLE_EXPERIMENTAL)
target_link_libraries(air ${DEPS_LIBRARIES})
target_include_directories(air PRIVATE ${DEPS_INCLUDE_DIRECTORIES} ${VULKAN_INCLUDE_DIRECTORIES})

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
file(COPY ${SHADERS} DESTINATION "${CMAKE_BINARY_DIR}/shaders")