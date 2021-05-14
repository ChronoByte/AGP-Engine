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
    Mode_Count
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

    //std::vector<Texture>  textures;
    //std::vector<Program>  programs;

	//Resources
	std::vector<Material>   materials;
	std::vector<Mesh>       meshes;
	std::vector<Model>      models;
	std::vector<Texture>  textures;
	std::vector<Program>  programs;

    // program indices
    u32 texturedGeometryProgramIdx;
	u32 texturedMeshProgramIdx;
    
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

    // Embedded geometry (in-editor simple meshes such as
    // a screen filling quad, a cube, a sphere...)
    GLuint embeddedVertices;
    GLuint embeddedElements;

    // Location of the texture uniform in the textured quad shader
    GLuint programUniformTexture;

    // VAO object to link our screen filling quad with our textured quad shader
    GLuint vao;

	//Models stuff
	u32 model;
	u32 plane;
	u32 texturedMeshProgram_uTexture;

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

};


void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

u32 LoadTexture2D(App* app, const char* filepath);

GLuint FindVAO(Mesh& mesh, u32 submeshIndex, const Program& program);

void renderQuad(App* app);
