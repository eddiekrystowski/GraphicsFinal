#version 430 core
out vec4 FragColor;

in GS_OUT {
    vec2 textureCoords;
    float color_variance;
    float value;
} fs_in;

uniform sampler2D grass_texture;
uniform sampler2D heightMap;
uniform bool showBackgrounds;

void main()
{
    if (fs_in.value <= 0.07) discard;
    vec4 color = texture(grass_texture, fs_in.textureCoords); 
    if (color.a < 0.2 && !showBackgrounds) discard;
    color.xyz = mix(color.xyz, 0.5*color.xyz, fs_in.color_variance);
    if (color.a < 0.15 && showBackgrounds) color = vec4(fs_in.color_variance);
    FragColor = color;
}