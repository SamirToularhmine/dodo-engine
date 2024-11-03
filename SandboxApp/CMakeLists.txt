cmake_minimum_required(VERSION 3.30)

set(target SandboxApp)

project(
    ${target}
    VERSION 0.1 
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug)

add_executable(${target} src/main.cpp)
target_link_libraries(${target} PUBLIC DodoEngine)

add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "$<TARGET_PROPERTY:${target},SOURCE_DIR>/resources"
            "$<TARGET_PROPERTY:${target},BINARY_DIR>/resources"
        COMMENT "copying ${CMAKE_BINARY_DIR}"
)

set_property(TARGET SandboxApp PROPERTY CXX_STANDARD 23)