cmake_minimum_required(VERSION 3.30)
project(weather_app_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(weather_app_2 main.cpp
        UseImGui.cpp
        UseImGui.h
        api/StationData.cpp
        api/StationData.h
        api/GeoLocation.cpp
)

find_package(glad CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE imgui::imgui)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE nlohmann_json::nlohmann_json)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE CURL::libcurl)
