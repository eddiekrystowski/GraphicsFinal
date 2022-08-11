#version 430 core

// vertex position
layout (location = 0) in vec3 aPos;
// texture coordinate
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec4 position;

uniform mat4 model;

void main()
{
    // convert XYZ vertex to XYZW homogeneous coordinate
    gl_Position = vec4(aPos, 1.0);
    position = model * vec4(aPos, 1.0);

    // pass texture coordinate though
    TexCoord = aTex;
}