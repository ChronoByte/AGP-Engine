#include "geometry.h"
#include "engine.h"

u32 Geometry::LoadPlane(App* app)
{
	vertices = {
	-1.0, -1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, // bottom-left
	1.0, -1.0, 0.0, 0.0, 0.0, -1.0, 1.0, 0.0, // bottom-right
	1.0, 1.0, 0.0, 0.0, 0.0, -1.0, 1.0, 1.0, // top-right
	-1.0, 1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0 // top-left
	};

	indices = {
		0, 1, 2,
		0, 2, 3
	};

	Mesh planeMesh = {};

	//create the vertex format
	VertexBufferLayout vertexBufferLayout = {};
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, 6 * sizeof(float) });
	vertexBufferLayout.stride = 8 * sizeof(float);

	//add the submesh into the mesh
	Submesh submesh = {};
	submesh.vertexBufferLayout = vertexBufferLayout;
	submesh.vertices.swap(vertices);
	submesh.indices.swap(indices);

	planeMesh.submeshes.push_back(submesh);

	////Geometry
	float verticesOffset = 0.0f;
	float indicesOffset = 0.0f;

	glGenBuffers(1, &planeMesh.vertexBufferHandle);
	glGenBuffers(1, &planeMesh.indexBufferHandle);

	for (int i = 0; i < planeMesh.submeshes.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, planeMesh.vertexBufferHandle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planeMesh.submeshes[i].vertices.size(), &planeMesh.submeshes[i].vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeMesh.indexBufferHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * planeMesh.submeshes[i].indices.size(), &planeMesh.submeshes[i].indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		submesh.vertexOffset = verticesOffset;
		submesh.indexOffset = indicesOffset;

		verticesOffset += sizeof(float) * planeMesh.submeshes[i].vertices.size();
		indicesOffset += sizeof(u32) * planeMesh.submeshes[i].indices.size();
	}

	

	planeModel.meshIdx = app->meshes.size();
	app->meshes.push_back(planeMesh);

	u32 modelIdx = app->models.size();

	Material planeMat;
	planeMat.albedo = vec3(1.0f, 1.0f, 1.0f);
	planeMat.albedoTextureIdx = app->whiteTexIdx;

	u32 materialIdx = app->materials.size();
	app->materials.push_back(planeMat);
	planeModel.materialIdx.push_back(materialIdx);

	app->models.push_back(planeModel);

	return modelIdx;
}
