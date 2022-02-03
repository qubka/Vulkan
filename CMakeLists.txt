cmake_minimum_required(VERSION 3.20)
project(Engine)

set(STB_INCLUDE_DIRS /usr/include/stb/)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

file(GLOB_RECURSE SRC_SOURCES src/*.cpp)
file(GLOB_RECURSE SRC_HEADERS src/*.hpp)
set(HEADER_FILES pch.hpp)

add_executable(${PROJECT_NAME} ${SRC_SOURCES} ${SRC_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${VULKAN_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${STB_INCLUDE_DIRS})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)