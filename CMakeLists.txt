cmake_minimum_required(VERSION 3.16)
project(PC3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# GLFW (window + input)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
if(UNIX AND NOT APPLE)
  set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
endif()
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)

# GLM (math)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)

# GLAD
FetchContent_Declare(glad GIT_REPOSITORY https://github.com/libigl/libigl-glad.git GIT_TAG master)

FetchContent_MakeAvailable(glfw glm glad)

add_executable(PC3D src/main.cpp)
target_link_libraries(PC3D PRIVATE glfw glm::glm glad)

add_custom_command(TARGET PC3D POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:PC3D>/shaders)

target_compile_definitions(PC3D PRIVATE PC3D_SHADER_SRC_DIR="${CMAKE_SOURCE_DIR}/shaders")
