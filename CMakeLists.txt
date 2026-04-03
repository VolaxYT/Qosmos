cmake_minimum_required(VERSION 3.20)
project(qosmos VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(qosmos 
    src/config.h 
    src/main.cpp 
    src/glad.c
    src/camera.h
    src/axes.h
    src/point_cloud.h
    src/quantum.h
    src/sampler.h
)

target_include_directories(qosmos PRIVATE dependencies)

target_link_libraries(qosmos PRIVATE glfw OpenGL::GL glm::glm)