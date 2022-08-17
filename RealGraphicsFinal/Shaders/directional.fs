#version 330 core
out vec4 FragColor; 

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct pointLight {
    vec3 position;

    //Phong shading values
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //values for attenuation (for point lighting only)
    float constant;
    float linear;
    float quadratic;
};

struct directionalLight {
    vec3 direction;
    vec3 lightPos;

    //Phong shading values
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
 
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform pointLight pointLights[4];
uniform int num_points;
uniform directionalLight dir_light;
uniform Material material;
uniform vec3 viewPos;

uniform float fogStart;
uniform float fogEnd;
uniform vec4 fogColor;

uniform sampler2D shadowMap;

vec3 directional() {
    //calculate ambient
    vec3 ambient = dir_light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    //calculate diffuse
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(dir_light.lightPos - FragPos);
    vec3 lightDir = normalize(-dir_light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dir_light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

    //calculate specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = 0.0f;
          spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = dir_light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;  
    float currentDepth = projCoords.z; 
    float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return (ambient + (1-shadow) * (diffuse + specular));
}

vec3 point(pointLight point) {
   vec3 norm = normalize(Normal);

   // calculate ambient
   vec3 ambient = point.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

   //calculate diffuse
   vec3 lightDir = normalize(point.position.xyz - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = point.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

   //calculate specular
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = point.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

   // calculate attenuation
   float distance = length(point.position - FragPos);
   float attenuation = 1.0 / (point.constant + point.linear * distance + point.quadratic * distance * distance);
   
   //factor attenutation
   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;

   return (ambient + diffuse);// + specular);
}

void main()
{   
    //calculate fog
    float fog = clamp( (  length(FragPos.xyz - viewPos) / (fogEnd - fogStart)), 0.0, 1.0);

    //calculate directional lighting
    vec3 directional_light = directional();

    //calculate point lighting
    vec3 point_lighting = vec3(0.0);
    if (num_points > 0) {
        for (int i = 0; i < num_points; i++) {
            point_lighting += point(pointLights[i]);
        }
    }

    // combine directional and point lighting
    vec3 total = directional_light + point_lighting;

    FragColor = vec4(directional_light, 1.0);
    //FragColor = mix(vec4(total, 1.0), fogColor, fog);
}