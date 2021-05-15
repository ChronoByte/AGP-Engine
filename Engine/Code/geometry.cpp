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

u32 Geometry::LoadSphere(App* app)
{
	int H = 32;
	int V = 16;
	static const float pi = 3.1416f;
	

	//Vertices
	for (int h = 0; h < H; ++h) {
		for (int v = 0; v < V + 1; ++v) {

			float nh = float(h) / H;
			float nv = float(v) / V - 0.5f;
			float angleh = 2 * pi * nh;
			float anglev = -pi * nv;

			//Position
			float verX = sinf(angleh) * cosf(anglev);
			vertices.push_back(verX);

			float verY = -sinf(anglev);
			vertices.push_back(verY);

			float verZ = -sinf(cosf(angleh) * cosf(anglev));
			vertices.push_back(verZ);

			//Normal
			float normX = sinf(angleh) * cosf(anglev);
			vertices.push_back(normX);

			float normY = -sinf(anglev);
			vertices.push_back(normY);

			float normZ = cosf(angleh) * cosf(anglev);
			vertices.push_back(normZ);
			
			//TextCoord
			vertices.push_back(0.0);
			vertices.push_back(0.0);

		}
	}


	//Indices
	for (unsigned int h = 0; h < H; ++h) {
		for (unsigned int v = 0; v < V; ++v) {
			indices.push_back((h + 0) * (V + 1) + v);
			indices.push_back(((h + 1) % H) * (V + 1) + v);
			indices.push_back(((h + 1) % H) * (V + 1) + v + 1);
			indices.push_back((h + 0) * (V + 1) + v);
			indices.push_back(((h + 1) % H) * (V + 1) + v + 1);
			indices.push_back((h + 0) * (V + 1) + v + 1);
		}
	}



	//Vertex input attributes (position, normal, textcoords)
	VertexBufferLayout vertexBufferLayout;
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, 6 * sizeof(float) });
	vertexBufferLayout.stride = 8 * sizeof(float);

	Submesh submesh = {};
	submesh.vertexBufferLayout = vertexBufferLayout;
	submesh.vertices.swap(vertices);
	submesh.indices.swap(indices);

	
	Mesh sphereMesh;
	sphereMesh.submeshes.push_back(submesh);


	//Buffers
	float verticesOffset = 0.0f;
	float indicesOffset = 0.0f;

	glGenBuffers(1, &sphereMesh.vertexBufferHandle);
	glGenBuffers(1, &sphereMesh.indexBufferHandle);

	for (int i = 0; i < sphereMesh.submeshes.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, sphereMesh.vertexBufferHandle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphereMesh.submeshes[i].vertices.size(), &sphereMesh.submeshes[i].vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereMesh.indexBufferHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * sphereMesh.submeshes[i].indices.size(), &sphereMesh.submeshes[i].indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		submesh.vertexOffset = verticesOffset;
		submesh.indexOffset = indicesOffset;

		verticesOffset += sizeof(float) * sphereMesh.submeshes[i].vertices.size();
		indicesOffset += sizeof(u32) * sphereMesh.submeshes[i].indices.size();
	}

	

	//Mesh
	sphereModel.meshIdx = app->meshes.size();
	app->meshes.push_back(sphereMesh);

	//Material
	Material sphereMat;
	sphereMat.albedo = vec3(1.0f, 1.0f, 1.0f);
	sphereMat.albedoTextureIdx = app->whiteTexIdx;
	u32 materialIdx = app->materials.size();
	app->materials.push_back(sphereMat);
	sphereModel.materialIdx.push_back(materialIdx);

	//Model
	u32 modelIdx = app->models.size();
	app->models.push_back(sphereModel);

	return modelIdx;
}
