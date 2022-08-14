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

	unsigned int grassAtlas;
	unsigned int cliffTexture;
	unsigned int dirtTexture;
	unsigned int windMap;
	unsigned int normalMap;


	Shader* shader;
	Shader* grassShader;

	Terrain(const char* heightmapPath, bool useAlternativeTextureLoad = false);

	void SetShader(Shader* shader);
	void SetGrassShader(Shader* grassShader);

	void SetGrassAtlas(unsigned int grassAtlas);
	void SetCliffTexture(unsigned int cliffTexture);
	void SetDirtTexture(unsigned int dirtTexture);
	void SetWindMap(unsigned int windMap);
	void SetNormalMap(unsigned int normalMap);

	void Render(Camera* camera, float deltaTime);
	void DrawGrass(Camera* camera, float deltaTime);

	static unsigned int LoadHeightmap(const char* heightmapPath, int* width, int* height);
	static unsigned int AlternativeLoadHeightmap(const char* path, int* width, int* height);

};

