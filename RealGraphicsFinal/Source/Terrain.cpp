#include "Terrain.h"

#include <iostream>
#include <vector>

unsigned int Terrain::resolution = 30;

Terrain::Terrain(const char* heightmapPath, bool useAlternativeTextureLoad) {
    std::vector<float> vertices;
    if (useAlternativeTextureLoad) {
        heightmap = Terrain::AlternativeLoadHeightmap(heightmapPath, &width, &height);
    }
    else {
        heightmap = Terrain::LoadHeightmap(heightmapPath, &width, &height);
    }
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

void Terrain::SetGrassShader(Shader* grassShader) {
    this->grassShader = grassShader;
}

void Terrain::SetGrassAtlas(unsigned int grassAtlas) {
    this->grassAtlas = grassAtlas;
}

void Terrain::SetGrassTexture(unsigned int grassTexture) {
    this->grassTexture = grassTexture;
}

void Terrain::SetDirtTexture(unsigned int dirtTexture) {
    this->dirtTexture = dirtTexture;
}

void Terrain::SetWindMap(unsigned int windMap) {
    this->windMap = windMap;
}

void Terrain::Render(Camera* camera, float deltaTime) {
    Shader* shader = this->shader;

    shader->use();

    // view/projection transformations
    glm::mat4 projection = camera->GetProjectionMatrix();//  glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
    glm::mat4 view = camera->GetViewMatrix();
    //std::cout << projection << std::endl;
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    //// world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, -16, 0)); // was -16
    shader->setMat4("model", model);
    shader->setFloat("water_level", 0.0f);

    // render the terrain
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, dirtTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, grassTexture);

    glBindVertexArray(VAO);

    glDrawArrays(GL_PATCHES, 0, 4 * Terrain::resolution * Terrain::resolution);

    DrawGrass(camera, deltaTime);

}


void Terrain::DrawGrass(Camera* camera, float deltaTime) {

    grassShader->use();
    grassShader->setFloat("time", glfwGetTime());
    //settings from imgui
    grassShader->setBool("useWind", ImguiHelper::wind);
    grassShader->setBool("useMultipleTextures", ImguiHelper::multipleTextures);
    grassShader->setFloat("windspeed", ImguiHelper::windspeed);
    grassShader->setFloat("min_grass_height", ImguiHelper::min_grass_height);
    grassShader->setFloat("grass_height_factor", ImguiHelper::grass_height_factor);
    grassShader->setFloat("grass1_density", ImguiHelper::grass1_density);
    grassShader->setFloat("grass2_density", ImguiHelper::grass2_density);
    grassShader->setFloat("grass3_density", ImguiHelper::grass3_density);
    grassShader->setFloat("grass4_density", ImguiHelper::grass4_density);
    grassShader->setFloat("flower1_density", ImguiHelper::flower1_density);
    grassShader->setFloat("flower2_density", ImguiHelper::flower2_density);
    grassShader->setFloat("flower3_density", ImguiHelper::flower3_density);
    grassShader->setFloat("flower4_density", ImguiHelper::flower4_density);
    grassShader->setBool("drawGrass1", ImguiHelper::drawGrass1);
    grassShader->setBool("drawGrass2", ImguiHelper::drawGrass2);
    grassShader->setBool("drawGrass3", ImguiHelper::drawGrass3);
    grassShader->setBool("drawGrass4", ImguiHelper::drawGrass4);
    grassShader->setBool("drawFlower1", ImguiHelper::drawFlower1);
    grassShader->setBool("drawFlower2", ImguiHelper::drawFlower2);
    grassShader->setBool("drawFlower3", ImguiHelper::drawFlower3);
    grassShader->setBool("drawFlower4", ImguiHelper::drawFlower4);
    grassShader->setBool("showBackgrounds", ImguiHelper::showBackgrounds);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -16.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(2.0f));	// it's a bit too big for our scene, so scale it down
    grassShader->setMat4("model", model);
    grassShader->setMat4("projection", camera->GetProjectionMatrix());
    grassShader->setMat4("view", camera->GetViewMatrix());
    grassShader->setFloat("water_level", 0.0f);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassAtlas);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, windMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, heightmap);
    //generatePlanes(40, tessHeightMapGrassShader, true);

    glBindVertexArray(VAO);
    glDrawArrays(GL_PATCHES, 0, 4 * Terrain::resolution * Terrain::resolution);
}



unsigned int Terrain::AlternativeLoadHeightmap(char const* path, int* width, int* height)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int nrComponents;
    unsigned char* data = stbi_load(path, width, height, &nrComponents, 0);
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
        glTexImage2D(GL_TEXTURE_2D, 0, format, *width, *height, 0, format, GL_UNSIGNED_BYTE, data);
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