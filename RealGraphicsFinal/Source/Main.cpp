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

//#define final_project
//#define final_project
#ifdef final_project
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadHeightmap(const char* path, int* height, int* width);
void renderScene(Shader& shader, Model rhinocer, Model tree);
void renderCube();
void renderQuad();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera* camera = new Camera(Projection::Perspective, 45, (float) SCR_WIDTH / SCR_HEIGHT);
//Camera camera(glm::vec3(67.0f, 20.0f, 169.9f), glm::vec3(0.0f, 1.0f, 0.0f), -128.1f, -42.4f);
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// meshes
unsigned int planeVAO;

glm::vec3 RGB(glm::vec3 color) {
    return glm::vec3(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f);
}

glm::vec3 RGB(float v0, float v1, float v2) {
    return glm::vec3(v0 / 255.0f, v1 / 255.0f, v2 / 255.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        camera->pause();
        if (camera->paused) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    const char* _source;
    const char* _type;
    const char* _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n",
        id, _type, _severity, _source, msg);
}

void drawPlane(glm::vec3 position, glm::vec3 scale, Shader& shader, bool pointMode) {
    glm::mat4 plane_model = glm::mat4(1.0f);
    plane_model = glm::translate(plane_model, position);
    plane_model = glm::scale(plane_model, scale);
    shader.use();

    shader.setMat4("model", plane_model);
    glBindVertexArray(planeVAO);
    if (pointMode == true) {
        glDrawArrays(GL_POINTS, 0, 6);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void generatePlanes(unsigned int layers, Shader& shader, bool drawGrass) {
    shader.use();
    shader.setFloat("time", (float)glfwGetTime());
    //settings from imgui
    shader.setBool("useWind", ImguiHelper::wind);
    shader.setBool("useMultipleTextures", ImguiHelper::multipleTextures);
    shader.setFloat("windspeed", ImguiHelper::windspeed);
    shader.setFloat("min_grass_height", ImguiHelper::min_grass_height);
    shader.setFloat("grass_height_factor", ImguiHelper::grass_height_factor);
    shader.setFloat("grass1_density", ImguiHelper::grass1_density);
    shader.setFloat("grass2_density", ImguiHelper::grass2_density);
    shader.setFloat("grass3_density", ImguiHelper::grass3_density);
    shader.setFloat("grass4_density", ImguiHelper::grass4_density);
    shader.setFloat("flower1_density", ImguiHelper::flower1_density);
    shader.setFloat("flower2_density", ImguiHelper::flower2_density);
    shader.setFloat("flower3_density", ImguiHelper::flower3_density);
    shader.setFloat("flower4_density", ImguiHelper::flower4_density);
    shader.setBool("drawGrass1", ImguiHelper::drawGrass1);
    shader.setBool("drawGrass2", ImguiHelper::drawGrass2);
    shader.setBool("drawGrass3", ImguiHelper::drawGrass3);
    shader.setBool("drawGrass4", ImguiHelper::drawGrass4);
    shader.setBool("drawFlower1", ImguiHelper::drawFlower1);
    shader.setBool("drawFlower2", ImguiHelper::drawFlower2);
    shader.setBool("drawFlower3", ImguiHelper::drawFlower3);
    shader.setBool("drawFlower4", ImguiHelper::drawFlower4);
    shader.setBool("showBackgrounds", ImguiHelper::showBackgrounds);
    for (int i = 0; i < layers; i++) {
        for (int j = 0; j < layers; j++) {
            if (drawGrass && ImguiHelper::renderGrass) {
                drawPlane(glm::vec3(0.5 * i, 1.0, 0.5 * j), glm::vec3(1.0f), shader, true);
            }
            else {
                drawPlane(glm::vec3(0.5 * i, 1.0, 0.5 * j), glm::vec3(1.0/125.0f), shader, false);
            }
        }
    }
}

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Final Project", NULL, NULL);
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

    WaterFrameBuffer* waterFrameBuffer = new WaterFrameBuffer();


    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback(GLDebugMessageCallback, NULL);

    // build and compile shaders
    // -------------------------
    Shader* shader = new Shader("./Shaders/directional.vs", "./Shaders/directional.fs", nullptr, nullptr, nullptr);
    Shader* simpleDepthShader = new Shader("./Shaders/depthShader.vs", "./Shaders/depthShader.fs", nullptr, nullptr, nullptr);
    Shader* debugDepthQuad = new Shader("./Shaders/debug_quad.vs", "./Shaders/debug_quad.fs", nullptr, nullptr, nullptr);
    Shader* skyboxShader = new Shader("./Shaders/skybox.vs", "./Shaders/skybox.fs", nullptr, nullptr, nullptr);
    Shader* grassShader = new Shader("./Shaders/grass.vs", "./Shaders/grass.fs", "./Shaders/grass.gs", nullptr, nullptr);
    Shader* tessHeightMapShader = new Shader("./Shaders/passthrough.vs", "./Shaders/terrain.fs", nullptr, "./Shaders/tessellation_ground.tesc", "./Shaders/tessellation_ground.tese");
    Shader* tessHeightMapGrassShader = new Shader("./Shaders/passthrough.vs", "./Shaders/grass.fs", "./Shaders/grass.gs", "./Shaders/tessellation.tesc", "./Shaders/tessellation.tese");
    Shader* treeShader = new Shader("./Shaders/treeShader.vs", "./Shaders/treeShader.fs", nullptr, nullptr, nullptr);
    Shader* waterShader = new Shader("./Shaders/water.vs", "./Shaders/water.fs");


    // tree positions
    glm::vec3 treePositions[50] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(2.0f, 0.0f, 2.0f),
        glm::vec3(3.0f, 0.0f, 3.0f),
        glm::vec3(4.0f, 0.0f, 4.0f),
        glm::vec3(5.0f, 0.0f, 5.0f),
        glm::vec3(6.0f, 0.0f, 6.0f),
        glm::vec3(7.0f, 0.0f, 6.0f),
        glm::vec3(8.0f, 0.0f, 6.0f),
        glm::vec3(9.0f, 0.0f, 6.0f),
        glm::vec3(10.0f, 0.0f, 6.0f),
        glm::vec3(11.0f, 0.0f, 6.0f),
        glm::vec3(12.0f, 0.0f, 6.0f),
        glm::vec3(13.0f, 0.0f, 6.0f),
        glm::vec3(14.0f, 0.0f, 6.0f),
        glm::vec3(15.0f, 0.0f, 6.0f),
        glm::vec3(16.0f, 0.0f, 6.0f),
        glm::vec3(17.0f, 0.0f, 6.0f),
        glm::vec3(18.0f, 0.0f, 6.0f),
        glm::vec3(19.0f, 0.0f, 6.0f),
        glm::vec3(20.0f, 0.0f, 6.0f),
        glm::vec3(21.0f, 0.0f, 6.0f),
        glm::vec3(22.0f, 0.0f, 6.0f),
        glm::vec3(23.0f, 0.0f, 6.0f),
        glm::vec3(24.0f, 0.0f, 6.0f),
        glm::vec3(25.0f, 0.0f, 6.0f),
        glm::vec3(26.0f, 0.0f, 6.0f),
        glm::vec3(27.0f, 0.0f, 6.0f),
        glm::vec3(28.0f, 0.0f, 6.0f),
        glm::vec3(29.0f, 0.0f, 6.0f),
        glm::vec3(30.0f, 0.0f, 6.0f),
        glm::vec3(31.0f, 0.0f, 6.0f),
        glm::vec3(32.0f, 0.0f, 6.0f),
        glm::vec3(33.0f, 0.0f, 6.0f),
        glm::vec3(34.0f, 0.0f, 6.0f),
        glm::vec3(35.0f, 0.0f, 6.0f),
        glm::vec3(36.0f, 0.0f, 6.0f),
        glm::vec3(37.0f, 0.0f, 6.0f),
        glm::vec3(38.0f, 0.0f, 6.0f),
        glm::vec3(39.0f, 0.0f, 6.0f),
        glm::vec3(40.0f, 0.0f, 6.0f),
        glm::vec3(41.0f, 0.0f, 6.0f),
        glm::vec3(42.0f, 0.0f, 6.0f),
        glm::vec3(43.0f, 0.0f, 6.0f),
        glm::vec3(44.0f, 0.0f, 6.0f),
        glm::vec3(45.0f, 0.0f, 6.0f),
        glm::vec3(46.0f, 0.0f, 6.0f),
        glm::vec3(47.0f, 0.0f, 6.0f),
        glm::vec3(48.0f, 0.0f, 6.0f),
        glm::vec3(49.0f, 0.0f, 6.0f),
    };

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 50, &treePositions[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //setup skybox
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

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<float> vertices;
    int width, height;
    unsigned int height_map = loadHeightmap("./Textures/hmap6.png", &width, &height);
    width = 257;
    height = 257;
    unsigned rez = 30;
    for (unsigned i = 0; i <= rez - 1; i++)
    {
        for (unsigned j = 0; j <= rez - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back(j / (float)rez); // v
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            std::cout << -width / 2.0f + width * i / (float)rez << ", " << "0.0, " << -height / 2.0f + height * j / (float)rez << std::endl;

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            std::cout << -width / 2.0f + width * (i+1) / (float)rez << ", " << "0.0, " << -height / 2.0f + height * j / (float)rez << std::endl;



            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            std::cout << -width / 2.0f + width * i / (float)rez << ", " << "0.0, " << -height / 2.0f + height * (j+1) / (float)rez << std::endl;


            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            std::cout << -width / 2.0f + width * (i+1) / (float)rez << ", " << "0.0, " << -height / 2.0f + height * (j+1) / (float)rez << std::endl;

        }
    }
    std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
    std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

    // first, configure the cube's VAO (and terrainVBO)
    unsigned int terrainVAO, terrainVBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    //normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)5);
    glEnableVertexAttribArray(2);


    glPatchParameteri(GL_PATCH_VERTICES, 4);

    // load textures
    // -------------
    stbi_set_flip_vertically_on_load(true);
    //unsigned int woodTexture = loadTexture("./Textures/JC-OceanBlue.jpg");
    unsigned int 
    unsigned int grass_texture = loadTexture("./Textures/grass_atlas_noborder.png");
    unsigned int wind_map = loadTexture("./Textures/wind.jpg");
    unsigned int dirt = loadTexture("./Textures/dirttexture.png");
    unsigned int grassTexture = loadTexture("./Textures/dirttexture.png");

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
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

    Model rhinocer("./Models/castle_no_ground.obj");
    Model tree("./Models/tree_model2.obj");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 1);
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);
    tessHeightMapGrassShader.use();
    tessHeightMapGrassShader.setInt("grass_texture", 0);
    tessHeightMapGrassShader.setInt("windMap", 1);
    tessHeightMapGrassShader.setInt("heightMap", 2);
    tessHeightMapShader.use();
    tessHeightMapShader.setInt("heightMap", 0);
    tessHeightMapShader.setInt("dirtTexture", 1);
    tessHeightMapShader.setInt("grassTexture", 2);



    // lighting info
    // -------------
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    glm::vec3 lightLevel(1.0f, 1.0f, 1.0f);
    glm::vec4 skybox_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


    Mesh* waterMesh = Water::GenerateMesh(glm::vec2(256.0f, 256.0f));
    Shader* waterShader = new Shader("./Shaders/water.vs", "./Shaders/water.fs");



    //setup IMGUI
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glCheckError();


        // 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = -10.0f, far_plane = 7.5f;
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        //glCullFace(GL_FRONT);
        renderScene(simpleDepthShader, rhinocer, tree);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(- width / 2, 0, -height / 2));
       //model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
        glCheckError();

        simpleDepthShader.setMat4("model", model);
        glCheckError();

        waterMesh->draw(simpleDepthShader);
        glCheckError();

        tessHeightMapShader.use();

        // view/projection transformations
        glm::mat4 projection = camera->GetProjectionMatrix();//  glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera->GetViewMatrix();
        tessHeightMapShader.setMat4("projection", projection);
        tessHeightMapShader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        tessHeightMapShader.setMat4("model", model);


        // render the terrain
        glActiveTexture(GL_TEXTURE0);
        glCheckError();

        glBindTexture(GL_TEXTURE_2D, height_map); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        glCheckError();

        glBindVertexArray(terrainVAO);
        glCheckError();

        glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);

        glCheckError();
        //glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // new ImGui frame
        ImguiHelper::createFrame();

        // 2. render scene as normal using the generated depth/shadow map  
        // --------------------------------------------------------------

                // be sure to activate shader when setting uniforms/drawing objects


        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        projection = camera->GetProjectionMatrix();//glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera->GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        shader.setVec3("viewPos", camera->GetPosition());
        //shader.setVec3("lightPos", lightPos);
        shader.setVec3("dir_light.direction", -lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        //set fog
        shader.setFloat("fogStart", ImguiHelper::fogStart);
        shader.setFloat("fogEnd", ImguiHelper::fogEnd);
        shader.setVec4("fogColor", ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]);

        //set point lights
        shader.setInt("num_points", 4);
