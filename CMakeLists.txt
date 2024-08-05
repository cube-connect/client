# cmake_minimum_required(VERSION 3.16)

# project(cc_client VERSION 1.0)

# # Set C++ standard to C++20
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_CXX_EXTENSIONS OFF)

# add_executable(${PROJECT_NAME} src/demo.cpp)

# find_package(enet)
# target_link_libraries(${PROJECT_NAME} enet::enet)



cmake_minimum_required(VERSION 3.15)
project(cube_connect)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")

# Include the directories from the dependencies
# include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# Link libraries
find_package(glfw3)
find_package(glad)
find_package(glm)
find_package(assimp)
find_package(stb)
find_package(imgui)
find_package(enet)

# adds the actual code files
add_subdirectory(src)
