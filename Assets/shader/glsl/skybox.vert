#version 450

// Input

layout (location = 0) in vec3 vertexPosition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// Output

out vec3 texCoordFrag;

void main()
{
	mat4 vp = projectionMatrix * viewMatrix;

	texCoordFrag = vertexPosition;
	
	vec4 pos = vp * vec4(vertexPosition, 1.0);
	gl_Position = pos;
}