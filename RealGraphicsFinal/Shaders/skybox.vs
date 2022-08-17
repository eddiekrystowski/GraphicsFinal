#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;
out vec4 FragPos;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
    //vec4 position = projection * view * vec4(aPos, 1.0);
    //gl_Position = position.xyzw;
    FragPos = pos.xyzw;
}