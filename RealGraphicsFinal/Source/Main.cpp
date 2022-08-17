//
// Sandbox program for Computer Graphics For Games (G4G)
// created May 2021 by Eric Ameres for experimenting
// with OpenGL and various graphics algorithms
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ImguiHelper.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <filesystem>

#include "shader_s.h"
#include "Camera.h"

#include <string>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Texture.h"
#include "Vertex.h"
#include "Model.h"
#include "Water.h"
#include "Light.h"
#include "Terrain.h"
#include "Debug.h"
#define glCheckError() Debug::glCheckError_(__FILE__, __LINE__) 
#include "Window.h"


#define _USE_MATH_DEFINES
#include <math.h>

#pragma warning( disable : 26451 )
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//settings for imgui
bool renderGrass = true;
bool drawGround = true;
bool wind = true;
bool multipleTextures = true;
float grass1_density = 0.33;
float grass2_density = 0.33;
float grass3_density = 0.33;
float grass4_density = 0.11;
float flower1_density = 0.02;
float flower2_density = 0.02;
float flower3_density = 0.03;
float flower4_density = 0.03;
bool drawGrass1 = true;
bool drawGrass2 = true;
bool drawGrass3 = true;
bool drawGrass4 = true;
bool drawFlower1 = true;
bool drawFlower2 = true;
bool drawFlower3 = true;
bool drawFlower4 = true;
float min_grass_height = 0.5f;
float grass_height_factor = 1.0f;
float windspeed = 0.15;
float windstrength = 0.15;
bool showBackgrounds = false;








/*----------------------------------------------------------------------------------------------------------------------*/












#define watertest
#ifdef watertest
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path); 

glm::vec3 RGB(glm::vec3 color) {
    return glm::vec3(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f);
}

glm::vec3 RGB(float v0, float v1, float v2) {
    return glm::vec3(v0 / 255.0f, v1 / 255.0f, v2 / 255.0f);
}

// settings
//const unsigned int SCR_WIDTH = 1600;
//const unsigned int SCR_HEIGHT = 1000;
const unsigned int shadowScale = 1;
const unsigned int SHADOW_WIDTH = shadowScale * Window::Height, SHADOW_HEIGHT = shadowScale* Window::Height;
unsigned int depthMapFBO;
unsigned int depthMap;
float near_plane = -10.0f;
float far_plane = 200;

unsigned int castleNormal;

