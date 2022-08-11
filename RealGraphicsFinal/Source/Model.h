#pragma once
#include "Mesh.h"
#include <vector>
#include <string>
#include "assimp/scene.h"
#include "Texture.h"

class Model {
public:
	//constructors
	Model();
	Model(std::string path);

	void draw(Shader& shader);
	void drawGrass(Shader& shader);
	void drawInstances(Shader& shader, int n);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> loaded_textures;

	int load(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

};