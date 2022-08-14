#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 12) out;

out GS_OUT {
	vec2 textureCoords;
	float color_variance;
	float value;
} gs_out;

in vec4 position[];
in vec2 texCoord[];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler2D windMap;
uniform sampler2D heightMap;
uniform sampler2D pathTexture;
uniform float time;

// uniforms from imgui settings
uniform bool useWind;
uniform bool useMultipleTextures;

uniform float windspeed;
uniform float min_grass_height;
uniform float grass_height_factor;

uniform float grass1_density;
uniform float grass2_density;
uniform float grass3_density;
uniform float grass4_density;
uniform float flower1_density;
uniform float flower2_density;
uniform float flower3_density;
uniform float flower4_density;
uniform bool drawGrass1;
uniform bool drawGrass2;
uniform bool drawGrass3;
uniform bool drawGrass4;
uniform bool drawFlower1;
uniform bool drawFlower2;
uniform bool drawFlower3;
uniform bool drawFlower4;
	
uniform float waterlevel;
// ----

mat4 rotateY(float angle);
mat4 rotateX(float angle);
mat4 rotateZ(float angle);

float random(vec2 val);
float noise(vec2 st);
float fbm(vec2 st);

float threshold(int num) {
	int ctr = 0;
	float total = 0.0;
	if (drawGrass1) total += grass1_density;
	ctr++;
	if(ctr == num) return total;
	if (drawGrass2) total += grass2_density;
	ctr++;
	if(ctr == num) return total;
	if (drawGrass3) total += grass3_density;
	ctr++;
	if(ctr == num) return total;
	if (drawGrass4) total += grass4_density;
	ctr++;
	if(ctr == num) return total;
	if (drawFlower1) total += flower1_density;
	ctr++;
	if(ctr == num) return total;
	if (drawFlower2) total += flower2_density;
	ctr++;
	if(ctr == num) return total;
	if (drawFlower3) total += flower3_density;
	ctr++;
	if(ctr == num) return total;
	if (drawFlower4) total += flower4_density;
	ctr++;
	if(ctr == num) return total;
	return total;
}

float calculateTotalDensity() {
	float total = 0.0;
	if (drawGrass1) total += grass1_density;
	if (drawGrass2) total += grass2_density;
	if (drawGrass3) total += grass3_density;
	if (drawGrass4) total += grass4_density;
	if (drawFlower1) total += flower1_density;
	if (drawFlower2) total += flower2_density;
	if (drawFlower3) total += flower3_density;
	if (drawFlower4) total += flower4_density;
	return total;
}

bool texturesUsed() {
	return drawGrass1 || drawGrass2 || drawGrass3 || drawGrass4 || drawFlower1 || drawFlower2 || drawFlower3 || drawFlower4; 
}

