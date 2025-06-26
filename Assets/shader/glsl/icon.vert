#version 450

// Input

layout(location = 0) in vec3 vertexPosition;

uniform vec3 cameraRightWorldSpace;
uniform vec3 cameraUpWorldSpace;
uniform mat4 viewProjection;
uniform vec3 billboardPosition;
uniform vec2 billboardSize;

// Output

out vec2 UV;

void main()
{
	vec3 particleCenter_wordspace = billboardPosition;
	
	vec3 vertexPosition_worldspace = particleCenter_wordspace + cameraRightWorldSpace * vertexPosition.x * billboardSize.x + cameraUpWorldSpace * vertexPosition.y * billboardSize.y;

	// UV of the vertex. No special space for this one.
	UV = vertexPosition.xy + vec2(0.5, 0.5);
	
	// Output position of the vertex
	gl_Position = viewProjection * vec4(vertexPosition_worldspace, 1.0f);
}