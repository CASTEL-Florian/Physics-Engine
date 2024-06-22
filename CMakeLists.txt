cmake_minimum_required(VERSION 3.7)
project(OpenGL_Test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp GLAD/src/glad.c)


include_directories(include)

#Include library files
include_directories(GLAD/include)



#Add deps
add_subdirectory(GLFW)

add_subdirectory(glm)

add_subdirectory(stb)

add_subdirectory(src)

add_executable(OpenGL_Test ${SOURCE_FILES})
target_link_libraries(OpenGL_Test src)

add_custom_target(copy-resources ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(GLOB RESOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/resources/*.*)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources
        DEPENDS ${CMAKE_SOURCE_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${RESOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/resources)
add_dependencies(OpenGL_Test copy-resources)