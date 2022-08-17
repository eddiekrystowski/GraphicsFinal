// Vertex Shader

#version 430

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoord;

out DATA {
	vec3 worldPosition;
	vec2 textureCoord;
	vec3 normal;
} Out;

uniform vec4 clipPlane;
uniform float textureTiling;

void main() {
	vec4 worldPosition = model * inPosition;
	vec4 positionRelativeToCamera = view * worldPosition;
	
	gl_Position = projection * positionRelativeToCamera;
	gl_ClipDistance[0] = dot(worldPosition, clipPlane);
	
	Out.worldPosition = worldPosition.xyz;
	Out.textureCoord = inTextureCoord * textureTiling;
	Out.normal = (model * vec4(inNormal, 0.0f)).xyz;
}