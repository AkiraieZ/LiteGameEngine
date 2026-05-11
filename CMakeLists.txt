cmake_minimum_required(VERSION 3.16)
project(LightGameEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.8
    GIT_SHALLOW TRUE
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw glm imgui)

file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp" "src/core/*.h")
file(GLOB_RECURSE RENDERER_SOURCES "src/renderer/*.cpp" "src/renderer/*.h")
file(GLOB_RECURSE PHYSICS_SOURCES "src/physics/*.cpp" "src/physics/*.h")
file(GLOB_RECURSE RESOURCES_SOURCES "src/resources/*.cpp" "src/resources/*.h")
file(GLOB_RECURSE SCENE_SOURCES "src/scene/*.cpp" "src/scene/*.h")
file(GLOB_RECURSE INPUT_SOURCES "src/input/*.cpp" "src/input/*.h")
file(GLOB_RECURSE EDITOR_SOURCES "src/editor/*.cpp" "src/editor/*.h")
file(GLOB_RECURSE UTILS_SOURCES "src/utils/*.cpp" "src/utils/*.h")
file(GLOB_RECURSE APP_SOURCES "src/app/*.cpp" "src/app/*.h")

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(LightGameEngineLib STATIC
    ${CORE_SOURCES}
    ${RENDERER_SOURCES}
    ${PHYSICS_SOURCES}
    ${RESOURCES_SOURCES}
    ${SCENE_SOURCES}
    ${INPUT_SOURCES}
    ${EDITOR_SOURCES}
    ${IMGUI_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/glad_gl.c
)

target_include_directories(LightGameEngineLib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(LightGameEngineLib PUBLIC
    glfw
    glm::glm
)

if(WIN32)
    target_link_libraries(LightGameEngineLib PUBLIC opengl32 gdi32)
endif()

add_executable(LightGameEngine
    ${APP_SOURCES}
    src/main.cpp
)

target_link_libraries(LightGameEngine PRIVATE LightGameEngineLib)

file(GLOB SHADER_FILES "shaders/*.vert" "shaders/*.frag" "shaders/*.glsl")
add_custom_command(TARGET LightGameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:LightGameEngine>/shaders
    COMMENT "Copying shaders to output directory"
)
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET LightGameEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:LightGameEngine>/assets
        COMMENT "Copying assets to output directory"
    )
endif()

if(MSVC)
    target_compile_options(LightGameEngineLib PRIVATE /W3)
    target_compile_options(LightGameEngine PRIVATE /W3)
    target_compile_definitions(LightGameEngineLib PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(LightGameEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LightGameEngine)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
