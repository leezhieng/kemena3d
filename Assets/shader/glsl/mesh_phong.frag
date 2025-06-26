#version 330 core

in vec3 vertexPositionFrag;
in vec3 vertexColorFrag;
in vec2 texCoordFrag;
in vec3 vertexNormalFrag;
//in vec3 vertexTangentFrag;
//in vec3 vertexBitangentFrag;

uniform mat4 normalMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;

in vec3 T;
in vec3 B;
in vec3 N;

out vec4 fragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;

uniform bool has_albedoMap;
uniform bool has_normalMap;
uniform bool has_specularMap;
uniform bool has_emissiveMap;

uniform float alphaCutoff = 0.2;

struct Material {
	vec2 tiling;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct SunLight 
{
	float power;
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform int sunLightNum;
uniform SunLight sunLights[32];

struct PointLight
{
	float power;
    vec3 position;
	
    float constant;
    float linear;
    float quadratic;
	
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform int pointLightNum;
uniform PointLight pointLights[32];

struct SpotLight
{
	float power;
    vec3  position;
    vec3  direction;
	
    float cutOff;
	float outerCutOff;
	
	float constant;
    float linear;
    float quadratic;
	
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform int spotLightNum;
uniform SpotLight spotLights[32];

vec3 CalcSunLight(SunLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 specularTexture)
{
	// ambient
    vec3 ambient = light.ambient;
    // diffuse shading
    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * light.power;
    // specular
	// phong
    vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.001), material.shininess);
	// blinn-phong
	//vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(viewDir, halfwayDir), 0.001), material.shininess);
    //vec3 specular = light.specular * spec * specularTexture * light.power;
	
	vec3 specular = light.specular * spec * specularTexture * light.power;
        
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 specularTexture)
{
	vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = light.power / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
    // combine results
    vec3 ambient  = light.ambient;
    vec3 diffuse  = light.diffuse * diff;
    vec3 specular = light.specular * spec * specularTexture;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 specularTexture)
{
	vec3 lightDir = normalize(light.position - fragPos);  

    // ambient
    vec3 ambient = light.ambient;
        
    // diffuse 
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * light.power;  
        
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularTexture * light.power;
	
	// spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;
        
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;   

    return (ambient + diffuse + specular);
}

void main()
{
	vec4 diffuseTexture = has_albedoMap ? texture(albedoMap, texCoordFrag * material.tiling) : vec4(1.0, 1.0, 1.0, 1.0);
	vec4 normalTexture = has_normalMap ? texture(normalMap, texCoordFrag * material.tiling) : vec4(0.5, 0.5, 1.0, 1.0);
	vec4 specularTexture = has_specularMap ? texture(specularMap, texCoordFrag * material.tiling) : vec4(0.0);
	vec4 emissiveTexture = has_emissiveMap ? texture(emissiveMap, texCoordFrag * material.tiling) : vec4(0.0);
	
	// Construct TBN matrix
	vec3 Tn = normalize(T);
	vec3 Nn = normalize(N);
	vec3 Bn = normalize(B);  // Recalculate bitangent from normal and tangent
	mat3 TBN = mat3(Tn, Bn, Nn);
	
	// Sample normal from texture (in tangent space)
    vec3 tangentNormal = normalTexture.rgb;
	
	// Flip green channel (Y)
	tangentNormal.g = 1.0 - tangentNormal.g;
	
	// Transform from [0,1] to [-1,1]
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0); 
	
	// Transform to object/local space normal
    vec3 localNormal = normalize(TBN * tangentNormal);
	
	// Optionally apply normalMatrix to get world-space normal
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
	vec3 norm = normalize(normalMatrix * localNormal);

    vec3 viewDir = normalize(viewPos - vertexPositionFrag);
	
	//if (diffuseTexture.a < alphaCutoff)
		//discard;
	
	vec3 result = vec3(0.0f);
	
	// Sun lighting
	if (sunLightNum > 0)
	{
		for(int i = 0; i < sunLightNum; i++)
		{
			result += CalcSunLight(sunLights[i], norm, vertexPositionFrag, viewDir, specularTexture.xyz);
		}
	}
	
    // Point lights
	if (pointLightNum > 0)
	{
		for(int i = 0; i < pointLightNum; i++)
		{
			result += CalcPointLight(pointLights[i], norm, vertexPositionFrag, viewDir, specularTexture.xyz); 
		}
	}
	
    // Spot light
	if (spotLightNum > 0)
	{
		for(int i = 0; i < spotLightNum; i++)
		{
			result += CalcSpotLight(spotLights[i], norm, vertexPositionFrag, viewDir, specularTexture.xyz);
		}
	}
	
	//result = pow(result, vec3(1.0/2.2));  // Convert to gamma space
	
	// Show lighting
	//fragColor = vec4(result, 1.0);
	//fragColor = vec4(result, 1.0) * vec4(1.0);  // Should be same as above
	
	// Show Combined
    //fragColor = vec4(result, 1.0) * diffuseTexture;
	fragColor = vec4(clamp(result, 0.0, 1.0), 1.0) * diffuseTexture + emissiveTexture;
	
	// Flat
	//fragColor = diffuseTexture;
	
	// Show normal
	//fragColor = vec4(norm * 0.5 + 0.5, 1.0);
	//fragColor = normalTexture;
	
	return;
}