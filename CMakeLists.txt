cmake_minimum_required(VERSION 3.20)
project(Engine)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE SRC_SOURCES src/*.cpp)
file(GLOB_RECURSE SRC_HEADERS src/*.hpp)
set(HEADER_FILES pch.h)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SRC_SOURCES} ${SRC_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${VULKAN_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIR})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${HEADER_FILES})

#link_libraries("-static")
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)