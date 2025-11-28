cmake_minimum_required(VERSION 3.16)
project(imtodo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ------------------------------------------------------------
# GLFW (FetchContent)
# ------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)

FetchContent_MakeAvailable(glfw)

# ------------------------------------------------------------
# IMGUI submodule
# ------------------------------------------------------------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp

    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# ------------------------------------------------------------
# imtodo executable
# ------------------------------------------------------------
file(GLOB IMTODO_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(imtodo ${IMTODO_SOURCES})

find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

target_link_libraries(imtodo
    PRIVATE imgui
    PRIVATE glfw
    PRIVATE OpenGL::GL
    PRIVATE X11::X11
)

target_include_directories(imtodo
    PRIVATE ${CMAKE_SOURCE_DIR}/src
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)