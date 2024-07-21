cmake_minimum_required(VERSION 3.16)

project(cc_client VERSION 1.0)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(enet)
target_link_libraries(${PROJECT_NAME} enet::enet)
