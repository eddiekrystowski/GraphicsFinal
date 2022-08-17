#include "Water.h"
#include "Mesh.h"
#include<vector>
#include "WaterFrameBuffer.h"

glm::vec3 light_position = glm::vec3(1024, 200, 3072);
glm::vec3 light_color = glm::vec3(1, 1, 1);
float light_intensity = 0.35f;
glm::vec3 light_direction = glm::vec3(-1, -1, 0);

Water::Water() {
	textureTiling = 1;
	moveFactor = 0;
	moveSpeed = 1.0f / 40.0f;
	distorsionStrength = 0.04f;
	specularPower = 20.0f;
}

Water::~Water() {}

void Water::SetMesh(Mesh* mesh) {
	this->mesh = mesh;
}

void Water::SetShader(Shader* shader) {
	this->shader = shader;
}

void Water::SetDUDV(unsigned int texture) {
	this->dudv = texture;
}
void Water::SetNormal(unsigned int texture) {
	this->normal = texture;
}

Mesh* Water::GetMesh() {
	return mesh;
}

Shader* Water::GetShader() {
	return shader;
}

unsigned int Water::GetDUDV() {
	return dudv;
}

unsigned int Water::GetNormal() {
	return normal;
}

float Water::GetMoveFactor() {
	return moveFactor;
}

void Water::Update(float deltaTime) {
	moveFactor += deltaTime * moveSpeed;
	moveFactor = fmod(moveFactor, 1.0f);
}

Mesh* Water::GenerateMesh(glm::vec2 size) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i < 2; ++i) {
		float x = i * size.x;
		for (int j = 0; j < 2; ++j) {
			float z = j * size.y;
			Vertex vertex;
			vertex.Position = glm::vec4(x, 0, z, 1);
			vertex.TexCoords = glm::vec2(i, j);
			vertex.Normal = glm::vec3(0, 1, 0);
			vertices.push_back(vertex);
		}
	}
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);

	return new Mesh(vertices, indices);
}

void Water::Render(Camera* camera, WaterFrameBuffer* waterFrameBuffer) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader* shader = this->GetShader();
	Mesh* mesh = this->GetMesh();
	//VertexArray* vArray = this->GetMesh()->GetVertexArray();
	//IndexBuffer* iBuffer = this->GetMesh()->GetIndexBuffer();

	shader->use();
	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterFrameBuffer->GetReflectionTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterFrameBuffer->GetRefractionTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->GetDUDV());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, this->GetNormal());
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, waterFrameBuffer->GetRefractionDepthTexture());

	shader->setFloat("near", camera->GetNearClip());
	shader->setFloat("far", camera->GetFarClip());
	shader->setVec3("lightPosition", light_position);
	shader->setVec3("lightColor", light_color);
	shader->setVec3("cameraPosition", camera->GetPosition());
	shader->setFloat("moveFactor", this->GetMoveFactor());
	shader->setFloat("textureTiling", this->textureTiling);
	shader->setFloat("distorsionStrength", this->distorsionStrength);
	shader->setFloat("specularPower", this->specularPower);
	shader->setMat4("projection", camera->GetProjectionMatrix());
	shader->setMat4("view", camera->GetViewMatrix());
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-128, 0, -128));
	shader->setMat4("model", model);


	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);

	//unbind refraction depth texture
	glBindTexture(GL_TEXTURE_2D, 0);
	//unbind normal
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	//unbind dudv
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	//unbind refraction texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	//unbind reflection texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//unbind VAO
	glBindVertexArray(0);
	//unbind ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//stop using shader
	glUseProgram(0);

	glDisable(GL_BLEND);
}