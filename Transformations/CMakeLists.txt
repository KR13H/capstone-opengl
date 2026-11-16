cmake_minimum_required(VERSION 3.16)
project(HelloTriangle C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

add_executable(hello_triangle src/main.cpp)
target_link_libraries(hello_triangle PRIVATE glad glfw OpenGL::GL glm::glm)