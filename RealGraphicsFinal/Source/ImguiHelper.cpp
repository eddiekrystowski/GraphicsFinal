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
bool ImguiHelper::drawGrass2 = true;
bool ImguiHelper::drawGrass3 = true;
bool ImguiHelper::drawGrass4 = true;
bool ImguiHelper::drawFlower1 = true;
bool ImguiHelper::drawFlower2 = true;
bool ImguiHelper::drawFlower3 = true;
bool ImguiHelper::drawFlower4 = true;
float ImguiHelper::min_grass_height = 0.5f;
float ImguiHelper::grass_height_factor = 2.2f;
float ImguiHelper::windspeed = 0.15f;
float ImguiHelper::windstrength = 0.15;
bool ImguiHelper::showBackgrounds = false;

float ImguiHelper::gamma = 1.2;
bool ImguiHelper::useGamma = false;
bool ImguiHelper::grassBlend = false;
bool ImguiHelper::useFog = false;

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
    //ImGui::Checkbox("Draw Castle", &drawCastle);
    ImGui::Checkbox("Draw Skybox", &drawSkybox);
    ImGui::Checkbox("Use Fog", &useFog);
    if (useFog) {
        ImGui::Indent(32.0f);
        ImGui::SliderFloat("Fog Start", &fogStart, 0.0f, 600.0f);
        ImGui::SliderFloat("Fog End", &fogEnd, 0.0f, 600.0f);
        ImGui::ColorEdit4("Fog Color", fogColor);
        ImGui::Unindent(32.0f);
    }
    ImGui::ColorEdit4("Light Color", lightColor);
    ImGui::Checkbox("Use Gamma", &useGamma);
    if (useGamma) {
        ImGui::Indent(32.0f);
        ImGui::SliderFloat("Gamma", &gamma, 0.0f, 10.0f);
        ImGui::Unindent(32.0f);
    }
    ImGui::Text("GRASS SETTINGS:");
    //ImGui::Checkbox("Draw Ground", &drawGround);
    ImGui::Checkbox("Draw Grass", &renderGrass);
    if (renderGrass) {
        ImGui::Indent(32.0f);
        ImGui::Checkbox("Blend Grass", &grassBlend);
        ImGui::SliderFloat("Min Grass Height", &min_grass_height, 0.0f, 1.0f);
        ImGui::SliderFloat("Grass Height Factor", &grass_height_factor, 0.0f, 5.0f);
        ImGui::Checkbox("Show Texture Backgrounds", &showBackgrounds);
        ImGui::Checkbox("Wind", &wind);
        if (wind) {
            ImGui::Indent(32.0f);
            ImGui::SliderFloat("Wind Speed", &windspeed, 0.0f, 1.0f);
            ImGui::SliderFloat("Wind Strength", &windstrength, 0.0f, 1.0f);
            ImGui::Unindent(32.0f);
        }
        ImGui::Checkbox("Use Multiple Grass Textures", &multipleTextures);
        if (multipleTextures) {
            ImGui::Indent(32.0f);
            ImGui::Checkbox("Use Grass 1", &drawGrass1);
            ImGui::Checkbox("Use Grass 2", &drawGrass2);
            ImGui::Checkbox("Use Grass 3", &drawGrass3);
            ImGui::Checkbox("Use Grass 4", &drawGrass4);
            ImGui::Checkbox("Use Flower 1", &drawFlower1);
            ImGui::Checkbox("Use Flower 2", &drawFlower2);
            ImGui::Checkbox("Use Flower 3", &drawFlower3);
            ImGui::Checkbox("Use Flower 4", &drawFlower4);
            if (drawGrass1) ImGui::SliderFloat("Grass 1 probability", &grass1_density, 0.0, 1.0);
            if (drawGrass2) ImGui::SliderFloat("Grass 2 probability", &grass2_density, 0.0, 1.0);
            if (drawGrass3) ImGui::SliderFloat("Grass 3 probability", &grass3_density, 0.0, 1.0);
            if (drawGrass4) ImGui::SliderFloat("Grass 4 probability", &grass4_density, 0.0, 1.0);
            if (drawFlower1) ImGui::SliderFloat("Flower 1 probability", &flower1_density, 0.0, 1.0);
            if (drawFlower2) ImGui::SliderFloat("Flower 2 probability", &flower2_density, 0.0, 1.0);
            if (drawFlower3) ImGui::SliderFloat("Flower 3 probability", &flower3_density, 0.0, 1.0);
            if (drawFlower4) ImGui::SliderFloat("Flower 4 probability", &flower4_density, 0.0, 1.0);
            ImGui::Unindent(32.0f);
        }
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

