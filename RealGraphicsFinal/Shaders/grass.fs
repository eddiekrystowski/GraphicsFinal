#version 430 core
out vec4 FragColor;

in GS_OUT {
    vec2 textureCoords;
    float color_variance;
    float value;
    vec3 position;
} fs_in;

uniform sampler2D grass_texture;
uniform sampler2D heightMap;
uniform bool showBackgrounds;

uniform float gamma;
uniform bool useGamma;

uniform vec3 viewPos;
uniform float fogStart;
uniform float fogEnd;
uniform vec4 fogColor;
uniform bool useFog;

void main()
{
    //calculate fog
    float fog = clamp( (  length(0.5*fs_in.position - viewPos)  / (fogEnd - fogStart)), 0.0, 1.0);
    if (fs_in.value <= 0.07) discard;
    vec4 color = texture(grass_texture, fs_in.textureCoords); 
    if (color.a < 0.2 && !showBackgrounds) discard;
    color.xyz = mix(color.xyz, 0.5*color.xyz, fs_in.color_variance);
    if (color.a < 0.15 && showBackgrounds) color = vec4(fs_in.color_variance);
    FragColor = color;
    if (useFog) FragColor = mix(FragColor, fogColor, fog);
    if (useGamma) FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
    
}