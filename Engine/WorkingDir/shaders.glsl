///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TEXTURED GEOMETRY SHADER
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef TEXTURED_GEOMETRY

#if defined(VERTEX) ///////////////////////////////////////////////////

// TODO: Write your vertex shader here

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

// TODO: Write your fragment shader here

in vec2 vTexCoord;

uniform sampler2D uTexture;

layout(location = 0) out vec4 oColor;

void main()
{
	oColor = texture(uTexture, vTexCoord);
}

#endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MESH SHADER		------ DEPRECATED ---------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef SHOW_TEXTURED_MESH

#if defined(VERTEX) ///////////////////////////////////////////////////

struct Light {
	unsigned int type; 
	vec3 position;
	vec3 color; 
	vec3 direction;
	unsigned int intensity;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[150];
};

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec3 vPosition; // in worldspace
out vec3 vNormal; // in worldspace
out vec3 viewDir;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = vec3(uWorldMatrix * vec4(aNormal, 0.0));
	viewDir = uCameraPosition - vPosition;
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);

	//float clippingScale = 5.0;

	//gl_Position = vec4(aPosition, clippingScale);

	//gl_Position.z = -gl_Position.z;
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition; // in worldspace
in vec3 vNormal; // in worldspace
in vec3 viewDir; 

uniform sampler2D uTexture;

struct Light {
	unsigned int type; 
	vec3 position;
	vec3 color; 
	vec3 direction;
	unsigned int intensity;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[150];
};


layout(location = 0) out vec4 FragColor;

vec3 CalculateLighting(Light light, vec3 normal, vec3 viewDir, vec3 frag_pos);

void main()
{
	vec3 lightColorInfluence = vec3(0.0);
	
	for(int i = 0; i < uLightCount; ++i)
		lightColorInfluence += CalculateLighting(uLights[i], vNormal, viewDir, vPosition);

	FragColor = texture(uTexture, vTexCoord) * vec4(lightColorInfluence, 1.0);
}

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 view_dir)
{
	// Diffuse 
	vec3 lightDirection = normalize(-light.direction);
	float diff = max(dot(lightDirection, normal), 0.0);
	vec3 diffuse = light.color * diff;

	// Specular
	// vec3 halfwayDir = normalize(lightDirection + view_dir);
	// float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);
	// vec3 specular = light.color * spec;
	vec3 specular = vec3(0.0);

	vec3 result = (diffuse + specular);
	return result;
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 view_dir, vec3 frag_pos)
{
	// Diffuse 
	vec3 lightDirection = normalize(light.position - frag_pos);
	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * diff;

	// Specular
	// vec3 reflectDir = reflect(-lightDir, normal);  
    // float spec = pow(max(dot(view_dir, reflectDir), 0.0), objectMaterial.smoothness) * objectMaterial.metalness;
	// vec3 specular = light.color * spec;
	vec3 specular = vec3(0.0);

	// Attenuation
 	float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (1.0 + 0.5 * distance + 1.0 * (distance * distance));    

	vec3 result = (diffuse + specular);
	return result;
}


vec3 CalculateLighting(Light light, vec3 normal, vec3 view_dir, vec3 frag_pos)
{

	vec3 result = vec3(0.0);

	switch(light.type)
	{
		case 0: 
			result = CalculatePointLight(light, normal, view_dir, frag_pos);
			break;
		case 1:
			result = CalculateDirectionalLight(light, normal, view_dir);
			break;

		default: break;
	}

	return result;
}


#endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// GEOMETRY PASS SHADER
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef GEOMETRY_PASS_SHADER

#if defined(VERTEX) ///////////////////////////////////////////////////

struct Light {
	unsigned int type; 
	vec3 position;
	vec3 color; 
	vec3 direction;
	unsigned int intensity;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[150];
};

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec3 vPosition; // in worldspace
out vec3 vNormal; // in worldspace


void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = vec3(uWorldMatrix * vec4(aNormal, 0.0));
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);

	//float clippingScale = 5.0;

	//gl_Position = vec4(aPosition, clippingScale);

	//gl_Position.z = -gl_Position.z;
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition; // in worldspace
in vec3 vNormal; // in worldspace

uniform sampler2D uTexture;

layout(location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec3 gAlbedoSpec;
layout(location = 4) out vec4 gDepth;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	gPosition = vPosition; 
	gNormal = normalize(vNormal);
	gAlbedoSpec.rgb = texture(uTexture, vTexCoord).rgb;
	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
	gDepth = vec4(vec3(depth), 1.0);
	FragColor = texture(uTexture, vTexCoord);
}

#endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SHADING PASS SHADER
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef SHADING_PASS_SHADER

#if defined(VERTEX) ///////////////////////////////////////////////////

struct Light {
	unsigned int type; 
	vec3 position;
	vec3 color; 
	vec3 direction;
	unsigned int intensity;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[150];
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////


struct Light {
	unsigned int type; 
	vec3 position;
	vec3 color; 
	vec3 direction;
	unsigned int intensity;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[150];
};

in vec2 vTexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

layout(location = 0) out vec4 FragColor;

vec3 CalculateLighting(Light light, vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 pixelColor);

void main()
{
	// retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, vTexCoord).rgb;
    vec3 Normal = texture(gNormal, vTexCoord).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, vTexCoord).rgb;
    float Specular = 0;

	vec3 viewDir  = normalize(uCameraPosition - FragPos);

    vec3 lighting  = vec3(0.0);
    for(int i = 0; i < uLightCount; ++i)
		lighting += CalculateLighting(uLights[i], Normal, viewDir, FragPos, Diffuse);

    FragColor = vec4(lighting, 1.0);
}

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 view_dir, vec3 pixelColor)
{
	float intensity = float(light.intensity);
	intensity *= 0.01;
	// Diffuse 
	vec3 lightDirection = normalize(-light.direction);
	float diff = max(dot(lightDirection, normal), 0.0);
	vec3 diffuse = light.color * diff * pixelColor *  intensity;

	// Specular
	vec3 halfwayDir = normalize(lightDirection + view_dir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
	vec3 specular = light.color * spec * intensity;

	vec3 result = (diffuse + specular);
	return result;
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 pixelColor)
{
	float intensity = float(light.intensity);
	intensity *= 0.01;

	// Diffuse 
	vec3 lightDirection = normalize(light.position - frag_pos);
	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * diff * pixelColor * intensity;

	// Specular
    vec3 halfwayDir = normalize(lightDirection + view_dir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
	vec3 specular = light.color * spec * intensity;

	// Attenuation
 	float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (1.0 + 0.3 * distance + 0.5 * (distance * distance));    

	diffuse *= attenuation;
    specular *= attenuation;

	vec3 result = (diffuse + specular);
	return result;
}


vec3 CalculateLighting(Light light, vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 pixelColor)
{

	vec3 result = vec3(0.0);

	switch(light.type)
	{
		case 0: 
			result = CalculatePointLight(light, normal, view_dir, frag_pos, pixelColor);
			break;
		case 1:
			result = CalculateDirectionalLight(light, normal, view_dir, pixelColor);
			break;

		default: break;
	}

	return result;
}

#endif
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// LIGHTS SHADER
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef LIGHTS_SHADER

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uWorldViewProjectionMatrix;

void main()
{
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

uniform vec3 lightColor; 

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(lightColor, 1.0);
}

#endif
#endif



// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// RELIEF MAPPING SHADER
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef RELIEF_MAPPING_SHADER

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;

	vec3 T = normalize(mat3(model) * aTangent);
	vec3 B = normalize(mat3(model) * aBitangent);
	vec3 N = normalize(mat3(model) * aNormal);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////



in VS_OUT{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float heightScale;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 gPosition;
layout(location = 2) out vec3 gNormal;
layout(location = 3) out vec3 gAlbedoSpec;
//layout(location = 4) out vec4 gDepth;


vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
	// offset texture coordinates with Parallax Mapping
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec2 texCoords = fs_in.TexCoords;

	texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);
	if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
		discard;

	// obtain normal from normal map
	vec3 normal = texture(normalMap, texCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	

	// get diffuse color
	vec3 color = texture(diffuseMap, texCoords).rgb;
	// ambient
	vec3 ambient = 0.1 * color;
	// diffuse
	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;
	// specular    
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.2) * spec;

	gPosition = fs_in.FragPos;
	gNormal = normal;
	gAlbedoSpec.rgb = texture(diffuseMap, texCoords).rgb;
	FragColor = vec4(ambient + diffuse + specular, 1.0);

	//float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
	//gDepth = vec4(vec3(depth), 1.0);
	//FragColor = texture(uTexture, vTexCoord);

	//FragColor = vec4(ambient + diffuse + specular, 1.0);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	// number of depth layers
	const float minLayers = 8;
	const float maxLayers = 32;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy / viewDir.z * heightScale;
	vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2  currentTexCoords = texCoords;
	float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(depthMap, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

#endif
#endif

// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.
