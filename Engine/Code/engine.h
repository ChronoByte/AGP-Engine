//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include "platform.h"
#include <glad/glad.h>
#include "geometry.h"
#include "assimp_model_loading.h"
#include "buffer_management.h"
#include "Entity.h"
#include "Light.h"
#include "Camera.h"
#include "FrameBufferObject.h"


#define BINDING(b) b

typedef glm::vec2  vec2;
typedef glm::vec3  vec3;
typedef glm::vec4  vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;

struct Image
{
    void* pixels;
    ivec2 size;
    i32   nchannels;
    i32   stride;
};

struct Texture
{
    GLuint      handle;
    std::string filepath;
};

struct Program
{
    GLuint             handle;
    std::string        filepath;
    std::string        programName;
    u64                lastWriteTimestamp; // What is this for?
	VertexShaderLayout vertexInputLayout;
};

struct GLInfo
{
	std::string version;
	std::string renderer;
	std::string vendor;
	std::string shadingLanguageVersion;

	std::vector<std::string> extensions;
};

enum Mode
{
    Mode_TexturedQuad,
	Mode_Model,
    Mode_Count,
    Mode_FBO
};

enum class RenderPipeline
{
    FORWARD,
    DEFERRED
};

enum FBO_TextureDisplay
{
    Final_Render,
    Normals,
    Albedo
};

struct App
{
    // Loop
    f32  deltaTime;
    bool isRunning;

    // Input
    Input input;

    // Graphics
    char gpuName[64];
    char openGlVersion[64];

    ivec2 displaySize;
    ivec2 lastFrameDisplaySize;

    //std::vector<Texture>  textures;
    //std::vector<Program>  programs;

	//Resources
	std::vector<Material>   materials;
	std::vector<Mesh>       meshes;
	std::vector<Model>      models;
	std::vector<Texture>  textures;
	std::vector<Program>  programs;

    // program indices
    u32 finalPassShaderIdx;
	u32 texturedMeshProgramIdx;
    u32 geometryPassShaderID; 
    u32 shadingPassShaderID; 
    u32 lightsShaderID;
	u32 reliefMapShaderID;
	u32 blurShaderID;

    // texture indices
    u32 diceTexIdx;
    u32 whiteTexIdx;
    u32 blackTexIdx;
    u32 normalTexIdx;
    u32 magentaTexIdx;

    // Mode
    Mode mode;

	// OpenGL info
	GLInfo info;
	bool show_opengl_info = true;
	bool show_effects = true;

    // Embedded geometry (in-editor simple meshes such as
    // a screen filling quad, a cube, a sphere...)
    GLuint embeddedVertices;
    GLuint embeddedElements;

    // Location of the texture uniform in the textured quad shader
    GLuint programGPassUniformTexture;
    GLuint programShadingPassUniformTexturePosition;
    GLuint programShadingPassUniformTextureNormals;
    GLuint programShadingPassUniformTextureAlbedo;
	GLuint programShadingPassUniformTextureDepth;
    GLuint programLightsUniformColor; 
    GLuint programLightsUniformWorldMatrix;

	//Relief Mapping uniform locations
	GLuint programReliefMapUniformTextureDiffuse;
	GLuint programReliefMapUniformTextureNormalMap;
	GLuint programReliefMapUniformTextureDepthMap;
	GLuint programReliefMapUniformModelMatrix;
	GLuint programReliefMapUniformViewMatrix;
	GLuint programReliefMapUniformProjectionMatrix;
	GLuint programReliefMapUniformLightPos;
	GLuint programReliefMapUniformViewPos;
	GLuint programReliefMapUniformHeightScale;

    // VAO object to link our screen filling quad with our textured quad shader
    GLuint vao;

	//Models stuff
	u32 model;
	u32 plane;
	u32 sphere;
	u32 cube;
	u32 texturedMeshProgram_uTexture;
	Geometry geo;

	// ------- Uniform blocks ---------- 

    // Local Params Block 
	Buffer ubuffer;
	GLint maxUniformBufferSize;
	GLint uniformBlockAlignment;
	u32 blockOffset;
	u32 blockSize;

    // Global Params Block 
    Buffer gpBuffer; 
    GLint maxGlobalParamsBufferSize; 
    GLint globalParamsAlignment; 
    u32 globalParamsOffset;
    u32 globalParamsSize; 

	// Transformations (temporal i guess)
	vec3 cameraPos;
	vec3 cameraRef;
	glm::mat4 worldMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 worldViewProjectionMatrix;

	//Entities
	std::vector<Entity> entities;

    //Lights
    std::vector<Light> lights; 

	//Camera
	Camera camera;

    //FBO
    GBuffer gFbo;
    ShadingBuffer shadingFbo; 
    PingPongBuffer blurFbo; 

    RenderTargetType displayedTexture = RenderTargetType::RENDER_TEXTURE;

	//Relief Mapping 
	float heigth_scale = 0.1f;
	bool clip_borders = false;
	int min_layers = 8;
	int max_layers = 32;

	u32 reliefDiffuseIdx;
	u32 reliefNormalIdx;
	u32 reliefHeightIdx;

	std::vector<float> ReliefAngles;
	std::vector<float> ReliefRotationRate;
	std::vector<glm::vec3> ReliefPositions;


    // Bloom
    bool using_bloom = true;
    int blurIterations = 10;
    float bright_threshold = 1;

    // Render Pipeline
    RenderPipeline render_pipeline = RenderPipeline::DEFERRED;
};


void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

void RenderUsingDeferredPipeline(App* app);
void RenderUsingForwardPipeline(App* app);

u32 LoadTexture2D(App* app, const char* filepath);

GLuint FindVAO(Mesh& mesh, u32 submeshIndex, const Program& program);

//Transformations
glm::mat4 TransformScale(const vec3& scaleFactors);
glm::mat4 TransformPositionScale(const vec3& pos, const vec3& scaleFactors);
glm::mat4 TransformRotation(const glm::mat4& matrix, float angle, vec3 axis);
vec3 GenerateRandomBrightColor();