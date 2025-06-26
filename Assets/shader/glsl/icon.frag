#version 450

// Input

in vec2 UV;

uniform sampler2D albedoMap;
uniform vec3 color;

uniform float alphaCutoff = 0.3;

// Ouput

out vec4 fragColor;

void main(){

	vec4 diffuseTexture = texture(albedoMap, UV);

	if (diffuseTexture.a < alphaCutoff)
		discard;

	// Output fragColor = color of the texture at the specified UV
	fragColor = diffuseTexture * vec4(color, 1.0);
	
	//fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}