// camera
Camera* camera = new Camera(Projection::Perspective, 45, (float) (Window::Width / (float)Window::Height));
float lastX = (float)Window::Width / 2.0;
float lastY = (float)Window::Height / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        camera->pause();
        if (camera->paused) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        std::cout << "Camera Position:\n\tx: " << camera->GetPosition().x << "\n\ty: " << camera->GetPosition().y << "\n\tz: " << camera->GetPosition().z << std::endl;
        glm::vec3 cameraDir = camera->GetDirection();
        std::cout << "Camera Direction:\n\tx: " << cameraDir.x << "\n\ty: " << cameraDir.y << "\n\tz: " << cameraDir.z << std::endl;
    }
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void renderScene(Shader* shader, Shader* directionalShader, unsigned int cubeTexture, WaterFrameBuffer* waterFrameBuffer, Water* water, Terrain* terrain, Model* castle, Shader& treeShader, Model* tree);
void renderCube(unsigned int texture, bool clipPlaneEnabled, glm::vec4 clipPlane = glm::vec4(0, 0, 0, 0));

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(Window::Width, Window::Height, "Final Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_DEBUG_OUTPUT);
    //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    //sometimes this line says GLDebugMessageCallback is not a member of the Debug class,
    //this error can be ignored, the program will compile and run successfully
    //glDebugMessageCallback(Debug::GLDebugMessageCallback, NULL);

    //camera->SetPosition(glm::vec3(99.6366, 180.265, 97.0331));

    // build shader programs and setup uniforms
    // -------------------------
    Shader* waterShader = new Shader("./Shaders/water.vs", "./Shaders/water.fs");
    Shader* shader = new Shader("Shaders/Shader.vs", "Shaders/Shader.fs");
    Shader* directionalShader = new Shader("./Shaders/directional.vs", "./Shaders/directional.fs", nullptr, nullptr, nullptr);
    Shader* debugDepthQuad = new Shader("./Shaders/debug_quad.vs", "./Shaders/debug_quad.fs", nullptr, nullptr, nullptr);
    Shader* tessHeightMapShader = new Shader("./Shaders/passthrough.vs", "./Shaders/terrain.fs", nullptr, "./Shaders/tessellation_ground.tesc", "./Shaders/tessellation_ground.tese");
    Shader* tessHeightMapGrassShader = new Shader("./Shaders/passthrough.vs", "./Shaders/grass.fs", "./Shaders/grass.gs", "./Shaders/tessellation.tesc", "./Shaders/tessellation.tese");
    Shader* treeShader = new Shader("./Shaders/treeShader.vs", "./Shaders/directional.fs", nullptr, nullptr, nullptr);
    Shader* simpleDepthShader = new Shader("./Shaders/depthShader.vs", "./Shaders/depthShader.fs", nullptr, nullptr, nullptr);
    Shader* skyboxShader = new Shader("./Shaders/skybox.vs", "./Shaders/skybox.fs");



    tessHeightMapGrassShader->use();
    tessHeightMapGrassShader->setInt("grass_texture", 0);
    tessHeightMapGrassShader->setInt("windMap", 1);
    tessHeightMapGrassShader->setInt("heightMap", 2);
    tessHeightMapGrassShader->setInt("pathTexture", 3);


    tessHeightMapShader->use();
    tessHeightMapShader->setInt("heightMap", 0);
    tessHeightMapShader->setInt("dirtTexture", 1);
    tessHeightMapShader->setInt("cliffTexture", 2);
    tessHeightMapShader->setInt("normalMap", 3);
    glm::vec3 lightLevel = glm::vec3(1.0);
    glm::vec3 diffuseColor = glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * lightLevel;
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.7);
    tessHeightMapShader->setVec3("dir_light.ambient", ambientColor);
    tessHeightMapShader->setVec3("dir_light.diffuse", diffuseColor);
    tessHeightMapShader->setVec3("dir_light.specular", glm::vec3(1.0) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * glm::vec3(1.0));
    tessHeightMapShader->setVec3("dir_light.lightPos", Light::position);
    tessHeightMapShader->setVec3("dir_light.direction", Light::direction);

    waterShader->use();
    waterShader->setInt("reflectionSampler", 0);
    waterShader->setInt("refractionSampler", 1);
    waterShader->setInt("dudvSampler", 2);
    waterShader->setInt("normalSampler", 3);
    waterShader->setInt("depthSampler", 4);

    debugDepthQuad->use();
    debugDepthQuad->setInt("depthMap", 0);

    tessHeightMapShader->use();
    tessHeightMapShader->setInt("heightMap", 0);

    directionalShader->use();
    directionalShader->setInt("num_points", 0);
    directionalShader->setInt("shadowMap", 4);
    directionalShader->setInt("normalMap", 3);
    directionalShader->setInt("fogEnd", ImguiHelper::fogEnd);
    directionalShader->setInt("fogStart", ImguiHelper::fogStart);
    directionalShader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    directionalShader->setVec3("dir_light.ambient", ambientColor);
    directionalShader->setVec3("dir_light.diffuse", diffuseColor);
    directionalShader->setVec3("dir_light.specular", glm::vec3(1.0) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * glm::vec3(1.0));
    directionalShader->setVec3("dir_light.lightPos", Light::position);
    directionalShader->setVec3("dir_light.direction", Light::direction); 
    directionalShader->setFloat("material.shininess", 32.0f);

    
    glUseProgram(0);


    // load textures
    // -------------
    stbi_set_flip_vertically_on_load(true);
    unsigned int waterDudv = loadTexture("Textures/dudv.png");
    unsigned int waterNormal = loadTexture("Textures/normal.png");
    unsigned int grassAtlas = loadTexture("Textures/grass_atlas_evenlessborder.png");
    unsigned int dirtTexture = loadTexture("Textures/dirttexture.png");
    unsigned int cliffTexture = loadTexture("Textures/cliff.jpg");
    unsigned int windMap = loadTexture("Textures/wind.jpg");
    unsigned int cliffNormalMap = loadTexture("Textures/cliffBump.png");
    castleNormal = loadTexture("Textures/castleNormal5.jpg");

    stbi_set_flip_vertically_on_load(false);
    int pw, ph;
    unsigned int pathTexture = Terrain::LoadHeightmap("Textures/hmap10_path2.png", &pw, &ph);

    stbi_set_flip_vertically_on_load(false);
    Terrain* terrain = new Terrain("./Textures/hmap10.png", false);

    terrain->SetShader(tessHeightMapShader);
    terrain->SetGrassShader(tessHeightMapGrassShader);
    terrain->SetGrassAtlas(grassAtlas);
    terrain->SetDirtTexture(dirtTexture);
    terrain->SetCliffTexture(cliffTexture);
    terrain->SetWindMap(windMap);
    terrain->SetNormalMap(cliffNormalMap);
    terrain->SetPathTexture(pathTexture);

    Water* water = new Water();
    Mesh* waterMesh = Water::GenerateMesh(glm::vec2(256.0f, 256.0f));
    WaterFrameBuffer* waterFrameBuffer = new WaterFrameBuffer();
    water->SetMesh(waterMesh);
    water->SetShader(waterShader);
    water->SetDUDV(waterDudv);
    water->SetNormal(waterNormal);
    water->textureTiling = 4;
    water->distorsionStrength = 0.04f;

    Model* castle = new Model("./Models/castle_no_ground.obj");

    // configure depth map FBO
 // -----------------------

    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    
    // configure skybox
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces
    {
        "./Textures/skybox/right.jpg",
        "./Textures/skybox/left.jpg",
        "./Textures/skybox/top.jpg",
        "./Textures/skybox/bottom.jpg",
        "./Textures/skybox/front.jpg",
        "./Textures/skybox/back.jpg"
    };
    unsigned int skybox = loadCubemap(faces);
 
    // load models
    Model tree = Model("./Models/treeModelLowPoly.obj");
    

    // lighting info
    // -------------
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    // initialize settings
    // -----------
    //glEnable(GL_FRAMEBUFFER_SRGB);
    ImguiHelper::setup(window);

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------

        glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderScene(shader, directionalShader, waterDudv, waterFrameBuffer, water, terrain, castle, *treeShader, &tree);
        glCheckError();

        debugDepthQuad->use();
        debugDepthQuad->setFloat("near_plane", near_plane);
        debugDepthQuad->setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        //renderQuad();\

        if (ImguiHelper::drawSkybox) {
            // draw skybox as last
            glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
            skyboxShader->use();
            glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
            skyboxShader->setMat4("view", view);
            skyboxShader->setMat4("projection", camera->GetProjectionMatrix());
            skyboxShader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
            skyboxShader->setFloat("fogStart", ImguiHelper::fogStart);
            skyboxShader->setFloat("fogEnd", ImguiHelper::fogEnd);
            // skybox cube
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // set depth function back to default
        }

        ImguiHelper::createFrame();

        if (!ImguiHelper::timeOfDay) {
            directionalShader->use();
            directionalShader->setVec3("dir_light.ambient", glm::vec3(0.1f) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]));
            directionalShader->setVec3("dir_light.diffuse", glm::vec3(0.4f) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]));
            directionalShader->setVec3("dir_light.specular", glm::vec3(1.0) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]));
            glUseProgram(0);
        }

        ImguiHelper::render();

        if (ImguiHelper::AutoTime) {
            ImguiHelper::time += ImguiHelper::timestep;
            ImguiHelper::time = fmod(ImguiHelper::time, 24.0f);
        }
        if (ImguiHelper::timeOfDay) {
            //calculate values based on the time of day
            //lightLevel = glm::vec3(glm::clamp((1.0f / 2.0f) * (sin((M_PI / 12.0f) * (ImguiHelper::time)-(14.0f / 24.0f) * M_PI)) + 0.6f, 0.0, 0.9));
            glm::vec3 light_col;
            if (ImguiHelper::time <= 6 || ImguiHelper::time >= 18) {
                //blend night with sunup/down
                float delta = (ImguiHelper::time <= 6) ? ImguiHelper::time + 6 : (ImguiHelper::time - 18);
                glm::vec3 other = RGB(240.0f, 160.0f, 22.0f);
                float sun = glm::clamp((float)((1.0f / 2.0f) * (cos((M_PI / 6.0f) * (delta)) + 0.6)), 0.0f, 1.0f) * (0.15f);
                //light_col = glm::mix(lightLevel, other, sun);
            }
            else {
                // time > 6 time < 18
                //blend sunup/sundown with mid day
                float delta = ImguiHelper::time - 6;
                glm::vec3 other = RGB(240.0f, 160.0f, 22.0f);
                float sun = glm::clamp((float)((1.0f / 2.0f) * (cos((M_PI / 6.0f) * (delta)) + 0.6)), 0.0f, 1.0f) * (0.15f);
                //light_col = glm::mix(lightLevel, other, sun);
            }
            ImguiHelper::lightColor[0] = light_col[0];
            ImguiHelper::lightColor[1] = light_col[1];
            ImguiHelper::lightColor[2] = light_col[2];
            //skybox_color = glm::vec4(glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]), 1.0f);
            ImguiHelper::fogColor[0] = light_col[0];
            ImguiHelper::fogColor[1] = light_col[1];
            ImguiHelper::fogColor[2] = light_col[2];

            // set light direction (determined by light "position")
            // the light revolves around the scene in a (large) circle given by the equation
            //      a(x - x0) + b(y - y0) + c(z - z0) = 0
            //     : where z is a constant and x,y change depending on time
            float radius = 1.0f;
            lightPos = glm::vec3(
                radius * (cos((M_PI / 12.0f) * (ImguiHelper::time)-(14.0f / 24.0f) * M_PI)),     // x coordinate
                radius * (sin((M_PI / 12.0f) * (ImguiHelper::time)-(14.0f / 24.0f) * M_PI)),     // y coordinate 
                lightPos[2]                                                    // z coordinate
            );
        }
        

        glCheckError();

        //renderScene(shader, waterDudv, waterFrameBuffer);

        water->Update(deltaTime);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    ////end imgui
    ImguiHelper::shutdown();


    glfwTerminate();
    return 0;
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube(unsigned int texture, bool clipPlaneEnabled, glm::vec4 clipPlane)
{

    if (clipPlaneEnabled) {
        glEnable(GL_CLIP_DISTANCE0);
    }
    else {
        glDisable(GL_CLIP_DISTANCE0);
    }
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderScene(Shader* shader, Shader* directionalShader, unsigned int cubeTexture, WaterFrameBuffer* waterFrameBuffer, Water* water, Terrain* terrain, Model* castle, Shader& treeShader, Model* tree) {
    glm::vec3 skyColor = glm::vec3(0.815f, 0.925f, 0.992f);
    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 cubePos = glm::vec3(0, 2, 0);
    glm::vec3 castlePos = glm::vec3(92.4, 57.0, 25);
    glm::vec3 castleRot = glm::vec3(0, 1, 0);
    float castleRotAngle = 60.0f;
    float castleScale = 0.2f;
    float castleYScale = 0.5f;


    /**********************************************************
    *******        RENDER PASS 1 -- DEPTH BUFFER        *******
    ***********************************************************/
    //render scene to depth fbo
    glm::mat4 lightProjection;
    glm::mat4 lightSpaceMatrix;
    //float near_plane = -10.0f, far_plane = 700.5f;
    //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    Camera* depthCam = new Camera(Projection::Orthographic, 25, 1.0, near_plane, far_plane);
    depthCam->SetPosition(Light::position);
    depthCam->viewMatrix = glm::lookAt(Light::position, glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));
    lightProjection = depthCam->GetProjectionMatrix();//glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    //lightView = glm::lookAt(Light::position, glm::vec3(200 * Light::direction.x, 200* Light::direction.y, 200 * Light::direction.z), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * depthCam->GetViewMatrix();
    // render scene from light's point of view
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    terrain->shader->use();
    terrain->shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    terrain->shader->setInt("shadowMap", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    terrain->Render(depthCam, deltaTime);
    shader->use();
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, cubePos);
    model = glm::mat4(1.0);
    model = glm::translate(model, castlePos);
    model = glm::scale(model, glm::vec3(castleScale));
    model = glm::rotate(model, glm::radians(castleRotAngle), castleRot);
    shader->setVec4("clipPlane", glm::vec4(0, 0, 0, 0));
    shader->setFloat("textureTiling", 1);
    shader->setVec3("directionalLight.color", Light::color);
    shader->setFloat("directionalLight.intensity", Light::intensity);
    shader->setVec3("directionalLight.direction", Light::direction);
    shader->setMat4("projection", depthCam->GetProjectionMatrix());
    shader->setMat4("view", depthCam->GetViewMatrix());
    shader->setMat4("model", model);
    shader->setInt("textureSampler", 0);
    castle->draw(*shader);
    
    treeShader.use();
    model = glm::mat4(1.0);
    treeShader.setMat4("model", model);
    treeShader.setMat4("view", camera->GetViewMatrix());
    treeShader.setMat4("projection", camera->GetProjectionMatrix());
    //set directional lighitng
    glm::vec3 lightLevel = glm::vec3(1.0);
    glm::vec3 diffuseColor = glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * lightLevel;
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.7);
    treeShader.setVec3("dir_light.ambient", ambientColor);
    treeShader.setVec3("dir_light.diffuse", diffuseColor);
    treeShader.setVec3("dir_light.specular", glm::vec3(0.0) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * glm::vec3(1.0));
    treeShader.setVec3("dir_light.lightPos", Light::position);
    treeShader.setVec3("dir_light.direction", Light::direction);
    treeShader.setInt("num_points", 0);
    treeShader.setVec3("viewPos", camera->GetPosition());
    treeShader.setFloat("fogStart", ImguiHelper::fogStart);
    treeShader.setFloat("fogEnd", ImguiHelper::fogEnd);
    treeShader.setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3] ));
    treeShader.setFloat("fogStart", ImguiHelper::fogStart);
    treeShader.setFloat("fogEnd", ImguiHelper::fogEnd);
    treeShader.setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    treeShader.setVec3("viewPos", camera->GetPosition());
    tree->drawInstances(treeShader, 50);
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // reset viewport
    glViewport(0, 0, Window::Width, Window::Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    /**********************************************************
    *******  RENDER PASS 2 -- WATER REFLECTION TEXTURE  *******
    ***********************************************************/
    glm::vec3 cameraPos = camera->GetPosition();
    float pitch = camera->GetPitch();

    //move camera underneath
    cameraPos.y *= -1;
    pitch *= -1;
    camera->SetPosition(cameraPos);
    camera->SetPitch(pitch);

    waterFrameBuffer->BindReflectionBuffer();
    waterFrameBuffer->Clear();
    //enable clip plane
    glCheckError();
    terrain->shader->use();
    terrain->shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    terrain->shader->setInt("shadowMap", 5);
    terrain->shader->setFloat("fogStart", ImguiHelper::fogStart);
    terrain->shader->setFloat("fogEnd", ImguiHelper::fogEnd);
    terrain->shader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    terrain->shader->setVec3("viewPos", camera->GetPosition());
    terrain->shader->setBool("useFog", ImguiHelper::useFog);
    terrain->grassShader->use();
    terrain->grassShader->setFloat("fogStart", ImguiHelper::fogStart);
    terrain->grassShader->setFloat("fogEnd", ImguiHelper::fogEnd);
    terrain->grassShader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    terrain->grassShader->setVec3("viewPos", camera->GetPosition());
    terrain->grassShader->setBool("useFog", ImguiHelper::useFog);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glCheckError();
    terrain->Render(camera, deltaTime);
    glCheckError();
    shader->use();
    shader->setVec4("clipPlane", glm::vec4(0, 1, 0, 0));
    shader->setFloat("fogStart", ImguiHelper::fogStart);
    shader->setFloat("fogEnd", ImguiHelper::fogEnd);
    shader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    shader->setVec3("viewPos", camera->GetPosition());
    shader->setBool("useFog", ImguiHelper::useFog);
    model = glm::mat4(1.0);
    model = glm::translate(model, cubePos);
    shader->setFloat("textureTiling", 1);
    shader->setVec3("directionalLight.color", Light::color);
    shader->setFloat("directionalLight.intensity", Light::intensity);
    shader->setVec3("directionalLight.direction", Light::direction);
    shader->setMat4("projection", camera->GetProjectionMatrix());
    shader->setMat4("view", camera->GetViewMatrix());
    shader->setMat4("model", model);
    shader->setInt("textureSampler", 0);
    glCheckError();
    renderCube(cubeTexture, true, glm::vec4(0, 1, 0, 0));
    glCheckError();
    model = glm::mat4(1.0);
    model = glm::translate(model, castlePos);
    model = glm::scale(model, glm::vec3(castleScale));
    model = glm::rotate(model, glm::radians(castleRotAngle), castleRot);
    shader->setMat4("model", model);
    glCheckError();
    castle->draw(*shader);
    glCheckError();
    glUseProgram(0);
    //disable clip plane
    treeShader.use();
    model = glm::mat4(1.0);
    treeShader.setMat4("model", model);
    treeShader.setMat4("view", camera->GetViewMatrix());
    treeShader.setMat4("projection", camera->GetProjectionMatrix());
    //set directional lighitng
    treeShader.setVec3("dir_light.ambient", ambientColor);
    treeShader.setVec3("dir_light.diffuse", diffuseColor);
    treeShader.setVec3("dir_light.specular", glm::vec3(0.0) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * glm::vec3(1.0));
    treeShader.setVec3("dir_light.lightPos", Light::position);
    treeShader.setVec3("dir_light.direction", Light::direction);
    treeShader.setBool("useFog", ImguiHelper::useFog);
    //point lighting
    treeShader.setInt("num_points", 0);

    treeShader.setVec3("viewPos", camera->GetPosition());
    treeShader.setFloat("fogStart", ImguiHelper::fogStart);
    treeShader.setFloat("fogEnd", ImguiHelper::fogEnd);
    treeShader.setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    glCheckError();
    tree->drawInstances(treeShader, 50);
    glCheckError();
    glUseProgram(0);
    waterFrameBuffer->UnbindBuffer();

    //move camera back
    cameraPos.y *= -1;
    pitch *= -1;
    camera->SetPosition(cameraPos);
    camera->SetPitch(pitch);

    glCheckError();
    /**********************************************************
    *******  RENDER PASS 3 -- WATER REFRACTION TEXTURE  *******
    ***********************************************************/
    waterFrameBuffer->BindRefractionBuffer();
    waterFrameBuffer->Clear();
    //Render terrain
    terrain->shader->use();
    terrain->shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    terrain->shader->setInt("shadowMap", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    terrain->Render(camera, deltaTime);

    //Render Cube and Castle
    shader->use();
    shader->setVec4("clipPlane", glm::vec4(0, -1, 0, 0));
    model = glm::mat4(1.0);
    model = glm::translate(model, cubePos);
    shader->setFloat("textureTiling", 1);
    shader->setVec3("directionalLight.color", Light::color);
    shader->setFloat("directionalLight.intensity", Light::intensity);
    shader->setVec3("directionalLight.direction", Light::direction);
    shader->setMat4("projection", camera->GetProjectionMatrix());
    shader->setMat4("view", camera->GetViewMatrix());
    shader->setMat4("model", model);
    shader->setInt("textureSampler", 0);
    renderCube(cubeTexture, true, glm::vec4(0, -1, 0, 0));
    model = glm::mat4(1.0);
    model = glm::translate(model, castlePos);
    model = glm::scale(model, glm::vec3(castleScale));
    model = glm::rotate(model, glm::radians(castleRotAngle), castleRot);
    shader->setMat4("model", model);
    castle->draw(*shader);

    //Render Trees
    treeShader.use();
    model = glm::mat4(1.0);
    treeShader.setMat4("model", model);
    treeShader.setMat4("view", camera->GetViewMatrix());
    treeShader.setMat4("projection", camera->GetProjectionMatrix());
    //set directional lighitng
    treeShader.setVec3("dir_light.ambient", ambientColor);
    treeShader.setVec3("dir_light.diffuse", diffuseColor);
    treeShader.setVec3("dir_light.specular", glm::vec3(0.0) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * glm::vec3(1.0));
    treeShader.setVec3("dir_light.lightPos", Light::position);
    treeShader.setVec3("dir_light.direction", Light::direction);
    //point lighting
    treeShader.setInt("num_points", 0);
    treeShader.setVec3("viewPos", camera->GetPosition());
    treeShader.setFloat("fogStart", ImguiHelper::fogStart);
    treeShader.setFloat("fogEnd", ImguiHelper::fogEnd);
    treeShader.setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3] ));

    tree->drawInstances(treeShader, 50);

    glUseProgram(0);
    waterFrameBuffer->UnbindBuffer();

    glCheckError();



    /**********************************************************
    *******        RENDER PASS 4 -- NORMAL RENDER       *******
    ***********************************************************/
    //Render terrain
    terrain->shader->use();
    terrain->shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    terrain->shader->setInt("shadowMap", 5);
    terrain->shader->setFloat("gamma", ImguiHelper::gamma);
    terrain->shader->setBool("useGamma", ImguiHelper::useGamma);
    terrain->shader->setFloat("fogStart", ImguiHelper::fogStart);
    terrain->shader->setFloat("fogEnd", ImguiHelper::fogEnd);
    terrain->shader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    terrain->shader->setVec3("viewPos", camera->GetPosition());
    terrain->grassShader->use();
    terrain->grassShader->setFloat("gamma",ImguiHelper::gamma);
    terrain->grassShader->setBool("useGamma", ImguiHelper::useGamma);
    terrain->grassShader->setFloat("fogStart", ImguiHelper::fogStart);
    terrain->grassShader->setFloat("fogEnd", ImguiHelper::fogEnd);
    terrain->grassShader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    terrain->grassShader->setVec3("viewPos", camera->GetPosition());
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    terrain->Render(camera, deltaTime);    

    //Render cube
    shader->use();
    shader->setVec4("clipPlane", glm::vec4(0, 0, 0, 0));
    model = glm::mat4(1.0);
    model = glm::translate(model, cubePos);
    shader->setFloat("textureTiling", 1);
    shader->setVec3("directionalLight.color", Light::color);
    shader->setFloat("directionalLight.intensity", Light::intensity);
    shader->setVec3("directionalLight.direction", Light::direction);
    shader->setMat4("projection", camera->GetProjectionMatrix());
    shader->setMat4("view", camera->GetViewMatrix());
    shader->setMat4("model", model);
    shader->setInt("textureSampler", 0);
    shader->setFloat("fogStart", ImguiHelper::fogStart);
    shader->setFloat("fogEnd", ImguiHelper::fogEnd);
    shader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    shader->setVec3("viewPos", camera->GetPosition());
    renderCube(cubeTexture, true, glm::vec4(0, -1, 0, 0));

    //Render Castle
    directionalShader->use();
    model = glm::mat4(1.0);
    model = glm::translate(model, castlePos);
    model = glm::scale(model, glm::vec3(castleScale));
    model = glm::rotate(model, glm::radians(castleRotAngle), castleRot);
    directionalShader->setMat4("model", model);
    directionalShader->setVec3("viewPos", camera->GetPosition());
    directionalShader->setMat4("view", camera->GetViewMatrix());
    directionalShader->setMat4("projection", camera->GetProjectionMatrix());
    directionalShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    directionalShader->setFloat("gamma", ImguiHelper::gamma);
    directionalShader->setBool("useGamma", ImguiHelper::useGamma);
    directionalShader->setFloat("fogStart", ImguiHelper::fogStart);
    directionalShader->setFloat("fogEnd", ImguiHelper::fogEnd);
    directionalShader->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    directionalShader->setVec3("viewPos", camera->GetPosition());
    directionalShader->setBool("useFog", ImguiHelper::useFog);
    directionalShader->setVec3("lightPos", Light::position);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, castleNormal);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    castle->draw(*directionalShader);


    //Render Trees
    treeShader.use();
    model = glm::mat4(1.0);
    treeShader.setMat4("model", model);
    treeShader.setMat4("view", camera->GetViewMatrix());
    treeShader.setMat4("projection", camera->GetProjectionMatrix());
    //set directional lighitng
    treeShader.setVec3("dir_light.ambient", ambientColor);
    treeShader.setVec3("dir_light.diffuse", diffuseColor);
    treeShader.setVec3("dir_light.specular", glm::vec3(0.0)* glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2])* glm::vec3(1.0));
    treeShader.setVec3("dir_light.specular", glm::vec3(0.0)* glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2])* glm::vec3(1.0));
    treeShader.setVec3("dir_light.lightPos", Light::position);
    treeShader.setVec3("dir_light.direction", Light::direction);
    //point lighting
    treeShader.setInt("num_points", 0);
    treeShader.setVec3("viewPos", camera->GetPosition());
    treeShader.setFloat("fogStart", ImguiHelper::fogStart);
    treeShader.setFloat("fogEnd", ImguiHelper::fogEnd);
    treeShader.setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    treeShader.setVec3("viewPos", camera->GetPosition());
    treeShader.setFloat("gamma", ImguiHelper::gamma);
    treeShader.setBool("useGamma", ImguiHelper::useGamma);

    tree->drawInstances(treeShader, 50);
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);

    //Render Water
    water->GetShader()->use();
    water->GetShader()->setFloat("gamma", ImguiHelper::gamma);
    water->GetShader()->setBool("useGamma", ImguiHelper::useGamma);
    water->GetShader()->setVec3("viewPos", camera->GetPosition());
    water->GetShader()->setFloat("fogStart", ImguiHelper::fogStart);
    water->GetShader()->setFloat("fogEnd", ImguiHelper::fogEnd);
    water->GetShader()->setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    water->Render(camera, waterFrameBuffer);
}



























// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(UP, deltaTime);
        else
            camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(DOWN, deltaTime);
        else
            camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path:" << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadHeightmap(char const* path, int* width, int* height) {
    unsigned int texture;
    glGenTextures(1, &texture);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int nrChannels;
    std::cout << "here" << std::endl;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    stbi_us* data = stbi_load_16(path, width, height, &nrChannels, 4);
    std::cout << "here2" << std::endl;
    if (data)
    {
        std::cout << "here3" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data); // Notice GL_UNSIGNED_SHORT
        std::cout << "here4" << std::endl;

        //glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Loaded heightmap of size " << *height << " x " << *width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

#endif