void createQuad(vec4 position,float angle) {
	//rotation matrix
	mat4 rotation = rotateY(angle);
	//add randomness to rotation
	mat4 randomness = rotateY(180*random(position.xz));
	//generate height
	float growth_factor = random(position.xz) * (1.0 - min_grass_height) + min_grass_height;

	//get wind
	vec2 windDirection = vec2(1.0, 1.0); 	
	vec2 uv = position.xz/10.0 + windDirection * windspeed * time;
	uv.x = mod(uv.x,1.0); 
	uv.y = mod(uv.y,1.0);
	vec4 wind = texture(windMap, uv); 
	float bias = -0.25;
	mat4 windModel = rotateX(wind.x + bias) * rotateZ(wind.y + bias);
	if (!useWind) windModel = mat4(1.0);
	mat4 wind_rotation = mat4(1.0);

	//positions
	vec4 offsets[4];
	float randX = (random(position.xz) - 0.5);
	offsets[0] = vec4(-0.5 + randX, 0.0, 0.0, 0.0);
	offsets[1] = vec4( 0.5 + randX, 0.0, 0.0, 0.0);	
	offsets[2] = vec4(-0.5 + randX, 1.0 * growth_factor * grass_height_factor, 0.0, 0.0);
	offsets[3] = vec4( 0.5 + randX, 1.0 * growth_factor * grass_height_factor, 0.0, 0.0);	

	//texture coordinates
	vec2 texCoords[4];
	if (useMultipleTextures) {
		float totalDensity = calculateTotalDensity();
			float randFloat = totalDensity*random(position.xz);
			if (randFloat < threshold(1)) {
				texCoords[0] = vec2(0.0, 0.5); // bottom left
				texCoords[1] = vec2(0.25 , 0.5); // bottom right
				texCoords[2] = vec2(0.0, 1.0); // top left
				texCoords[3] = vec2(0.25, 1.0); // top right
			} else if (randFloat < threshold(2)){
				texCoords[0] = vec2(0.25, 0.5); // bottom left
				texCoords[1] = vec2(0.5 , 0.5); // bottom right
				texCoords[2] = vec2(0.25, 1.0); // top left
				texCoords[3] = vec2(0.5, 1.0); // top right
			} else if (randFloat < threshold(3)){
				texCoords[0] = vec2(0.5, 0.5); // bottom left
				texCoords[1] = vec2(0.75 , 0.5); // bottom right
				texCoords[2] = vec2(0.5, 1.0); // top left
				texCoords[3] = vec2(0.75, 1.0); // top right
			} else if (randFloat < threshold(4)){
				texCoords[0] = vec2(0.75, 0.5); // bottom left
				texCoords[1] = vec2(1.0 , 0.5); // bottom right
				texCoords[2] = vec2(0.75, 1.0); // top left
				texCoords[3] = vec2(1.0, 1.0); // top right
			} else if (randFloat < threshold(5)){
				texCoords[0] = vec2(0.0, 0.0); // bottom left
				texCoords[1] = vec2(0.25 , 0.0); // bottom right
				texCoords[2] = vec2(0.0, 0.5); // top left
				texCoords[3] = vec2(0.25, 0.5); // top right
			} else if (randFloat < threshold(6)){
				texCoords[0] = vec2(0.25, 0.0); // bottom left
				texCoords[1] = vec2(0.5 , 0.0); // bottom right
				texCoords[2] = vec2(0.25, 0.5); // top left
				texCoords[3] = vec2(0.5, 0.5); // top right
			} else if (randFloat < threshold(7)){
				texCoords[0] = vec2(0.5, 0.0); // bottom left
				texCoords[1] = vec2(0.75 , 0.0); // bottom right
				texCoords[2] = vec2(0.5, 0.5); // top left
				texCoords[3] = vec2(0.75, 0.5); // top right
			} else if (randFloat < threshold(8)){
				texCoords[0] = vec2(0.75, 0.0); // bottom left
				texCoords[1] = vec2(1.0 , 0.0); // bottom right
				texCoords[2] = vec2(0.75, 0.5); // top left
				texCoords[3] = vec2(1.0, 0.5); // top right
			} else {
				texCoords[0] = vec2(0.0, 0.0); // bottom left
				texCoords[1] = vec2(0.0 , 0.0); // bottom right
				texCoords[2] = vec2(0.0, 0.0); // top left
				texCoords[3] = vec2(0.0, 0.0); // top right
			}
	} else {
		texCoords[0] = vec2(0.0, 0.5); // bottom left
		texCoords[1] = vec2(0.25 , 0.5); // bottom right
		texCoords[2] = vec2(0.0, 1.0); // top left
		texCoords[3] = vec2(0.25, 1.0); // top right
	}

	for (int i = 0; i < 4; i++) {
		float texelsize = 1.0 / 30.0;
		float height_max = 256.0;
		float left  = texture(heightMap, texCoord[0] + vec2(-texelsize, 0.0)).r * height_max * 2.0 - 1.0;
		float right = texture(heightMap, texCoord[0] + vec2( texelsize, 0.0)).r * height_max * 2.0 - 1.0;
		float up    = texture(heightMap, texCoord[0] + vec2(0.0,  texelsize)).r * height_max * 2.0 - 1.0;
		float down  = texture(heightMap, texCoord[0] + vec2(0.0, -texelsize)).r * height_max * 2.0 - 1.0;
		vec3 normal = normalize(vec3(down - up, 2.0, left - right));
		if (texCoord[0].x <= 10.0/256.0 || (texCoord[0].x >= 246.0/256.0 && texCoord[0].y >= 71.0/256.0) || (texCoord[0].y <= 10.0/256.0 && texCoord[0].x  <= 185.0/256.0) || texCoord[0].y >= 240.0/256.0) {
			normal = vec3(0.0, 1.0, 0.0);
		}
		gs_out.value = dot(normal, vec3(0.0, 1.0, 0.0));
		// apply wind rotation to top two corners
		if (i == 2) wind_rotation = windModel;
		//set position
		gl_Position = gl_in[0].gl_Position + projection * view * model * (vec4(0.0, 0.0, 0.0, 1.0) + wind_rotation * randomness * rotation * offsets[i]);
		//set texture coordinates
		gs_out.textureCoords = texCoords[i];
		gs_out.color_variance = fbm(position.xz);
		EmitVertex();
	}
	EndPrimitive();
}

void main() {
	if (position[0].y < waterlevel) return;
	if (texture(pathTexture, texCoord[0]).r > 0.2)  return;
	if (texCoord[0].x <= 1.0/256.0 || texCoord[0].x >= 255.0/256.0) return;
    if (texCoord[0].y <= 1.0/256.0 || texCoord[0].y >= 255.0/256.0) return;
	// world space position acts as seed for randomness (should be unique and will not change based on view)
	createQuad(position[0] , 0.0);
	createQuad(position[0] , 45.0);
	createQuad(position[0] , -45.0);
}

mat4 rotateY(float angle) {
	return mat4(	cos(angle),		0.0,		sin(angle),			0.0,
					0.0,			1.0,		0.0,				0.0,
					-sin(angle),	0.0,		cos(angle),			0.0,
					0.0,			0.0,		0.0,				1.0
				);
}

mat4 rotateX(float angle) {
		return mat4(	1.0,			0.0,		0.0,				0.0,
						0.0,			cos(angle),		-sin(angle),	0.0,
						0.0,			sin(angle),		cos(angle),		0.0,
						0.0,			0.0,		0.0,				1.0
				);
}

mat4 rotateZ(float angle) {
		return mat4(	cos(angle),		-sin(angle),	0.0,		0.0,
						sin(angle),		cos(angle),		0.0,		0.0,
						0.0,			0.0,			1.0,		0.0,
						0.0,			0.0,			0.0,		1.0
				);
}

float random(vec2 val){
    return fract(sin(dot(val, vec2(12.9898, 78.233))) * 43758.5453);
}

float noise (in vec2 st) {
	vec2 i = floor(st);
	vec2 f = fract(st);
	// Four corners in 2D of a tile
	float a = random(i);
	float b = random(i + vec2(1.0, 0.0));
	float c = random(i + vec2(0.0, 1.0));
	float d = random(i + vec2(1.0, 1.0));
	// Smooth Interpolation
	vec2 u = f*f*(3.0-2.0*f);
	// Mix 4 coorners percentages
	return mix(a, b, u.x) +
	(c - a)* u.y * (1.0 - u.x) +
	(d - b) * u.x * u.y;
}

#define NUM_OCTAVES 5
float fbm ( in vec2 _st) {
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100.0);
	// Rotate to reduce axial bias
	mat2 rot = mat2(cos(0.5), sin(0.5),
	-sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(_st);
		_st = rot * _st * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}