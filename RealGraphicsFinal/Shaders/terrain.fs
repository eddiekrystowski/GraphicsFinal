#version 430 core

in float Height;

out vec4 FragColor;

void main()
{
    float h = (Height + 16)/64.0f;
    vec3 col = vec3(h,h,h) / 4;
    FragColor = vec4(col, 1.0);
}