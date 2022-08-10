#version 330 core
out vec4 FragColor;

uniform vec4 color;
uniform float fogStart;
uniform float fogEnd;
uniform vec4 fogColor;

in vec4 FragPos;

void main()
{   
    float fog = clamp(((fogEnd - length(FragPos.xyz)) / (fogEnd - fogStart)), 0.0, 1.0);
    FragColor = mix(fogColor, color, fog);
}