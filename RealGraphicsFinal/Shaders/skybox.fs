#version 430 core
out vec4 FragColor;

uniform samplerCube skybox;
uniform float fogStart;
uniform float fogEnd;
uniform vec4 fogColor;

in vec4 FragPos;
in vec3 TexCoords;

void main()
{   
    float fog = clamp(((fogEnd - length(FragPos.xyz)) / (fogEnd - fogStart)), 0.0, 1.0);
    FragColor = mix(fogColor, texture(skybox, TexCoords).rgba, fog);
}   