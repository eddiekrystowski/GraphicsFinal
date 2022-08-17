#pragma once
#include <imgui.h>
#include "shader_s.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class ImguiHelper {
public:
    static bool drawCastle;
    static bool drawOcean;
    static bool drawSkybox;
    static bool timeOfDay;
    static bool AutoTime;
    static float timestep;
    static float fogStart;
    static float fogEnd;

    static float lightColor[];
    static float fogColor[];
    static float time;

    //settings for imgui
    static bool renderGrass;
    static bool drawGround;
    static bool wind;
    static bool multipleTextures;
    static float grass1_density;
    static float grass2_density;
    static float grass3_density;
    static float grass4_density;
    static float flower1_density;
    static float flower2_density;
    static float flower3_density;
    static  float flower4_density;
    static bool drawGrass1;
    static bool drawGrass2;
    static bool drawGrass3;
    static bool drawGrass4;
    static bool drawFlower1;
    static bool drawFlower2;
    static bool drawFlower3;
    static  bool drawFlower4;
    static float min_grass_height;
    static float grass_height_factor;
    static float windspeed;
    static float windstrength;
    static bool showBackgrounds;
    static float gamma;
    static bool useGamma;
    static bool grassBlend;
    static bool useFog;

    //helper functions
    static void initialize();
    static void setup(GLFWwindow* window);
    static void render();
    static void shutdown();
    static void createFrame();
    static void endFrame();
    static void setDefaultShader(Shader& shader);
    static void setGrassShader(Shader& grassShader);


private:

};