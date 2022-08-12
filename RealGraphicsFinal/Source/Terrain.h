#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include "Camera.h"
#include "shader_s.h"

class Terrain {
public:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int heightmap;
	int width;
	int height;

	static unsigned int resolution;

	Shader* shader;

	Terrain(const char* heightmapPath);

	void SetShader(Shader* shader);

	void Render(Camera* camera);
	static unsigned int LoadHeightmap(const char* heightmapPath, int* width, int* height);

};

