#version 330 core

in vec3 vertexPositionFrag;
in vec3 vertexColorFrag;
in vec2 texCoordFrag;
in vec3 vertexNormalFrag;
//in vec3 vertexTangentFrag;
//in vec3 vertexBitangentFrag;
in vec4 lightSpaceMatrixFrag;

uniform mat4 normalMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;
//uniform mat4 lightSpaceMatrix;

in vec3 T;
in vec3 B;
in vec3 N;

out vec4 fragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughnessMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;
uniform sampler2D shadowMap;

uniform bool has_albedoMap;
uniform bool has_normalMap;
uniform bool has_metallicRoughnessMap;
uniform bool has_aoMap;
uniform bool has_emissiveMap;
uniform bool has_shadowMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform bool has_irradianceMap;
uniform bool has_prefilterMap;
uniform bool has_brdfLUT;

//uniform float alphaCutoff = 0.5;

// Constants
const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// Function: Normal Mapping
vec3 GetNormalFromMap(mat3 TBN, vec4 normalTexture)
{
    vec3 tangentNormal = normalTexture.xyz * 2.0 - 1.0;
    vec3 worldNormal = normalize(TBN * tangentNormal);
    return worldNormal;
}

// ----------------------------------------------------------------------------
// Function: Fresnel Schlick
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// ----------------------------------------------------------------------------
// Function: Distribution GGX
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// ----------------------------------------------------------------------------
// Function: Geometry Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

// ----------------------------------------------------------------------------
// Function: Geometry Smith
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------

struct Material {
	vec2 tiling;
    vec3 diffuse;
	float metallic;
    float roughness;
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

vec3 CalcSunLight(SunLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao, float shadow)
{
	vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);
    vec3 radiance = light.diffuse * light.power;

	// Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);

    //vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F0 = mix(vec3(0.04), albedo * material.diffuse, material.metallic);
    vec3 F  = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 nominator   = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, L), 0.0);

	// Outgoing light
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
	
	// Apply shadow
	Lo *= (1.0 - shadow); // Shadow attenuation
	
	// IBL contribution
    vec3 F_ibl = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);

    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuseIBL = irradiance * albedo;

    vec3 R = reflect(-viewDir, normal);
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * 4.0).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specularIBL = prefilteredColor * (F_ibl * brdf.x + brdf.y);

    // Add ambient
    vec3 ambient = (diffuseIBL * kD + specularIBL) * ao;
	
	// Tweak how much ambient is affected by shadow
	float ambientShadowFactor = 0.3 + 0.7 * (0.8 - shadow); // Range: 1.0 (lit), 0.3 (fully shadowed)
	ambient *= ambientShadowFactor;
	
	return ambient + Lo;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao, float shadow)
{
	vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);
    float distance = length(light.position - fragPos);
    float attenuation = light.power / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 radiance = light.diffuse * attenuation;

	// Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F  = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 nominator   = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, L), 0.0);

	// Outgoing light
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
	
	// Apply shadow
	//Lo *= (1.0 - shadow); // Shadow attenuation
	
	// IBL
    vec3 F_ibl = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuseIBL = irradiance * albedo;

    vec3 R = reflect(-viewDir, normal);
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * 4.0).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specularIBL = prefilteredColor * (F_ibl * brdf.x + brdf.y);

    vec3 ambient = (diffuseIBL * kD + specularIBL) * ao;

    return ambient + Lo;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao, float shadow)
{
	vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);
    float distance = length(light.position - fragPos);

    // spotlight angle
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float attenuation = light.power / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 radiance = light.diffuse * attenuation * intensity;

	// Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F  = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 nominator   = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, L), 0.0);

	// Outgoing light
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
	
	// Apply shadow
	//Lo *= (1.0 - shadow); // Shadow attenuation
	
	// IBL
    vec3 F_ibl = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuseIBL = irradiance * albedo;

    vec3 R = reflect(-viewDir, normal);
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * 4.0).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specularIBL = prefilteredColor * (F_ibl * brdf.x + brdf.y);

    vec3 ambient = (diffuseIBL * kD + specularIBL) * ao;

    return ambient + Lo;
}