#define usepointlights
#ifdef usepointlights
        // point lights
        int num_point_lights = 4;
        shader.setInt("num_points", num_point_lights);
        //first point light
        shader.setVec3("pointLights[0].position", glm::vec3(-14.286f / 2.0f, 6.032f / 2.0f, -3.492f / 2.0f));
        shader.setVec3("pointLights[0].ambient", glm::vec3(0.5f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setVec3("pointLights[0].diffuse", glm::vec3(0.7f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setVec3("pointLights[0].specular", glm::vec3(0.9f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setFloat("pointLights[0].constant", 1.0f);
        shader.setFloat("pointLights[0].linear", 1.5f);
        shader.setFloat("pointLights[0].quadratic", 5.0f);
        //second point light
        shader.setVec3("pointLights[1].position", glm::vec3(-19.365f / 2.0f, 6.032f / 2.0f, -3.492f / 2.0f));
        shader.setVec3("pointLights[1].ambient", glm::vec3(0.1f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setVec3("pointLights[1].diffuse", glm::vec3(0.4f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setVec3("pointLights[1].specular", glm::vec3(0.4f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setFloat("pointLights[1].constant", 1.0f);
        shader.setFloat("pointLights[1].linear", 1.5f);
        shader.setFloat("pointLights[1].quadratic", 5.0f);
        //third point light
        shader.setVec3("pointLights[2].position", glm::vec3(-14.286f / 2.0f, 6.032f / 2.0f, -7.937f / 2.0f));
        shader.setVec3("pointLights[2].ambient", glm::vec3(0.1f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setVec3("pointLights[2].diffuse", glm::vec3(0.4f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setVec3("pointLights[2].specular", glm::vec3(0.4f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setFloat("pointLights[2].constant", 1.0f);
        shader.setFloat("pointLights[2].linear", 1.5f);
        shader.setFloat("pointLights[2].quadratic", 5.0f);
        //fourth point light
        shader.setVec3("pointLights[3].position", glm::vec3(-21.270 / 2.0f, 6.032 / 2.0f, -6.667 / 2.0f));
        shader.setVec3("pointLights[3].ambient", glm::vec3(0.1f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setVec3("pointLights[3].diffuse", glm::vec3(0.4f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setVec3("pointLights[3].specular", glm::vec3(0.4f) * RGB(224.0f, 117.0f, 67.0f));
        shader.setFloat("pointLights[3].constant", 1.0f);
        shader.setFloat("pointLights[3].linear", 1.5f);
        shader.setFloat("pointLights[3].quadratic", 5.0f);
#endif
        //set directional light
        shader.setVec3("dir_light.lightPos", lightPos);
        if (!ImguiHelper::timeOfDay) {
            shader.setVec3("dir_light.ambient", glm::vec3(0.1f) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]));
            shader.setVec3("dir_light.diffuse", glm::vec3(0.4f) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]));
            shader.setVec3("dir_light.specular", glm::vec3(1.0) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]));
        }
        else {
            shader.setVec3("dir_light.ambient", glm::vec3(0.01f) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * lightLevel);
            shader.setVec3("dir_light.diffuse", glm::vec3(0.4f) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * lightLevel);
            shader.setVec3("dir_light.specular", glm::vec3(1.0) * glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]) * lightLevel);
        }

        

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderScene(shader, rhinocer, tree);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-width / 2, 0, -height / 2));
        //model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
        shader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        waterMesh->draw(shader);

        tessHeightMapShader.use();

        // view/projection transformations
        projection = camera->GetProjectionMatrix();// glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        view = camera->GetViewMatrix();
        tessHeightMapShader.setMat4("projection", projection);
        tessHeightMapShader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        tessHeightMapShader.setMat4("model", model);
        tessHeightMapShader.setFloat("water_level", 0.0f);

        // render the terrain
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, height_map); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dirt);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, grassTexture);


        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);

        glCheckError();
        //generatePlanes(40, shader, false);

        
        tessHeightMapGrassShader.use();
        tessHeightMapGrassShader.setFloat("time", glfwGetTime());
        //settings from imgui
        tessHeightMapGrassShader.setBool("useWind", ImguiHelper::wind);
        tessHeightMapGrassShader.setBool("useMultipleTextures", ImguiHelper::multipleTextures);
        tessHeightMapGrassShader.setFloat("windspeed", ImguiHelper::windspeed);
        tessHeightMapGrassShader.setFloat("min_grass_height", ImguiHelper::min_grass_height);
        tessHeightMapGrassShader.setFloat("grass_height_factor", ImguiHelper::grass_height_factor);
        tessHeightMapGrassShader.setFloat("grass1_density", ImguiHelper::grass1_density);
        tessHeightMapGrassShader.setFloat("grass2_density", ImguiHelper::grass2_density);
        tessHeightMapGrassShader.setFloat("grass3_density", ImguiHelper::grass3_density);
        tessHeightMapGrassShader.setFloat("grass4_density", ImguiHelper::grass4_density);
        tessHeightMapGrassShader.setFloat("flower1_density", ImguiHelper::flower1_density);
        tessHeightMapGrassShader.setFloat("flower2_density", ImguiHelper::flower2_density);
        tessHeightMapGrassShader.setFloat("flower3_density", ImguiHelper::flower3_density);
        tessHeightMapGrassShader.setFloat("flower4_density", ImguiHelper::flower4_density);
        tessHeightMapGrassShader.setBool("drawGrass1", ImguiHelper::drawGrass1);
        tessHeightMapGrassShader.setBool("drawGrass2", ImguiHelper::drawGrass2);
        tessHeightMapGrassShader.setBool("drawGrass3", ImguiHelper::drawGrass3);
        tessHeightMapGrassShader.setBool("drawGrass4", ImguiHelper::drawGrass4);
        tessHeightMapGrassShader.setBool("drawFlower1", ImguiHelper::drawFlower1);
        tessHeightMapGrassShader.setBool("drawFlower2", ImguiHelper::drawFlower2);
        tessHeightMapGrassShader.setBool("drawFlower3", ImguiHelper::drawFlower3);
        tessHeightMapGrassShader.setBool("drawFlower4", ImguiHelper::drawFlower4);
        tessHeightMapGrassShader.setBool("showBackgrounds", ImguiHelper::showBackgrounds);

        model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(6.0f, 0.0f, -0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(2.0f));	// it's a bit too big for our scene, so scale it down
        tessHeightMapGrassShader.setMat4("model", model);
        tessHeightMapGrassShader.setMat4("projection", projection);
        tessHeightMapGrassShader.setMat4("view", view);
        tessHeightMapGrassShader.setMat4("inverseView", glm::inverse(view));
        tessHeightMapGrassShader.setMat4("inverseProjection", glm::inverse(projection));
        tessHeightMapGrassShader.setFloat("water_level", 0.0f);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grass_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, wind_map);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, height_map);
        //generatePlanes(40, tessHeightMapGrassShader, true);

        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);

        // render Depth map to quad for visual debugging
        // ---------------------------------------------
#ifdef debugging
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad();
#endif

        //draw skybox as last
        glDepthFunc(GL_LEQUAL); // need LEQUAL instead of LESS
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // undo translation
        view = glm::scale(view, glm::vec3(500));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setVec4("color", skybox_color);
        skyboxShader.setVec3("viewPos", camera->GetPosition());
        skyboxShader.setFloat("fogStart", ImguiHelper::fogStart);
        skyboxShader.setFloat("fogEnd", ImguiHelper::fogEnd);
        skyboxShader.setVec4("fogColor", ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        //draw skybox
        if (ImguiHelper::drawSkybox) glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set back to LESS

        //draw imgui
#define useImGUI
#ifdef useImGUI

        ImguiHelper::render();

        if (ImguiHelper::AutoTime) {
            ImguiHelper::time += ImguiHelper::timestep;
            ImguiHelper::time = fmod(ImguiHelper::time, 24.0f);
        }
        if (ImguiHelper::timeOfDay) {
            //calculate values based on the time of day
            lightLevel = glm::vec3(glm::clamp((1.0f / 2.0f) * (sin((M_PI / 12.0f) * (ImguiHelper::time)-(14.0f / 24.0f) * M_PI)) + 0.6f, 0.0, 0.9));
            glm::vec3 light_col;
            if (ImguiHelper::time <= 6 || ImguiHelper::time >= 18) {
                //blend night with sunup/down
                float delta = (ImguiHelper::time <= 6) ? ImguiHelper::time + 6 : (ImguiHelper::time - 18);
                glm::vec3 other = RGB(240.0f, 160.0f, 22.0f);
                float sun = glm::clamp((float)((1.0f / 2.0f) * (cos((M_PI / 6.0f) * (delta)) + 0.6)), 0.0f, 1.0f) * (0.15f);
                light_col = glm::mix(lightLevel, other, sun);
            }
            else {
                // time > 6 time < 18
                //blend sunup/sundown with mid day
                float delta = ImguiHelper::time - 6;
                glm::vec3 other = RGB(240.0f, 160.0f, 22.0f);
                float sun = glm::clamp((float)((1.0f / 2.0f) * (cos((M_PI / 6.0f) * (delta)) + 0.6)), 0.0f, 1.0f) * (0.15f);
                light_col = glm::mix(lightLevel, other, sun);
            }
            ImguiHelper::lightColor[0] = light_col[0];
            ImguiHelper::lightColor[1] = light_col[1];
            ImguiHelper::lightColor[2] = light_col[2];
            skybox_color = glm::vec4(glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2]), 1.0f);
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
#endif

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    ////end imgui
    ImguiHelper::shutdown();


    glfwTerminate();
    return 0;
}

// renders the 3D scene
// --------------------
void renderScene(Shader& shader, Model rhinocer, Model tree)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-20.0f, (1.0f + sin(glfwGetTime()) * 0.1) / 2.0f, -20.0f));
    model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    // 
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(6.0f, 0.0f, -0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
    shader.setMat4("model", model);
    rhinocer.draw(shader);
    tree.drawInstances(shader, 1);
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
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
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

// camera
Camera* camera = new Camera(Projection::Perspective, 45, (float) (Window::Width / (float)Window::Height));
float lastX = (float)Window::Width / 2.0;
float lastY = (float)Window::Height / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        camera->pause();
        if (camera->paused) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}


void renderScene(Shader* shader, unsigned int cubeTexture, WaterFrameBuffer* waterFrameBuffer, Water* water, Terrain* terrain, Shader& treeShader, Model* tree);
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
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    //sometimes this line says GLDebugMessageCallback is not a member of the Debug class,
    //this error can be ignored, the program will compile and run successfully
    glDebugMessageCallback(Debug::GLDebugMessageCallback, NULL);
   

    // build shader programs and setup uniforms
    // -------------------------
    Shader* waterShader = new Shader("./Shaders/water.vs", "./Shaders/water.fs");
    Shader* shader = new Shader("Shaders/Shader.vs", "Shaders/Shader.fs");
    Shader* directionalShader = new Shader("./Shaders/directional.vs", "./Shaders/directional.fs", nullptr, nullptr, nullptr);
    Shader* debugDepthQuad = new Shader("./Shaders/debug_quad.vs", "./Shaders/debug_quad.fs", nullptr, nullptr, nullptr);
    Shader* tessHeightMapShader = new Shader("./Shaders/passthrough.vs", "./Shaders/terrain.fs", nullptr, "./Shaders/tessellation_ground.tesc", "./Shaders/tessellation_ground.tese");
    Shader* tessHeightMapGrassShader = new Shader("./Shaders/passthrough.vs", "./Shaders/grass.fs", "./Shaders/grass.gs", "./Shaders/tessellation.tesc", "./Shaders/tessellation.tese");
    Shader* treeShader = new Shader("./Shaders/treeShader.vs", "./Shaders/directional.fs", nullptr, nullptr, nullptr);

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

 
    // load models
    Model tree = Model("./Models/treeModelLowPoly.obj");
    

    // lighting info
    // -------------
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    // initialize settings
    // -----------

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

        renderScene(shader, waterDudv, waterFrameBuffer, water, terrain, *treeShader, &tree);

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

void renderScene(Shader* shader, unsigned int cubeTexture, WaterFrameBuffer* waterFrameBuffer, Water* water, Terrain* terrain, Shader& treeShader, Model* tree) {

    glm::vec3 skyColor = glm::vec3(0.815f, 0.925f, 0.992f);
    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 cubePos = glm::vec3(0, 2, 0);

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
    terrain->Render(camera, deltaTime);
    shader->use();
    shader->setVec4("clipPlane", glm::vec4(0, 1, 0, 0));
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, cubePos);
    shader->setFloat("textureTiling", 1);
    shader->setVec3("directionalLight.color", Light::color);
    shader->setFloat("directionalLight.intensity", Light::intensity);
    shader->setVec3("directionalLight.direction", Light::direction);
    shader->setMat4("gProj", camera->GetProjectionMatrix());
    shader->setMat4("gCamera", camera->GetViewMatrix());
    shader->setMat4("gWorld", model);
    shader->setInt("textureSampler", 0);
    renderCube(cubeTexture, true, glm::vec4(0, 1, 0, 0));
    glUseProgram(0);
    //disable clip plane
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
    //point lighting
    treeShader.setInt("num_points", 0);

    treeShader.setVec3("viewPos", camera->GetPosition());
    treeShader.setFloat("fogStart", ImguiHelper::fogStart);
    treeShader.setFloat("fogEnd", ImguiHelper::fogEnd);
    treeShader.setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    tree->drawInstances(treeShader, 50);
    glUseProgram(0);
    waterFrameBuffer->UnbindBuffer();



    //debugDepthQuad.use();
    //debugDepthQuad.setFloat("near_plane", near_plane);
    //debugDepthQuad.setFloat("far_plane", far_plane);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, depthMap);
    //renderQuad();

    //move camera back
    cameraPos.y *= -1;
    pitch *= -1;
    camera->SetPosition(cameraPos);
    camera->SetPitch(pitch);

    // Render the scene in the refraction buffer
    waterFrameBuffer->BindRefractionBuffer();
    waterFrameBuffer->Clear();
    terrain->Render(camera, deltaTime);
    shader->use();
    shader->setVec4("clipPlane", glm::vec4(0, -1, 0, 0));
    model = glm::mat4(1.0);
    model = glm::translate(model, cubePos);
    shader->setFloat("textureTiling", 1);
    shader->setVec3("directionalLight.color", Light::color);
    shader->setFloat("directionalLight.intensity", Light::intensity);
    shader->setVec3("directionalLight.direction", Light::direction);
    shader->setMat4("gProj", camera->GetProjectionMatrix());
    shader->setMat4("gCamera", camera->GetViewMatrix());
    shader->setMat4("gWorld", model);
    shader->setInt("textureSampler", 0);
    renderCube(cubeTexture, true, glm::vec4(0, -1, 0, 0));
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


    terrain->Render(camera, deltaTime);    
    shader->use();
    shader->setVec4("clipPlane", glm::vec4(0, 0, 0, 0));
    model = glm::mat4(1.0);
    model = glm::translate(model, cubePos);
    shader->setFloat("textureTiling", 1);
    shader->setVec3("directionalLight.color", Light::color);
    shader->setFloat("directionalLight.intensity", Light::intensity);
    shader->setVec3("directionalLight.direction", Light::direction);
    shader->setMat4("gProj", camera->GetProjectionMatrix());
    shader->setMat4("gCamera", camera->GetViewMatrix());
    shader->setMat4("gWorld", model);
    shader->setInt("textureSampler", 0);
    renderCube(cubeTexture, true, glm::vec4(0, -1, 0, 0));
    treeShader.use();
    model = glm::mat4(1.0);
    treeShader.setMat4("model", model);
    treeShader.setMat4("view", camera->GetViewMatrix());
    treeShader.setMat4("projection", camera->GetProjectionMatrix());
    //set directional lighitng
    treeShader.setVec3("dir_light.ambient", ambientColor);
    treeShader.setVec3("dir_light.diffuse", diffuseColor);
    treeShader.setVec3("dir_light.specular", glm::vec3(0.0)* glm::vec3(ImguiHelper::lightColor[0], ImguiHelper::lightColor[1], ImguiHelper::lightColor[2])* glm::vec3(1.0));
    treeShader.setVec3("dir_light.lightPos", Light::position);
    treeShader.setVec3("dir_light.direction", Light::direction);
    //point lighting
    treeShader.setInt("num_points", 0);

    treeShader.setVec3("viewPos", camera->GetPosition());
    treeShader.setFloat("fogStart", ImguiHelper::fogStart);
    treeShader.setFloat("fogEnd", ImguiHelper::fogEnd);
    treeShader.setVec4("fogColor", glm::vec4(ImguiHelper::fogColor[0], ImguiHelper::fogColor[1], ImguiHelper::fogColor[2], ImguiHelper::fogColor[3]));
    tree->drawInstances(treeShader, 50);
    glUseProgram(0);


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
