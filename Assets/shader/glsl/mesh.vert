#version 330 core

// Input

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 vertexNormal;
layout (location = 4) in vec3 vertexTangent;
layout (location = 5) in vec3 vertexBitangent;
layout (location = 6) in ivec4 boneIDs; 
layout (location = 7) in vec4 weights;

uniform mat4 normalMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;
uniform mat4 lightSpaceMatrix;

const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

// Output

out vec3 vertexPositionFrag;
out vec3 vertexColorFrag;
out vec2 texCoordFrag;
out vec3 vertexNormalFrag;
//out vec3 vertexTangentFrag;
//out vec3 vertexBitangentFrag;
out vec4 lightSpaceMatrixFrag;

out vec3 T;
out vec3 B;
out vec3 N;

void main()
{
	// Bone animation
	vec4 totalPosition = vec4(vertexPosition, 1.0f);
	vec3 totalNormal = vec3(0.0);
	vec3 totalTangent = vec3(0.0);
	vec3 totalBitangent = vec3(0.0);
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
			totalNormal = vertexNormal;
			totalTangent = vertexTangent;
			totalBitangent = vertexBitangent;
			break;
		}
		
        // Apply bone transformation
		totalPosition += (finalBonesMatrices[boneID] * vec4(vertexPosition, 1.0f)) * weight;

		//vec3 localNormal = mat3(finalBonesMatrices[boneID]) * vertexNormal;
		//totalNormal += localNormal * weight;
		
		mat3 normalMatrixBone = transpose(inverse(mat3(finalBonesMatrices[boneID])));
		
		totalNormal += normalMatrixBone * vertexNormal * weight;
		totalTangent += normalMatrixBone * vertexTangent * weight;
		totalBitangent += normalMatrixBone * vertexBitangent * weight;
		
		totalWeight += weight;
    }
	
	// Fallback for non-skinned (static) model
	if (totalWeight == 0.0)
	{
		totalPosition = vec4(vertexPosition, 1.0f);
		totalNormal = vertexNormal;
		totalTangent = vertexTangent;
		totalBitangent = vertexBitangent;
	}

	mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
	
	vertexColorFrag = vertexColor;
	texCoordFrag = texCoord;
	
	// Never moved
	if (totalPosition == vec4(vertexPosition, 1.0f))
		vertexNormalFrag = vertexNormal;
	else
		vertexNormalFrag = totalNormal;
	
	//vertexTangentFrag = vertexTangent;
	//vertexBitangentFrag = vertexBitangent;
	
	//vertexPositionFrag = vec3(modelMatrix * vec4(vertexPosition, 1.0));
	vertexPositionFrag = vec3(modelMatrix * totalPosition);
	
	N = mat3(transpose(inverse(modelMatrix))) * totalNormal;
    T = mat3(transpose(inverse(modelMatrix))) * totalTangent;
    B = mat3(transpose(inverse(modelMatrix))) * totalBitangent;
	
	lightSpaceMatrixFrag = lightSpaceMatrix * vec4(vertexPositionFrag, 1.0);
	
	//gl_Position = mvp * vec4(vertexPosition, 1.0);
	gl_Position = mvp * totalPosition;
}