float ShadowCalculation(vec4 lightSpaceMatrixFrag)
{
	// perform perspective divide
    vec3 projCoords = lightSpaceMatrixFrag.xyz / lightSpaceMatrixFrag.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	// Add bias to prevent shadow acne
    float bias = 0.005;
	// Check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
	vec4 diffuseTexture = has_albedoMap ? texture(albedoMap, texCoordFrag * material.tiling) : vec4(1.0, 1.0, 1.0, 1.0);
	vec4 normalTexture = has_normalMap ? texture(normalMap, texCoordFrag * material.tiling) : vec4(0.5, 0.5, 1.0, 1.0);
	vec4 metallicRoughnessTexture = has_metallicRoughnessMap ? texture(metallicRoughnessMap, texCoordFrag * material.tiling) : vec4(0.5, 0.5, 0.5, 1.0);
	vec4 aoTexture = has_aoMap ? texture(aoMap, texCoordFrag * material.tiling) : vec4(1.0, 1.0, 1.0, 1.0);
	vec4 emissiveColor = has_emissiveMap ? texture(emissiveMap, texCoordFrag * material.tiling) : vec4(0.0, 0.0, 0.0, 1.0);
	
	// Construct TBN matrix
	vec3 Tn = normalize(T);
	vec3 Nn = normalize(N);
	vec3 Bn = normalize(B);  // Recalculate bitangent from normal and tangent
	mat3 TBN = mat3(Tn, Bn, Nn);
	
	/*
	// Sample normal from texture (in tangent space)
    vec3 tangentNormal = normalTexture.rgb;
	
	// Flip green channel (Y)
	tangentNormal.g = 1.0 - tangentNormal.g;
	
	// Transform from [0,1] to [-1,1]
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0); 
	
	// Transform to object/local space normal
    vec3 localNormal = normalize(TBN * tangentNormal);
	
	// Optionally apply normalMatrix to get world-space normal
	mat3 normalMat = mat3(transpose(inverse(modelMatrix)));
	vec3 norm = normalize(normalMat * localNormal);

    vec3 viewDir = normalize(viewPos - vertexPositionFrag);
	*/
	
	//////
	// Material inputs
    vec3 albedo = pow(diffuseTexture.rgb, vec3(2.2)); // sRGB to linear
    vec3 worldNormal = GetNormalFromMap(TBN, normalTexture);
    vec3 viewVector = normalize(viewPos - vertexPositionFrag);

    vec3 mrSample = metallicRoughnessTexture.rgb;
    float metallic = mrSample.b;
    float roughness = mrSample.g;
    float ao = aoTexture.r;
	/////
	
	//if (diffuseTexture.a < alphaCutoff)
		//discard;
	
	// Shadow
	float shadow = ShadowCalculation(lightSpaceMatrixFrag); 
	
	// Lighting
	vec3 result = vec3(0.0f);
	
	// Sun lighting
	if (sunLightNum > 0)
	{
		for(int i = 0; i < sunLightNum; i++)
		{
			result += CalcSunLight(sunLights[i], worldNormal, vertexPositionFrag, viewVector, albedo * material.diffuse, metallic * material.metallic, roughness * material.roughness, ao, shadow);
		}
	}

    // Point lights
	if (pointLightNum > 0)
	{
		for(int i = 0; i < pointLightNum; i++)
		{
			result += CalcPointLight(pointLights[i], worldNormal, vertexPositionFrag, viewVector, albedo * material.diffuse, metallic * material.metallic, roughness * material.roughness, ao, shadow);
		}
	}
	
    // Spot light
	if (spotLightNum > 0)
	{
		for(int i = 0; i < spotLightNum; i++)
		{
			result += CalcSpotLight(spotLights[i], worldNormal, vertexPositionFrag, viewVector, albedo * material.diffuse, metallic * material.metallic, roughness * material.roughness, ao, shadow);
		}
	}
	
	// HDR tonemapping (optional)
    result = result / (result + vec3(1.0));
	
	// Gamma correction for output (optional)
	result = pow(result, vec3(1.0 / 2.2));

	fragColor = vec4(result, 1.0) + emissiveColor;
}
