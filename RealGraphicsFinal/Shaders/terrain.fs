#version 430 core

in float Height;

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D dirtTexture;
uniform sampler2D grassTexture;
uniform sampler2D heightMap;

uniform float water_level;


void main()
{   
    float texelsize = 1.0 / 30.0;
    float height_max = 256.0;
    float left  = texture(heightMap, texCoord + vec2(-texelsize, 0.0)).r * height_max * 2.0 - 1.0;
    float right = texture(heightMap, texCoord + vec2( texelsize, 0.0)).r * height_max * 2.0 - 1.0;
    float up    = texture(heightMap, texCoord + vec2(0.0,  texelsize)).r * height_max * 2.0 - 1.0;
    float down  = texture(heightMap, texCoord + vec2(0.0, -texelsize)).r * height_max * 2.0 - 1.0;
    vec3 normal = normalize(vec3(down - up, 2.0, left - right));
    float value = dot(normal, vec3(0.0, 1.0, 0.0));
    if (value > 0.1) {
        FragColor = texture(grassTexture, texCoord*50); 
    } else {
        FragColor = texture(dirtTexture, texCoord*50);
    }
    
}