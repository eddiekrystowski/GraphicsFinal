#version 430 core

in float Height;

out vec4 FragColor;
in vec2 texCoord;
in vec4 position;

uniform sampler2D dirtTexture;
uniform sampler2D cliffTexture;
uniform sampler2D heightMap;
uniform sampler2D normalMap;

uniform float water_level;

uniform vec3 cameraPosition;

struct directionalLight {
    vec3 direction;
    vec3 lightPos;

    //Phong shading values
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform directionalLight dir_light;

vec3 directional(sampler2D sampler, vec3 patchNormal, int textureTiling) {
    //calculate ambient
    vec3 ambient = dir_light.ambient * texture(sampler, texCoord*textureTiling).rgb;

    //calculate diffuse
    vec3 bumpMapNormal = texture(normalMap, texCoord*textureTiling).rgb; 
    bumpMapNormal = normalize(bumpMapNormal * 2.0 - 1.0); 
    vec3 normal = (bumpMapNormal - vec3(0.0, 0.0, 1.0)) + patchNormal;
    //normal = patchNormal;
    vec3 lightDir = normalize(dir_light.lightPos - vec3(position));
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = dir_light.diffuse * diff * texture(sampler, texCoord*textureTiling).rgb;

    //calculate specular
    vec3 viewDir = normalize(cameraPosition - vec3(position));
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = 0.0f;
          spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = dir_light.specular * spec * texture(sampler, texCoord*textureTiling).rgb;

    /*
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;  
    float currentDepth = projCoords.z; 
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    if(projCoords.z > 1.0)
        shadow = 0.0;
    */

    return (ambient  + /*  (1-shadow) * */  (diffuse + specular));
}


void main()
{   
    float texelsize = 1.0 / 30.0;
    float height_max = 256.0;
    float left  = texture(heightMap, texCoord + vec2(-texelsize, 0.0)).r * height_max * 2.0 - 1.0;
    float right = texture(heightMap, texCoord + vec2( texelsize, 0.0)).r * height_max * 2.0 - 1.0;
    float up    = texture(heightMap, texCoord + vec2(0.0,  texelsize)).r * height_max * 2.0 - 1.0;
    float down  = texture(heightMap, texCoord + vec2(0.0, -texelsize)).r * height_max * 2.0 - 1.0;
    vec3 normal = normalize(vec3(down - up, 2.0, left - right));
    float value = dot(normal, vec3(0.0, 1.0, 0.0));
    if (value > 0.1) {
        FragColor = texture(dirtTexture, texCoord*10);//vec4(directional(dirtTexture, normal, 80), 1.0); 
    } else {
        FragColor = vec4(directional(cliffTexture, normal, 10), 1.0);
    }
    
}