cmake_minimum_required(VERSION 3.15)
project(game C CXX)

set(CMAKE_CXX_STANDARD 20)

set(glew-cmake_BUILD_SHARED FALSE)
add_definitions(-DGLEW_STATIC)

find_package(Vulkan REQUIRED)

set(DEPS_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../deps/glfw/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../deps/glew-cmake/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../deps/glm/glm"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../deps/entt/src"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../deps/soil/inc"
                             "${CMAKE_CURRENT_SOURCE_DIR}/../air_engine/src"
                             )
set(DEPS_LIBRARIES glfw libglew_static glm::glm soil Vulkan::Vulkan air)

file(GLOB_RECURSE SRC_FILES
	"src/*.cpp"
    "src/*.h"
)

file(GLOB_RECURSE GAME_SHADERS
    "src/grass_system/shaders/*.shader"
    "src/environment_system/shaders/*.shader"
)

if(AIR_IMGUI)
    file(GLOB_RECURSE IMGUI_SRC_FILES
        "imgui/*.cpp"
    )
    list(APPEND SRC_FILES ${IMGUI_SRC_FILES})
    list(APPEND DEPS_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
endif()

add_executable(game ${SRC_FILES})

target_compile_definitions(glm PUBLIC GLM_ENABLE_EXPERIMENTAL)
target_link_libraries(game PRIVATE ${DEPS_LIBRARIES})
target_include_directories(game PRIVATE ${DEPS_INCLUDE_DIRECTORIES} ${VULKAN_INCLUDE_DIRECTORIES})

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(COPY ${GAME_SHADERS} ${SHADERS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders")