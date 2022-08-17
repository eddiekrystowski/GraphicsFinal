#include "Model.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <iostream>
#include "stb_image.h"

unsigned int getTexture(const char* path, const std::string& directory, bool gamma=false) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

Model::Model(std::string path) {
	load(path);
}

int Model::load(std::string path) {
	Assimp::Importer importer;
	/*
	* aiProcessTriangulate : If the model is not comprised completely of triangular faces, convert all primitive shapes to triangles
	* aiProcess_FlipUVs : flip texture coordinates along the y axis.
	*/
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals  | aiProcess_FlipUVs | aiProcess_SplitLargeMeshes | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR: ASSIMP::" << importer.GetErrorString() << std::endl;
		return EXIT_FAILURE;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
	return EXIT_SUCCESS;
}

void Model::draw(Shader& shader) {
	//draw all meshes that make up model
	for (unsigned int i = 0; i < this->meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}

void Model::drawGrass(Shader& shader) {
	//draw all meshes that make up model
	for (unsigned int i = 0; i < this->meshes.size(); i++) {
		meshes[i].drawGrass(shader);
	}
}

void Model::drawInstances(Shader& shader, int n) {
	for (unsigned int i = 0; i < this->meshes.size(); i++) {
		meshes[i].drawInstances(shader, n);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// process every mesh for node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// process all children
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// gather vertex data
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		aiVector3D position = mesh->mVertices[i];
		vertex.Position = glm::vec3(position.x, position.y, position.z);
		aiVector3D normal = mesh->mNormals[i];
		vertex.Normal = glm::vec3(normal.x, normal.y, normal.z);
		if (mesh->mTextureCoords[0]) {
			// mesh contains texture coordinates
			aiVector3D textureCoords = mesh->mTextureCoords[0][i];
			vertex.TexCoords = glm::vec2(textureCoords.x, textureCoords.y);
			// tangent
			glm::vec3 vector;
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else {
			std::cout << "No Texture coordinates for Vertex " << i << std::endl;
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		// add vertex to vertices
		vertices.push_back(vertex);
	}
	// gather index data
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	// gather material data
	if (mesh->mMaterialIndex >= 0) {
		//mesh contains material
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	}
	// create and return mesh from data
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < loaded_textures.size(); j++)
		{
			if (std::strcmp(loaded_textures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(loaded_textures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = getTexture(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			loaded_textures.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}