#include "UseImGui.h"
#include <algorithm>
#include <bits/ranges_algo.h>
#include "./api/StationData.h"

void UseImGui::Init(GLFWwindow* window, const char* glsl_version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

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
    ImGui::Begin("Conan Logo");
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

            ImGui::TableNextColumn();
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

    ImGui::End();
}