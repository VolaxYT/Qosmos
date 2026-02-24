cmake_minimum_required(VERSION 4.0.0)
project(qosmos VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)


add_executable(qosmos 
    src/config.h 
    src/main.cpp 
    src/glad.c
    src/triangle_mesh.h
    src/triangle_mesh.cpp
)

target_include_directories(qosmos PRIVATE dependencies)

target_link_libraries(qosmos PRIVATE glfw OpenGL::GL)
target_link_libraries(qosmos PRIVATE glm::glm)