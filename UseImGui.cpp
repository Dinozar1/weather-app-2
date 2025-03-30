#include "UseImGui.h"
#include <bits/ranges_algo.h>
#include "./api/StationData.h"
#include "./api/SensorsData.h"
#include "api/SensorsData.h"
#include <implot.h>
#include "implot.h"
#include "implot_internal.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
using namespace std::filesystem;


void UseImGui::Init(GLFWwindow* window, const char* glsl_version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    // Setup platform bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void UseImGui::NewFrame() {
    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UseImGui::Update() {
    ImGui::Begin("");
    ImGui::End();
}
void UseImGui::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void UseImGui::Shutdown() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void CustomImGui::Update() {
    // buffer to store cityFilter input
    static char cityFilterBuffer[256] = "";

    ImGui::Begin("Air Quality App");

    // Add a new section for radius search
    ImGui::Text("Station Radius Search");

    // Address input
    ImGui::InputText("Address", addressBuffer, IM_ARRAYSIZE(addressBuffer));

    // Radius input
    ImGui::SliderFloat("Radius (km)", &searchRadius, 1.0f, 100.0f, "%.1f km");

    // Search button
    if (ImGui::Button("Search Nearby Stations")) {
        // Reset previous search
        radiusStations.clear();
        isRadiusSearch = false;

        // Geocode the address
        StationData::GeoCoordinates searchCoords = StationData::GeocodeAddress(addressBuffer);

        if (searchCoords.valid) {
            // Find stations within radius
            for (const auto& station : StationData::stations) {

                //convert all coordinates of stations from string to double
                const double stationLat = stod(station.gegrLat);
                const double stationLon = stod(station.gegrLon);

                const double distance = StationData::CalculateDistance(
                    searchCoords.latitude, searchCoords.longitude,
                    stationLat, stationLon
                );


                if (distance <= searchRadius) radiusStations.push_back(station);

            }

            isRadiusSearch = true;
        } else ImGui::OpenPopup("Geocoding Error");
    }

    // Error popup for geocoding
    // only proves when valid is false
    if (ImGui::BeginPopupModal("Geocoding Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Could not find coordinates for the given address.");
        ImGui::Text("Please check the address and try again.");

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // cityFilter input
    ImGui::InputText("City Filter", cityFilterBuffer, IM_ARRAYSIZE(cityFilterBuffer));

    constexpr ImGuiTableFlags tableFlags =
        ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV |
                    ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_ScrollX |
                            ImGuiTableFlags_Resizable;

    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    if (ImGui::BeginTable("Stations", 7, tableFlags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 13))) {
        // Table headers
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableNextColumn();
        ImGui::TableHeader("ID");
        ImGui::TableNextColumn();
        ImGui::TableHeader("Station Name");
        ImGui::TableNextColumn();
        ImGui::TableHeader("City");
        ImGui::TableNextColumn();
        ImGui::TableHeader("Province");
        ImGui::TableNextColumn();
        ImGui::TableHeader("District");
        ImGui::TableNextColumn();
        ImGui::TableHeader("Latitude");
        ImGui::TableNextColumn();
        ImGui::TableHeader("Longitude");
        ImGui::TableNextColumn();

        size_t displayedStations = 0;

        // Choose which vector to iterate through based on search type
        const auto& stationsToDisplay = isRadiusSearch ? radiusStations : StationData::stations;

        for (const auto& station : stationsToDisplay) {
            // City filter logic
            string filterLower = cityFilterBuffer;
            string cityLower = station.cityName;
            ranges::transform(filterLower, filterLower.begin(), ::tolower);
            ranges::transform(cityLower, cityLower.begin(), ::tolower);

            if (strlen(cityFilterBuffer) > 0 && cityLower.find(filterLower) == string::npos) continue;

            ImGui::TableNextRow();

            // Inside the stations table loop, add a selection mechanism
            ImGui::TableNextColumn();
            if (ImGui::Selectable(("##row" + std::to_string(station.id)).c_str(), false, ImGuiSelectableFlags_SpanAllColumns)) {
                selectedStation = &station;
                FetchStationSensors(station.id);
                showSensorsWindow = true;
            }
            ImGui::Text("%d", station.id);

            ImGui::TableNextColumn();
            ImGui::Text("%s", station.stationName.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", station.cityName.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", station.provinceName.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", station.districtName.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", station.gegrLat.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", station.gegrLon.c_str());

            displayedStations++;
        }

        ImGui::EndTable();

        // Show total and displayed stations, with radius search info
        if (isRadiusSearch) {
            ImGui::Text("Total Stations: %zu", StationData::stations.size());
            ImGui::SameLine();
            ImGui::Text("| Stations within %.1f km: %zu", searchRadius, displayedStations);
        } else {
            ImGui::Text("Total Stations: %zu", StationData::stations.size());
            ImGui::SameLine();
            ImGui::Text("| Displayed Stations: %zu", displayedStations);
        }
    }


    ShowStationSensorsWindow();
    ImGui::End();
}

void CustomImGui::ShowStationSensorsWindow() {
    if (showSensorsWindow && selectedStation != nullptr) {

        bool isOpen = true;

        ImGui::Begin("Station Sensors", &isOpen, ImGuiWindowFlags_NoCollapse);

        if (!isOpen) {
            showSensorsWindow = false;
        }

        //display selected station information
        ImGui::Text("Station: %s", selectedStation->stationName.c_str());
        ImGui::Text("City: %s", selectedStation->cityName.c_str());
        ImGui::Text("Province: %s", selectedStation->provinceName.c_str());

        constexpr ImGuiTableFlags tableFlags =
            ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV |
                    ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_Resizable;

        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        if (ImGui::BeginTable("Sensors", 5, tableFlags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 13))) {
            // Table headers
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableNextColumn();
            ImGui::TableHeader("ID");
            ImGui::TableNextColumn();
            ImGui::TableHeader("Parameter Name");
            ImGui::TableNextColumn();
            ImGui::TableHeader("Parameter Formula");
            ImGui::TableNextColumn();
            ImGui::TableHeader("Parameter Code");
            ImGui::TableNextColumn();
            ImGui::TableHeader("Parameter Id");

            for (const auto& sensor : SensorsData::sensors) {
                if (sensor.stationId == selectedStation->id) {
                    ImGui::TableNextRow();


                    ImGui::TableNextColumn();

                    if (ImGui::Selectable(("##row" + std::to_string(sensor.id)).c_str(), false, ImGuiSelectableFlags_SpanAllColumns)) {
                        selectedSensor = &sensor;
                        FetchSensorValues(sensor.id);
                        showSensorsValueBox = true;
                    }

                    ImGui::Text("%d", sensor.id);

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", sensor.paramName.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", sensor.paramFormula.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", sensor.paramCode.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%d", sensor.idParam);
                }
            }

            ImGui::EndTable();

        }

        if (showSensorsValueBox) {
            ImGui::Text("Selected parameter: %s", selectedSensor->paramName.c_str());

            // variables for inputs
            static char startDateBuffer[32] = "";
            static char endDateBuffer[32] = "";
            static char startTimeBuffer[32] = "00:00";
            static char endTimeBuffer[32] = "23:59";

            double minValue = std::numeric_limits<double>::max();
            double maxValue = std::numeric_limits<double>::lowest();
            double firstValue = 0;
            double lastValue = 0;
            string dateOfMin;
            string dateOfMax;
            double average = 0;
            int valueCount = 0;

            struct dataBaseFormat {
                string stationName;
                string cityName;
                int stationId;
                int sensorId;
                string paramName;
                string paramFormula;
                vector<SensorsData::SensorValue> sensorsValues;
            };

            dataBaseFormat dataFormat(selectedStation->stationName,
                selectedStation->cityName,
                selectedStation->id,
                selectedSensor->id,
                selectedSensor->paramName,
                selectedSensor->paramFormula);

            ImGui::Text("Start Date (YYYY-MM-DD):");
            ImGui::InputText("##StartDate", startDateBuffer, IM_ARRAYSIZE(startDateBuffer));

            ImGui::Text("Start Time (HH:MM):");
            ImGui::InputText("##StartTime", startTimeBuffer, IM_ARRAYSIZE(startTimeBuffer));

            ImGui::Text("End Date (YYYY-MM-DD):");
            ImGui::InputText("##EndDate", endDateBuffer, IM_ARRAYSIZE(endDateBuffer));

            ImGui::Text("End Time (HH:MM):");
            ImGui::InputText("##EndTime", endTimeBuffer, IM_ARRAYSIZE(endTimeBuffer));

            constexpr ImGuiTableFlags tableFlagsValues =
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_BordersV |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_SizingFixedSame;

            if (ImGui::BeginTable("Values of Sensor", 2, tableFlagsValues, ImVec2(300.0f, TEXT_BASE_HEIGHT * 13))) {
                ImGui::TableSetupColumn("Date and Time", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableNextColumn();
                ImGui::TableHeader("Date and Time");
                ImGui::TableNextColumn();
                ImGui::TableHeader("Value");


                // Convert start and end dates and times to time_t for comparison
                time_t startTime = 0, endTime = 0;
                struct tm startTm = {0}, endTm = {0}; // https://cplusplus.com/reference/ctime/tm/

                //check if there is something in input
                if (strlen(startDateBuffer) > 0 && strlen(startTimeBuffer) > 0) {
                    // connect date and hour to one var
                    auto startDateTime = string(startDateBuffer) + " " + string(startTimeBuffer);

                    // change string into tm structure
                    strptime(startDateTime.c_str(), "%Y-%m-%d %H:%M", &startTm);

                    // change tm into time_t var
                    startTime = mktime(&startTm);
                }

                // same for tail
                if (strlen(endDateBuffer) > 0 && strlen(endTimeBuffer) > 0) {
                    string endDateTime = string(endDateBuffer) + " " + string(endTimeBuffer);
                    strptime(endDateTime.c_str(), "%Y-%m-%d %H:%M", &endTm);
                    endTime = mktime(&endTm);
                }

                bool isFirstValue = true;

                // Filter values based on date and time range
                for (const auto& value : selectedSensor->sensorsValues) {

                    tm valueTm = {0};

                    //change string into tm type
                    strptime(value.data.c_str(), "%Y-%m-%d %H:%M", &valueTm);

                    //convert tm into time_t
                    time_t valueTime = mktime(&valueTm);

                    // pass only values that fits into time schedule
                    if ((startTime == 0 || valueTime >= startTime) && (endTime == 0 || valueTime <= endTime)) {

                        // calculate max and min value
                        if (value.value < minValue) {
                            minValue = value.value;
                            dateOfMin = value.data;
                        }
                        if (value.value > maxValue) {
                            maxValue = value.value;
                            dateOfMax = value.data;
                        }

                        if (isFirstValue) {
                            firstValue = value.value;
                            isFirstValue = false;
                        }

                        lastValue = value.value;

                        average += value.value;
                        valueCount++;

                        SensorsData::SensorValue values(value.data, value.value);
                        dataFormat.sensorsValues.push_back(values);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", value.data.c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text("%f", value.value);
                    }
                }

                ImGui::EndTable();


            }



            if (showSensorsValueBox && selectedSensor) {
                ImGui::SameLine();

                ImGui::BeginChild("Chart", ImVec2(300, 0), true);

                if (ImPlot::BeginPlot(selectedSensor->paramName.c_str(), ImVec2(-1, 0))) {
                    vector<double> dates;
                    vector<double> values;

                    int counter = 0;

                    // Reuse the filtered values for the chart
                    time_t startTime = 0, endTime = 0;
                    struct tm startTm = {0}, endTm = {0};

                    // Do the same thing on chart data
                    if (strlen(startDateBuffer) > 0 && strlen(startTimeBuffer) > 0) {
                        string startDateTime = string(startDateBuffer) + " " + string(startTimeBuffer);
                        strptime(startDateTime.c_str(), "%Y-%m-%d %H:%M", &startTm);
                        startTime = mktime(&startTm);
                    }
                    if (strlen(endDateBuffer) > 0 && strlen(endTimeBuffer) > 0) {
                        string endDateTime = string(endDateBuffer) + " " + string(endTimeBuffer);
                        strptime(endDateTime.c_str(), "%Y-%m-%d %H:%M", &endTm);
                        endTime = mktime(&endTm);
                    }

                    for (auto it = selectedSensor->sensorsValues.rbegin(); it != selectedSensor->sensorsValues.rend(); ++it) {
                        struct tm valueTm = {0};
                        strptime(it->data.c_str(), "%Y-%m-%d %H:%M", &valueTm);
                        time_t valueTime = mktime(&valueTm);


                        if ((startTime == 0 || valueTime >= startTime) && (endTime == 0 || valueTime <= endTime)) {
                            dates.push_back(counter++);
                            values.push_back(it->value);
                        }
                    }

                    ImPlot::PlotLine("Sensor Values", dates.data(), values.data(), dates.size());

                    ImPlot::EndPlot();
                }
                ImGui::EndChild();


                }
            //calculate average
            average = valueCount > 0 ? average / valueCount : 0;

            if (valueCount > 0) {
                ImGui::Text("Statistical Analysis:");
                ImGui::Text("Minimum Value: %.2f (on %s)", minValue, dateOfMin.c_str());
                ImGui::Text("Maximum Value: %.2f (on %s)", maxValue, dateOfMax.c_str());
                ImGui::Text("Average Value: %.2f", average);

                if (firstValue <= lastValue) ImGui::Text("The trend is downward");
                else ImGui::Text("The trend is upward");
            }

            if (ImGui::Button("Save to Local Database (JSON)")) {
                json record = {
                    {"stationName", dataFormat.stationName},
                    {"cityName", dataFormat.cityName},
                    {"stationId", dataFormat.stationId},
                    {"sensorId", dataFormat.sensorId},
                    {"paramName", dataFormat.paramName},
                    {"paramFormula", dataFormat.paramFormula},
                    {"sensorValues", json::array()}
                };

                for (const auto& value : dataFormat.sensorsValues) {
                    record["sensorValues"].push_back({
                        {"date", value.data},
                        {"value", value.value}
                    });
                }

                path dataBasePath = "../LocalDataBase.json";


                json database;

                // If file exist save data to database variable
                if (exists(dataBasePath)) {
                    // Read existing data
                    ifstream file(dataBasePath);
                    file >> database;
                    file.close();
                }

                // if it's empty add a empty array
                if (database.is_null()) {
                    database = json::array();
                }

                //add a new record
                database.push_back(record);

                ofstream outFile(dataBasePath);
                outFile << std::setw(4) << database << std::endl;
                outFile.close();

            }

            }
        ImGui::End();
        }


    }



void CustomImGui::FetchStationSensors(const int stationId) {
    SensorsData::sensors.clear();

    const string sensorsUrl = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + to_string(stationId);

    const string sensorsJsonStr = StationData::FetchStations(sensorsUrl);

    if (sensorsJsonStr.empty()) {
        // Handle API fetch failure
        ImGui::OpenPopup("API Error");
        return;
    }

    if (!SensorsData::ParseSensorData(sensorsJsonStr)) {
        // Handle parsing failure
        ImGui::OpenPopup("Parsing Error");
        return;
    }

    // Check if we actually got any sensors
    if (SensorsData::sensors.empty()) {
        ImGui::OpenPopup("No Sensors");
    }


}

void CustomImGui::FetchSensorValues(const int sensorId) {
    for (auto& sensor : SensorsData::sensors) {
        sensor.sensorsValues.clear();
    }

    const string valuesUrl = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + to_string(sensorId);

    const string valuesOfSensor = StationData::FetchStations(valuesUrl);

    if (valuesOfSensor.empty()) {
        // Handle API fetch failure
        ImGui::OpenPopup("API Error");
        return;
    }

    if (!SensorsData::ParseSensorValues(valuesOfSensor)) {
        // Handle parsing failure
        ImGui::OpenPopup("Parsing Error");
        return;
    }

    // Check if we got any values
    bool hasValues = false;
    for (const auto& sensor : SensorsData::sensors) {
        if (!sensor.sensorsValues.empty()) {
            hasValues = true;
            break;
        }
    }

    if (!hasValues) {
        ImGui::OpenPopup("No Sensor Values");
    }
}