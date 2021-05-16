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
// MESH SHADER
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef SHOW_TEXTURED_MESH

#if defined(VERTEX) ///////////////////////////////////////////////////

struct Light {
	unsigned int type; 
	vec3 position;
	vec3 color; 
	vec3 direction;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[16];
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
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[16];
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
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[16];
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

void main()
{
	gPosition = vPosition; 
	gNormal = normalize(vNormal);
	gAlbedoSpec.rgb = texture(uTexture, vTexCoord).rgb;

	FragColor = texture(uTexture, vTexCoord);
}

#endif
#endif

// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.

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
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[16];
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
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount; 
	Light uLights[16];
};

in vec2 vTexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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
	// Diffuse 
	vec3 lightDirection = normalize(-light.direction);
	float diff = max(dot(lightDirection, normal), 0.0);
	vec3 diffuse = light.color * diff * pixelColor;

	// Specular
	vec3 halfwayDir = normalize(lightDirection + view_dir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
	vec3 specular = light.color * spec;

	vec3 result = (diffuse + specular);
	return result;
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 pixelColor)
{
	// Diffuse 
	vec3 lightDirection = normalize(light.position - frag_pos);
	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * diff * pixelColor;

	// Specular
    vec3 halfwayDir = normalize(lightDirection + view_dir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
	vec3 specular = light.color * spec;

	// Attenuation
 	float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (1.0 + 0.5 * distance + 1.0 * (distance * distance));    

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

// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.
