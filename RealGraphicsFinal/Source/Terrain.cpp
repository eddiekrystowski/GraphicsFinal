#include "Terrain.h"

#include <iostream>
#include <vector>

unsigned int Terrain::resolution = 30;

Terrain::Terrain(const char* heightmapPath) {
    std::vector<float> vertices;
    heightmap = Terrain::LoadHeightmap("./Textures/hmap6.png", &width, &height);
    std::cout << "Loaded terrain of size " << width << "x" << height << std::endl;
    for (unsigned i = 0; i <= Terrain::resolution - 1; i++)
    {
        for (unsigned j = 0; j <= Terrain::resolution - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)Terrain::resolution); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)Terrain::resolution); // v.z
            vertices.push_back(i / (float)Terrain::resolution); // u
            vertices.push_back(j / (float)Terrain::resolution); // v
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            //std::cout << -width / 2.0f + width * i / (float)rez << ", " << "0.0, " << -height / 2.0f + height * j / (float)rez << std::endl;

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)Terrain::resolution); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)Terrain::resolution); // v.z
            vertices.push_back((i + 1) / (float)Terrain::resolution); // u
            vertices.push_back(j / (float)Terrain::resolution); // v
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            //std::cout << -width / 2.0f + width * (i + 1) / (float)rez << ", " << "0.0, " << -height / 2.0f + height * j / (float)rez << std::endl;



            vertices.push_back(-width / 2.0f + width * i / (float)Terrain::resolution); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)Terrain::resolution); // v.z
            vertices.push_back(i / (float)Terrain::resolution); // u
            vertices.push_back((j + 1) / (float)Terrain::resolution); // v
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            //std::cout << -width / 2.0f + width * i / (float)rez << ", " << "0.0, " << -height / 2.0f + height * (j + 1) / (float)rez << std::endl;


            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)Terrain::resolution); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)Terrain::resolution); // v.z
            vertices.push_back((i + 1) / (float)Terrain::resolution); // u
            vertices.push_back((j + 1) / (float)Terrain::resolution); // v
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            //std::cout << -width / 2.0f + width * (i + 1) / (float)rez << ", " << "0.0, " << -height / 2.0f + height * (j + 1) / (float)rez << std::endl;

        }
    }
    std::cout << "Loaded " << Terrain::resolution * Terrain::resolution << " patches of 4 control points each" << std::endl;
    std::cout << "Processing " << Terrain::resolution * Terrain::resolution * 4 << " vertices in vertex shader" << std::endl;

    // first, configure the cube's VAO (and terrainVBO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
}


void Terrain::SetShader(Shader* shader) {
    this->shader = shader;
}

void Terrain::Render(Camera* camera) {
    Shader* shader = this->shader;

    shader->use();

    std::cout << "Rendering terrain" << std::endl;

    // view/projection transformations
    glm::mat4 projection = camera->GetProjectionMatrix();//  glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
    glm::mat4 view = camera->GetViewMatrix();
    //std::cout << projection << std::endl;
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    shader->setMat4("model", model);


    // render the terrain
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, heightmap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    glBindVertexArray(VAO);

    glDrawArrays(GL_PATCHES, 0, 4 * Terrain::resolution * Terrain::resolution);
    std::cout << "done rendering terrain" << std::endl;
}








unsigned int Terrain::LoadHeightmap(const char* heightmapPath, int* width, int* height) {
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
    //need to use stbi load 16 because heightmap is 16 bits per color
    stbi_us* data = stbi_load_16(heightmapPath, width, height, &nrChannels, 4);
    if (data)
    {
        // Need to use GL_UNSIGNED_SHORT since each color is 16 bits == 2 bytes == 1 short, not an int (4 bytes)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);

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