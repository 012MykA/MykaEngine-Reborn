cmake_minimum_required(VERSION 3.23)
project(MykaEditor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE MYKA_EDITOR_SOURCES
    src/*.cpp
    src/*.hpp
)

add_executable(MykaEditor ${MYKA_EDITOR_SOURCES})

target_link_libraries(MykaEditor
    PRIVATE
        MykaEngine
)

target_compile_definitions(MykaEditor
    PRIVATE
        MYKA_DEBUG
)

if (MSVC)
    target_compile_options(MykaEditor PRIVATE /W4 /utf-8)
endif()

set_target_properties(MykaEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
