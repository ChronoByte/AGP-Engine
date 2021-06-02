#include "geometry.h"
#include "engine.h"

u32 Geometry::LoadPlane(App* app)
{

	//Geometry
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


	//Vertex input attributes (position, normal, textcoords)
	VertexBufferLayout vertexBufferLayout = {};
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, 6 * sizeof(float) });
	vertexBufferLayout.stride = 8 * sizeof(float);

	//Submesh
	Submesh submesh = {};
	submesh.vertexBufferLayout = vertexBufferLayout;
	submesh.vertices.swap(vertices);
	submesh.indices.swap(indices);

	//Mesh
	Mesh planeMesh;
	planeMesh.submeshes.push_back(submesh);

	//Buffers
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

	
	//Mesh
	planeModel.meshIdx = app->meshes.size();
	app->meshes.push_back(planeMesh);


	//Material
	Material planeMat;
	planeMat.albedo = vec3(1.0f, 1.0f, 1.0f);
	planeMat.albedoTextureIdx = app->whiteTexIdx;
	u32 materialIdx = app->materials.size();
	app->materials.push_back(planeMat);
	planeModel.materialIdx.push_back(materialIdx);

	//Model
	u32 modelIdx = app->models.size();
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

			float verZ = cosf(angleh) * cosf(anglev);
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

u32 Geometry::LoadCube(App* app)
{
	//Geometry
	
	 vertices = {
		//front
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		
		//back
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		
		//left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		
		//right
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		

		//below
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		
		//above
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		


	};
	std::vector<GLfloat> vertices_vector(vertices);

	indices = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};

	//Vertex input attributes (position, normal, textcoords)
	VertexBufferLayout vertexBufferLayout = {};
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, 6 * sizeof(float) });
	//vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 3, 3, 8 * sizeof(float) });
	//vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 4, 3, 11 * sizeof(float) });
	vertexBufferLayout.stride = 8 * sizeof(float);

	//Submesh
	Submesh submesh = {};
	submesh.vertexBufferLayout = vertexBufferLayout;
	submesh.vertices.swap(vertices);
	submesh.indices.swap(indices);

	//Mesh
	Mesh cubeMesh;
	cubeMesh.submeshes.push_back(submesh);


	//Buffers
	float verticesOffset = 0.0f;
	float indicesOffset = 0.0f;

	glGenBuffers(1, &cubeMesh.vertexBufferHandle);
	glGenBuffers(1, &cubeMesh.indexBufferHandle);

	for (int i = 0; i < cubeMesh.submeshes.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, cubeMesh.vertexBufferHandle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cubeMesh.submeshes[i].vertices.size(), &cubeMesh.submeshes[i].vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeMesh.indexBufferHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * cubeMesh.submeshes[i].indices.size(), &cubeMesh.submeshes[i].indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		submesh.vertexOffset = verticesOffset;
		submesh.indexOffset = indicesOffset;

		verticesOffset += sizeof(float) * cubeMesh.submeshes[i].vertices.size();
		indicesOffset += sizeof(u32) * cubeMesh.submeshes[i].indices.size();
	}


	GLuint tangentBuffer, bitangentBuffer;
	std::vector<glm::vec3> tangents, bitangents;

	CalculateTangentSpace(vertices_vector, tangents, bitangents);

	glGenBuffers(1, &tangentBuffer);
	glGenBuffers(1, &bitangentBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof glm::vec3, &bitangents[0], GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);











	//Mesh
	cubeModel.meshIdx = app->meshes.size();
	app->meshes.push_back(cubeMesh);


	////Material
	//Material cubeMat;
	//cubeMat.albedo = vec3(1.0f, 1.0f, 1.0f);
	//cubeMat.albedoTextureIdx = app->whiteTexIdx;
	//u32 materialIdx = app->materials.size();
	//app->materials.push_back(cubeMat);
	//cubeModel.materialIdx.push_back(materialIdx);

	//Model
	u32 modelIdx = app->models.size();
	app->models.push_back(cubeModel);

	return modelIdx;
}

void Geometry::CalculateTangentSpace(std::vector<GLfloat>& vertices, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents)
{
	int counting = 0;
	for (int i = 0; i < vertices.size(); i += 3 * 8)
	{
		//shortcuts of vertices
		glm::vec3 v0 = glm::vec3(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
		glm::vec3 v1 = glm::vec3(vertices[i + 8], vertices[i + 8 + 1], vertices[i + 8 + 2]);
		glm::vec3 v2 = glm::vec3(vertices[i + 2 * 8], vertices[i + 2 * 8 + 1], vertices[i + 2 * 8 + 2]);

		//shortcuts for UVs
		glm::vec2 uv0 = glm::vec2(vertices[i + 6], vertices[i + 7]);
		glm::vec2 uv1 = glm::vec2(vertices[i + 8 + 6], vertices[i + 8 + 7]);
		glm::vec2 uv2 = glm::vec2(vertices[i + 2 * 8 + 6], vertices[i + 2 * 8 + 7]);

		//edges of the triangle: position delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		//uv delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		//compute tangent and bitangent:
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		counting++;
	}
}
