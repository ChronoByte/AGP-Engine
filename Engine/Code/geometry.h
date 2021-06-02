#pragma once

//#include "engine.h"
struct App;
typedef unsigned int u32;
#include <glad/glad.h>
#include "platform.h"

//TEXTURED QUAD --------
struct VertexV3V2
{
	glm::vec3 pos;
	glm::vec2 uv;
};

const VertexV3V2 vertices[] =
{
	{ glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)}, // bottom-left vertex
	{ glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)}, // bottom-right vertex
	{ glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)}, // top-right vertex
	{ glm::vec3(-0.5, 0.5, 0.0), glm::vec2(0.0, 1.0)}, // top-left vertex
};

const u16 indices[] = {
	0,1,2,
	0,2,3
};


//MESH --------
struct VertexBufferAttribute
{
	u8 location;
	u8 componentCount;
	u8 offset;
};

struct VertexBufferLayout
{
	std::vector<VertexBufferAttribute> attributes;
	u8 stride;
};

struct VertexShaderAttribute
{
	u8 location;
	u8 componentCount;
};

struct VertexShaderLayout
{
	std::vector<VertexShaderAttribute> attributes;
};

struct Vao
{
	GLuint handle;
	GLuint programHandle;
};

struct Submesh
{
	VertexBufferLayout	vertexBufferLayout;
	std::vector<float>	vertices;
	std::vector<u32>	indices;
	u32					vertexOffset;
	u32					indexOffset;
	std::vector<Vao>	vaos;
};

struct Mesh
{
	std::vector<Submesh>	submeshes;
	GLuint					vertexBufferHandle;
	GLuint					indexBufferHandle;
};

struct Material
{
	std::string		name;
	glm::vec3			albedo;
	glm::vec3			emissive;
	f32				smoothness;
	u32				albedoTextureIdx;
	u32				emissiveTextureIdx;
	u32				specularTextureIdx;
	u32				normalTextureIdx;
	u32				bumpTextureIdx;
};

struct Model
{
	u32					meshIdx;
	std::vector<u32>	materialIdx;
};

struct Geometry
{
	std::vector<float> vertices;
	std::vector<u32> indices;

	Model planeModel;
	Model sphereModel;
	Model cubeModel;

	u32 LoadPlane(App* app);
	u32 LoadSphere(App* app);
	u32 LoadCube(App* app);

	void CalculateTangentSpace(std::vector<GLfloat>& vertices, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents);
};

