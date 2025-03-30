/**
* @file UseImGui.h
 * @brief Interface and implementation for ImGui integration
 * @details Provides UseImGui class to represent the graphical interphase of the application and handles initialization, updating and exiting
 */


#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <string>
#include <algorithm>
#include <ranges>
#include "./api/StationData.h"
#include "api/SensorsData.h"


/**
 * @class UseImGui
 * @brief Base class providing ImGui initialization and lifecycle management
 * @details Handles the setup, frame management, rendering, and shutdown of ImGui.
 *          Provides a virtual Update method for derived classes.
 */
class UseImGui {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~UseImGui() = default;

    /**
     * @brief Initializes ImGui with GLFW and OpenGL
     * @param window Pointer to the GLFW window
     * @param glsl_version OpenGL shader version string
     */
    static void Init(GLFWwindow* window, const char* glsl_version);


    /**
     * @brief Begins a new ImGui frame
     * @details Should be called at the beginning of each render loop iteration
     */
    static void NewFrame();

    /**
     * @brief Updates the ImGui UI elements
     * @details Virtual method to be implemented by childen classes
     */
    virtual void Update();

    /**
     * @brief Renders ImGui elements to the screen
     * @details Should be called after all UI elements are defined
     */
    static void Render();

    /**
     * @brief Cleans up ImGui resources
     * @details Should be called before application termination
     */
    static void Shutdown();
};


/**
 * @class CustomImGui
 * @brief Custom ImGui implementation for air quality station monitoring
 * @details Provides a specific GUI for GIOŚ air quality application like rendering Stations, Sensors and sensors values with analitics
 */
class CustomImGui final : public UseImGui {
public:
    /**
     * @brief Implements the application GUI
     * @details There will be all components of GUI interfaces like text, input tables etc.
     */
    void Update() override;

    /**
     * @brief Displaying a window with selected station from the main interface
     * @details Shows information of the selected station with a list of available sensors and its parameters
     */
    void ShowStationSensorsWindow();

private:
    /** @brief Collection of stations found within search radius */
    std::vector<StationData::Station> radiusStations;

    // Search parameters
    /** @brief Input buffer for address search */
    char addressBuffer[256] = "";

    /** @brief Search radius in kilometers */
    float searchRadius = 10.0f; // Default 10 km

    /** @brief Flag indicating if radius search is active */
    bool isRadiusSearch = false;



    //second window with sensors

    //Selecting sensors methods
    /** @brief Pointer to the currently selected station */
    const StationData::Station *selectedStation = nullptr;

    /** @brief Indicates if window with station info and sensors list should be displayed */
    bool showSensorsWindow = false;


    /**
     * @brief Fetches sensor list for a specific station
     * @param stationId ID of the station to fetch sensors for
     */
    static void FetchStationSensors(int stationId);

    //Sensors values
    /** @brief Pointer to the currently selected sensor */
    const SensorsData::Sensor *selectedSensor = nullptr;

    /** @brief Flag controlling visibility of sensor values display */
    bool showSensorsValueBox = false;

    /**
     * @brief Fetches measurement values for a specific sensor
     * @param sensorId ID of the sensor to fetch values for
     */
    static void FetchSensorValues(int sensorId);


};