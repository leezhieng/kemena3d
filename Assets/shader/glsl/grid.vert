#version 330 core

layout (location = 0) in vec3 vertexPosition;

out vec4 vertexPositionFrag;
out vec3 viewPosFrag;
out vec2 texCoordFrag;
out mat4 modelMatrixFrag;
out float grid_sizeFrag;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;

const float grid_size = 100.0f;

void main ()
{
	vertexPositionFrag = vec4(vertexPosition, 1.0);
	viewPosFrag = viewPos;
	modelMatrixFrag = modelMatrix;
	
	grid_sizeFrag = grid_size;
	
	vec4 world_pos = vec4(vertexPosition, 1.0);
    world_pos.xyz *= grid_size;
	
	// Follow camera view
	world_pos.xz  += viewPos.xz;
	
	vec4 position = projectionMatrix * viewMatrix * world_pos;
    vec2 coords = world_pos.xz;
	texCoordFrag = coords;
	
	gl_Position = position;
}