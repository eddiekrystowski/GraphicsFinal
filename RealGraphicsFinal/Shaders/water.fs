// Fragment Shader 

#version 400

in DATA {
	vec4 worldPosition;
	vec2 textureCoord;
	vec3 normal;
	vec3 toCamera;
	vec3 fromLight;
} In;

uniform sampler2D reflectionSampler;
uniform sampler2D refractionSampler;
uniform sampler2D dudvSampler;
uniform sampler2D normalSampler;
uniform sampler2D depthSampler;

uniform float moveFactor;
uniform float distorsionStrength;
uniform float specularPower;
uniform vec3 lightColor;
uniform float near;
uniform float far;

uniform float gamma;
uniform bool useGamma;

uniform vec3 viewPos;
uniform float fogStart;
uniform float fogEnd;
uniform vec4 fogColor;
uniform bool useFog;

out vec4 outColor;

void main() {
	//calculate fog
    float fog = clamp( (  length(In.worldPosition.xyz - viewPos) / (fogEnd - fogStart)), 0.0, 1.0);
	// calculate texture coordinates based on ndc coordinates
	vec2 normalizedDeviceCoord = (In.worldPosition.xy / In.worldPosition.w) / 2.0 + 0.5;
	vec2 reflectionTextureCoord = vec2(normalizedDeviceCoord.x, -normalizedDeviceCoord.y);
	vec2 refractionTextureCoord = vec2(normalizedDeviceCoord.x, normalizedDeviceCoord.y);

	// calculate distance from object to clipping plane
	float terrainDepth = texture(depthSampler, refractionTextureCoord).r;
	float terrainDistance = 2.0 * near * far / (far + near - (2.0 * terrainDepth - 1.0) * (far - near));

	//calculate depth
	float fragmentDepth = gl_FragCoord.z;
	float fragmentDistance = 2.0 * near * far / (far + near - (2.0 * fragmentDepth - 1.0) * (far - near));

	//calculate water depth
	float waterDepth = terrainDistance - fragmentDistance;

	//distort water dudv (surface)
	vec2 distortedTexCoords = texture(dudvSampler, vec2(In.textureCoord.x + moveFactor, In.textureCoord.y)).rg * 0.1;
	distortedTexCoords = In.textureCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	// have to multiply * 2 - 1 here to get range -1 to 1 (ndc)
	vec2 totalDistortion = (texture(dudvSampler, distortedTexCoords).rg * 2.0 - 1.0) * distorsionStrength * clamp(waterDepth / 20.0, 0.0, 1.0);

	//update reflection and refraction tex coords based on distortion
	refractionTextureCoord += totalDistortion;
	refractionTextureCoord = clamp(refractionTextureCoord, 0.001, 0.999);

	reflectionTextureCoord += totalDistortion;
	reflectionTextureCoord.x = clamp(reflectionTextureCoord.x, 0.001, 0.999);
	reflectionTextureCoord.y = clamp(reflectionTextureCoord.y, -0.999, -0.001);

	vec4 reflectColor = texture(reflectionSampler, reflectionTextureCoord);
	vec4 refractColor = texture(refractionSampler, refractionTextureCoord);

	// calculate normal
	vec4 normalMapColor = texture(normalSampler, distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);

	//fresnel effect
	vec3 viewVector = normalize(In.toCamera);
	float refractiveFactor = dot(viewVector, normal);
	refractiveFactor = pow(refractiveFactor, 0.5);
	refractiveFactor = clamp(refractiveFactor, 0.001, 0.999);

	vec3 reflectedLight = reflect(normalize(In.fromLight), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, specularPower);
	vec3 specularHighlights = lightColor * specular * 0.5;

	//finally mix together
	outColor = mix(reflectColor, refractColor, refractiveFactor);
	outColor = mix(outColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHighlights, 0);
	outColor.a = clamp(waterDepth / 5.0, 0.0, 1.0);

	if (useFog) outColor = mix(outColor, fogColor, 0.4 * fog);
	if (useGamma) outColor.rgb = pow(outColor.rgb, vec3(1.0/gamma));
	
}