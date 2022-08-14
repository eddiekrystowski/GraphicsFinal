#include "ImguiHelper.h"
#include "shader_s.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

bool ImguiHelper::drawCastle = true;
bool ImguiHelper::drawOcean = true;
bool ImguiHelper::drawSkybox = true;
bool ImguiHelper::timeOfDay = false;
bool ImguiHelper::AutoTime = false;
float ImguiHelper::timestep = 0.02;
float ImguiHelper::fogStart = 0.0f;
float ImguiHelper::fogEnd = 100.0f;

float ImguiHelper::lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float ImguiHelper::fogColor[] = { 0.7, 0.8, 0.9, 1.0 };
float ImguiHelper::time = 0.0;

//settings for imgui
bool ImguiHelper::renderGrass = true;
bool ImguiHelper::drawGround = true;
bool ImguiHelper::wind = true;
bool ImguiHelper::multipleTextures = true;
float ImguiHelper::grass1_density = 1.0;
float ImguiHelper::grass2_density = 1.0;
float ImguiHelper::grass3_density = 1.0;
float ImguiHelper::grass4_density = 1.0;
float ImguiHelper::flower1_density = 0.01;
float ImguiHelper::flower2_density = 0.01;
float ImguiHelper::flower3_density = 0.01;
float ImguiHelper::flower4_density = 0.01;
bool ImguiHelper::drawGrass1 = true;
bool ImguiHelper::drawGrass2 = false;
bool ImguiHelper::drawGrass3 = false;
bool ImguiHelper::drawGrass4 = false;
bool ImguiHelper::drawFlower1 = false;
bool ImguiHelper::drawFlower2 = false;
bool ImguiHelper::drawFlower3 = false;
bool ImguiHelper::drawFlower4 = false;
float ImguiHelper::min_grass_height = 0.5f;
float ImguiHelper::grass_height_factor = 1.0f;
float ImguiHelper::windspeed = 0.15f;
float ImguiHelper::windstrength = 0.15;
bool ImguiHelper::showBackgrounds = false;

void ImguiHelper::setup(GLFWwindow* window) {
    //setup IMGUI
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

void ImguiHelper::createFrame() {
    // new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImguiHelper::render() {
    ImGui::Begin("Settings");
    ImGui::Checkbox("Draw Castle", &drawCastle);
    ImGui::Checkbox("Draw Ocean", &drawOcean);
    ImGui::Checkbox("Draw Skybox", &drawSkybox);
    ImGui::SliderFloat("Fog Start", &fogStart, 0.0f, 100.0f);
    ImGui::SliderFloat("Fog End", &fogEnd, 0.0f, 100.0f);
    ImGui::ColorEdit4("Fog Color", fogColor);
    ImGui::ColorEdit4("Light Color", lightColor);
    ImGui::Checkbox("Time of Day", &timeOfDay);
    if (timeOfDay) {
        ImGui::Indent(32.0f);
        ImGui::Checkbox("AutoTime", &AutoTime);
        if (AutoTime) {
            ImGui::Indent(32.0f);
            ImGui::SliderFloat("timestep", &timestep, 0.0f, 1.0f);
            ImGui::Unindent(32.0f);
        }
        ImGui::SliderFloat("Time", &time, 0.0f, 24.0f);
        ImGui::Unindent(32.0f);
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiHelper::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

