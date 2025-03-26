cmake_minimum_required(VERSION 3.30)
project(weather_app_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(weather_app_2 main.cpp
        UseImGui.cpp
        UseImGui.h)

find_package(glad CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE imgui::imgui)
