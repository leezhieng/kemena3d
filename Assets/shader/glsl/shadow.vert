#version 330 core

// Input

layout (location = 0) in vec3 vertexPosition;
//layout (location = 1) in vec3 vertexColor;
//layout (location = 2) in vec2 texCoord;
//layout (location = 3) in vec3 vertexNormal;
//layout (location = 4) in vec3 vertexTangent;
//layout (location = 5) in vec3 vertexBitangent;
layout (location = 6) in ivec4 boneIDs; 
layout (location = 7) in vec4 weights;

uniform mat4 lightSpaceMatrix;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
//uniform vec3 viewPos;

const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

// Output

out vec3 vertexPositionFrag;

void main()
{
	// Bone animation
	vec4 totalPosition = vec4(vertexPosition, 1.0f);
	float totalWeight = 0.0;
	
    for(int i = 0 ; i < MAX_BONE_INFLUENCE; i++)
    {
		int boneID = boneIDs[i];  // Access correct bone ID
		float weight = weights[i];  // Get corresponding weight
		
        if(boneID == -1 || weight <= 0.0)  // Skip invalid influences
			continue;
		
        if(boneID >= MAX_BONES)  // Safety check
		{
			totalPosition = vec4(vertexPosition, 1.0f);
			break;
		}
		
        // Apply bone transformation
		totalPosition += (finalBonesMatrices[boneID] * vec4(vertexPosition, 1.0f)) * weight;
		
		mat3 normalMatrixBone = transpose(inverse(mat3(finalBonesMatrices[boneID])));
		
		totalWeight += weight;
    }
	
	// Fallback for non-skinned (static) model
	if (totalWeight == 0.0)
	{
		totalPosition = vec4(vertexPosition, 1.0f);
	}
	
	mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
	
	//vertexPositionFrag = (mvp * totalPosition).xyz;
	
	vec4 worldPosition = modelMatrix * totalPosition;
	vertexPositionFrag = (lightSpaceMatrix * worldPosition).xyz;
	
    gl_Position = lightSpaceMatrix * worldPosition;
}