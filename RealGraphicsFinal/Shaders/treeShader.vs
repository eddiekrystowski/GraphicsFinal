#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;

const vec3 treePositions[50] = vec3[50](
	vec3(10.0, 0.0, 0.0),
	//southern forest
	vec3(-120.0, 11.0, 0.0),
	vec3(-120.0, 12.0, -5.0),
	vec3(-120.0, 12.0, -10.0),
	vec3(-125.0, 11.0, 2.5),
	vec3(-125.0, 12.0, -2.50),
	vec3(-125.0, 12.0, -7.5),
	vec3(-125.0, 12.0, 7.5),
	vec3(-125.0, 12.0, -12.5),
		vec3(-125.0, 12.0, -17.5),
		vec3(-125.0, 13.0, -22.5),
		vec3(-125.0, 13.0, -17.5),
		vec3(-125.0, 12.0, -32.5),
		vec3(-125.0, 12.0, -37.5),
		vec3(-125.0, 11.0, -42.5),
		vec3(-125.0, 10.0, -47.5),
	vec3(-125.0, 9.0, -50.0),
	vec3(-125.0, 8.0, -55.0),
	vec3(-125.0, 8.0, -60.0),
	vec3(-125.0, 7.0, -65.0),
	vec3(-125.0, 7.0, -70.0),
	vec3(-125.0, 7.0, -85.0),
	vec3(-125.0, 7.0, -90.0),
	// ----	
	vec3(-120.0, 9.0, -52.5),
	vec3(-120.0, 8.0, -57.5),
	vec3(-120.0, 8.0, -62.5),
	vec3(-120.0, 7.0, -67.5),
	vec3(-120.0, 7.0, -72.5),
	vec3(-120.0, 7.0, -87.5),
	vec3(-115.0, 8.0, -52.5),
	vec3(-115.0, 7.0, -57.0),
	vec3(-115.0, 7.0, -62.0),
	vec3(-115.0, 6.0, -67.0),
	vec3(-115.0, 6.0, -72.0),
	vec3(-115.0, 6.0, -87.0),
	vec3(-105.0, 8.0, 20.0),
	vec3(-105.0, 7.0, -40.0),
	vec3(-120.0, 10.0, -10.0),
	vec3(-120.0, 10.0, -37.0),
	vec3(-120.0, 10.0, -42.5),
	vec3(-120.0, 10.0, -47.5),
	vec3(95.0, 42.0, -100.0),
	vec3(96.0, 43.0, -95.0),
	vec3(80.0, 37.0, -85.0),
	vec3(-10.0, 10.0, 0.0),
	vec3(-7.0, 12.0, 5.0),
	vec3(-10.0, 10.0, 5.0),
	vec3(-10.0, 10.0, 10.0),
	vec3(-3.0, 11.0, 15.0),
	vec3(-14.0, 10.0, 15.0)
);

void main()
{
	gl_Position = projection * view * model * vec4(aPos + treePositions[gl_InstanceID], 1.0);
	//gl_Position = vec4(pos.x + treePositions[gl_InstanceID].x, pos.y + treePositions[gl_InstanceID].y, pos.z + treePositions[gl_InstanceID].z, pos.w);
	FragPos = vec3(model * vec4(aPos+treePositions[gl_InstanceID], 1.0));
	Normal = transpose(inverse(mat3(model))) * aNormal;
	TexCoords = aTexCoords;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}