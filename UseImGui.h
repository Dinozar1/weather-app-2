#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

};