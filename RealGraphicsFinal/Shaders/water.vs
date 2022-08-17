// Vertex Shader

#version 430

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoord;


out DATA {
	vec4 worldPosition;
	vec2 textureCoord;
	vec3 normal;
	vec3 toCamera;
	vec3 fromLight;
} Out;

uniform float textureTiling;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;

void main() {
	vec4 worldPosition = model * inPosition;
	vec4 positionRelativeToCamera = view * worldPosition;

	Out.worldPosition = projection * positionRelativeToCamera;
	Out.textureCoord = inTextureCoord * textureTiling;
	Out.normal = inNormal;
	Out.toCamera = cameraPosition - worldPosition.xyz;
	Out.fromLight = worldPosition.xyz - lightPosition;

	gl_Position = Out.worldPosition;
}