#version 430 core

in float Height;

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D dirtTexture;
uniform sampler2D grassTexture;
uniform float water_level;

void main()
{   
    float h = (Height + 64)/256.0f;
    if (h > water_level) {
       FragColor = texture(grassTexture, texCoord*50); 
    } else {
       FragColor = texture(dirtTexture, texCoord*50); 
    }
    
}