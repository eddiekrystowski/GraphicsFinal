#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include "Camera.h"
#include "shader_s.h"
#include "ImguiHelper.h"

#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>

class Terrain {
public:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int heightmap;
	int width;
	int height;

	static unsigned int resolution;

	unsigned int grassTexture;
	unsigned int grassAtlas;
	unsigned int dirtTexture;
	unsigned int sandTexture;
	unsigned int windMap;


	Shader* shader;
	Shader* grassShader;

	Terrain(const char* heightmapPath);

	void SetShader(Shader* shader);
	void SetGrassShader(Shader* grassShader);

	void SetGrassTexture(unsigned int grassTexture);
	void SetGrassAtlas(unsigned int grassAtlas);

	void SetDirtTexture(unsigned int dirtTexture);
	void SetWindMap(unsigned int windMap);

	void Render(Camera* camera, float deltaTime);
	void DrawGrass(Camera* camera, float deltaTime);

	static unsigned int LoadHeightmap(const char* heightmapPath, int* width, int* height);

};

