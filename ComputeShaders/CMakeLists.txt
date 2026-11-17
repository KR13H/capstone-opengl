cmake_minimum_required(VERSION 3.16)
project(HelloTriangle C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
include(FetchContent)

find_package(glfw3 QUIET)
if (NOT glfw3_FOUND)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
    FetchContent_MakeAvailable(glfw)
endif()

find_package(glm CONFIG QUIET)
if (NOT glm_FOUND)
    FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
    FetchContent_MakeAvailable(glm)
endif()

add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

add_executable(hello_triangle src/main.cpp)
target_link_libraries(hello_triangle PRIVATE glad glfw OpenGL::GL glm::glm)