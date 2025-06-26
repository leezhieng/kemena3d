#version 450

in vec2 texCoordFrag;

out vec4 fragColor;

uniform sampler2D albedoMap;

void main()
{
	vec4 diffuseTexture = texture(albedoMap, texCoordFrag);
	
    fragColor = diffuseTexture;
}