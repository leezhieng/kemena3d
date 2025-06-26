#version 150

in vec3 texCoordFrag;

out vec4 fragColor;

uniform samplerCube cubeMap;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

void main(void)
{
    fragColor = texture(cubeMap, texCoordFrag);
	
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}