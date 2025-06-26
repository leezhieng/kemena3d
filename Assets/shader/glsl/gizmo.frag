#version 450

in vec3 vertexPositionFrag;

out vec4 fragColor;

struct Material {
    vec3 ambient;
    float shininess;
}; 
uniform Material material;

void main()
{
    fragColor = vec4(material.ambient, 1.0) * material.shininess;
}