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
	{
		lightColorInfluence += CalculateLighting(uLights[i], vNormal, viewDir, vPosition);
	}

	FragColor = texture(uTexture, vTexCoord) * vec4(lightColorInfluence, 1.0);
}

vec3 CalculateLighting(Light light, vec3 normal, vec3 view_dir, vec3 frag_pos)
{

	// ================= DIRECTIONAL =================

	// Diffuse 
	vec3 lightDirection = normalize(-light.direction);
	float diff = max(dot(lightDirection, normal), 0.0);
	vec3 diffuse = light.color * diff;

	// Specular
	// vec3 halfwayDir = normalize(lightDirection + view_dir);
	// float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);
	// vec3 specular = light.color * spec;
	vec3 specular = vec3(0.0);

	vec3 result = vec3(0.0);
	result = (diffuse + specular);
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

struct Light {
	unsigned int type; 
	vec3 position;
	vec3 color; 
	vec3 direction;
};

layout(binding = 0, std140) uniform GlobalParams
{
	unsigned int uLightCount; 
	Light uLights[16];
};


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
