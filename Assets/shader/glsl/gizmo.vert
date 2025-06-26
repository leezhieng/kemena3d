#version 450

// Input

layout (location = 0) in vec3 vertexPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// Output

out vec3 vertexPositionFrag;

void main()
{
	mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

	vertexPositionFrag = vec3(modelMatrix * vec4(vertexPosition, 1.0));
	
	gl_Position = mvp * vec4(vertexPosition, 1.0);
}