#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Mesh.h"
#include "shader_s.h"
#include "Texture.h"
#include "Camera.h"
#include "WaterFrameBuffer.h"

class Water {
private:
	Mesh* mesh;
	Shader* shader;
	unsigned int dudv;
	unsigned int normal;
	float moveFactor;

public:
	Water();
	~Water();

	float textureTiling;
	float distorsionStrength;
	float moveSpeed;
	float specularPower;

	void SetMesh(Mesh* mesh);
	void SetShader(Shader* shader);
	void SetDUDV(unsigned int texture);
	void SetNormal(unsigned int normal);

	Mesh* GetMesh();
	Shader* GetShader();
	unsigned int GetDUDV();
	unsigned int GetNormal();
	float GetMoveFactor();

	void Update(float deltaTime);
	void Render(Camera* camera, WaterFrameBuffer* waterFrameBuffer);

	static Mesh* GenerateMesh(glm::vec2 size);
};