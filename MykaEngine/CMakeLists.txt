cmake_minimum_required(VERSION 3.23)
project(MykaEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLAD
add_library(glad STATIC
    vendor/glad/src/glad.c
)

target_include_directories(glad
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

# ImGui
add_library(imgui STATIC
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
)

target_link_libraries(imgui PRIVATE glfw)

# ImGuizmo
add_library(ImGuizmo STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/ImGuizmo/ImGuizmo.cpp
)

target_include_directories(ImGuizmo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/ImGuizmo
)

target_link_libraries(ImGuizmo
    PUBLIC
        imgui
)

# stb_image
add_library(stb_image STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image/stb_image.cpp
)

target_include_directories(stb_image PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image
)

# entt
add_library(entt INTERFACE)

target_include_directories(entt INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/entt/include
)

# glfw - glm - spdlog - nlohmann/json
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/json)

# Sources
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    src/*.hpp
)

add_library(MykaEngine STATIC
    ${ENGINE_SOURCES}
)

target_include_directories(MykaEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MykaEngine
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/json/include
)

target_link_libraries(MykaEngine
    PUBLIC
        entt
        spdlog
        imgui
        ImGuizmo
    PRIVATE
        glad
        stb_image
        glfw
        opengl32
)

target_precompile_headers(MykaEngine
    PRIVATE
        src/mykapch.hpp
)

if (MSVC)
    target_compile_options(MykaEngine PRIVATE /W4 /utf-8)
endif()

set_target_properties(MykaEngine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
