#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <string>
#include <algorithm>
#include <ranges>
#include "./api/StationData.h"

class UseImGui {
public:
    virtual ~UseImGui() = default;

    static void Init(GLFWwindow* window, const char* glsl_version);

    static void NewFrame();
    virtual void Update();

    static void Render();

    static void Shutdown();
};

class CustomImGui final : public UseImGui {
public:
    void Update() override;

private:
    // Store searched stations within radius
    std::vector<StationData::Station> radiusStations;

    // Search parameters
    char addressBuffer[256] = "";
    float searchRadius = 10.0f; // Default 10 km
    bool isRadiusSearch = false;
};