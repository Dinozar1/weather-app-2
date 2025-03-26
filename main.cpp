#include <iostream>
#include "UseImGui.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

using namespace std;

int main() {
    // GLFW Initialization
    if (!glfwInit())  // Initialize GLFW. Returns false if initialization fails
        return 1;

    // OpenGL Context Version Setup
    const auto glsl_version = "#version 130";  // GLSL (OpenGL Shading Language) version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // Set OpenGL major version to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);  // Set OpenGL minor version to 0

    // Window Creation
    GLFWwindow *window = glfwCreateWindow(
        1280,        // Window width
        720,         // Window height
        "Dear ImGui - Example",  // Window title
        nullptr,        // Monitor for fullscreen
        nullptr         // Share resources with another window
    );
    if (window == nullptr)  // Check if window creation failed
        return 1;

    glfwMakeContextCurrent(window);  // Make the created window's context the current OpenGL context
    glfwSwapInterval(1);  // Enable V-Sync (limit frame rate to monitor refresh rate)

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw("Unable to context to OpenGL");  // Throw error if OpenGL function loading fails

    // Get Framebuffer Size and Set Viewport
    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height); // This function retrieves the actual size of the framebuffer for the specified window
    glViewport(0, 0, screen_width, screen_height);  // This OpenGL function sets the viewport, which defines the rectangular region where OpenGL will render

    // ImGui Initialization
    CustomImGui myimgui;  // Create an instance of custom ImGui wrapper
    CustomImGui::Init(window, glsl_version);  // Initialize ImGui with window and GLSL version

    // Main Rendering Loop
    while (!glfwWindowShouldClose(window)) {  // Continue until window close is requested
        glfwPollEvents();  // Process pending window events (input, etc.)

        glClear(GL_COLOR_BUFFER_BIT);  // Clear the color buffer

        CustomImGui::NewFrame();  // Start a new ImGui frame
        myimgui.Update();    // Update ImGui content
        CustomImGui::Render();    // Render ImGui elements

        glfwSwapBuffers(window);  // Swap front and back buffers
    }

    // Cleanup
    CustomImGui::Shutdown();  // Shutdown ImGui
    return 0;  // Exit successfully
}