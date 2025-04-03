cmake_minimum_required(VERSION 3.30)
project(weather_app_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(weather_app_2 main.cpp
        UseImGui.cpp
        UseImGui.h
        api/StationData.cpp
        api/StationData.h
        api/GeoLocation.cpp
        api/SensorsData.cpp
        api/SensorsData.h
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

find_package(implot CONFIG REQUIRED)
target_link_libraries(weather_app_2 PRIVATE implot::implot)

enable_testing()

find_package(GTest CONFIG REQUIRED)

# Create test executable
add_executable(station_data_test
        api/StationData.h
        api/StationData.cpp
        api/GeoLocation.cpp
        tests/TestCalculateDistance.cpp
        tests/TestGeocodeAddress.cpp
)

# Link test executable with GTest and other dependencies
target_link_libraries(station_data_test PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
        CURL::libcurl  # Needed for curl definitions
        nlohmann_json::nlohmann_json  # If needed for tests
)

# Register tests with CTest
add_test(NAME StationDataTests COMMAND station_data_test)

# Include directories for tests
target_include_directories(station_data_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)