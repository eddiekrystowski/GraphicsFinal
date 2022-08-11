#pragma once
#include "Texture.h"
#include "Vertex.h"
#include "shader_s.h"
#include <vector>
#include <string>
class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    //constructors
    Mesh();
    Mesh(std::vector<Vertex> aVertices, std::vector<unsigned int> aIndices, std::vector<Texture> aTextures);

    //render
    void draw(Shader& shader);
    void drawGrass(Shader& shader);
    void drawInstances(Shader& shader, int n);

private:
    unsigned int VAO, VBO, EBO; //, grassVAO, grassVBO;

    void configure();
};