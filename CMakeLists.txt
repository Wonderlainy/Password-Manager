# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Password_Manager VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES WIN32
            src/main.cpp
            src/GUI.cpp
            external/imgui/misc/cpp/imgui_stdlib.cpp
            external/imgui/imgui.cpp
            external/imgui/imgui_draw.cpp
            external/imgui/imgui_widgets.cpp
            external/imgui/imgui_tables.cpp
            external/imgui/backends/imgui_impl_glfw.cpp
            external/imgui/backends/imgui_impl_opengl3.cpp)

set(LINK_LIBRARIES  glfw3
                    # Replace this line with the proper path to your OpenGL32.Lib file
                    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64/OpenGL32.Lib"
                    )        

include_directories( external/glfw-3.4/include
                     external/imgui
                     external/imgui/backends
                     )

link_directories(external/glfw-3.4/build/src/Release)

# Add the executable
add_executable(Password_Manager ${SOURCES})

target_link_libraries(Password_Manager ${LINK_LIBRARIES})