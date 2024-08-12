cmake_minimum_required(VERSION 3.10)
project(client VERSION 0.1)

# Enable JSON compilation database generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add the main executable
add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources
        ${PROJECT_BINARY_DIR}/resources
        COMMENT "Copying resources into binary directory")

add_dependencies(${PROJECT_NAME} copy_resources)

# Link libraries
find_package(glfw3)
find_package(glad)
find_package(glm)
find_package(assimp)
find_package(stb)
find_package(imgui)
find_package(enet)

target_link_libraries(${PROJECT_NAME} glfw glad::glad glm::glm assimp::assimp stb::stb imgui::imgui enet::enet)
