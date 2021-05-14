//
// engine.cpp : Put all your graphics stuff in this file. This is kind of the graphics module.
// In here, you should type all your OpenGL commands, and you can also type code to handle
// input platform events (e.g to move the camera or react to certain shortcuts), writing some
// graphics related GUI options, and so on.
//

#include "engine.h"
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>



GLuint CreateProgramFromSource(String programSource, const char* shaderName)
{
    GLchar  infoLogBuffer[1024] = {};
    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
    GLsizei infoLogSize;
    GLint   success;

    char versionString[] = "#version 430\n";
    char shaderNameDefine[128];
    sprintf(shaderNameDefine, "#define %s\n", shaderName);
    char vertexShaderDefine[] = "#define VERTEX\n";
    char fragmentShaderDefine[] = "#define FRAGMENT\n";

    const GLchar* vertexShaderSource[] = {
        versionString,
        shaderNameDefine,
        vertexShaderDefine,
        programSource.str
    };
    const GLint vertexShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(vertexShaderDefine),
        (GLint) programSource.len
    };
    const GLchar* fragmentShaderSource[] = {
        versionString,
        shaderNameDefine,
        fragmentShaderDefine,
        programSource.str
    };
    const GLint fragmentShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(fragmentShaderDefine),
        (GLint) programSource.len
    };

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vshader);
    glAttachShader(programHandle, fshader);
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    glUseProgram(0);

    glDetachShader(programHandle, vshader);
    glDetachShader(programHandle, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programHandle;
}

u32 LoadProgram(App* app, const char* filepath, const char* programName)
{
    String programSource = ReadTextFile(filepath);

    Program program = {};
    program.handle = CreateProgramFromSource(programSource, programName);
    program.filepath = filepath;
    program.programName = programName;
    program.lastWriteTimestamp = GetFileLastWriteTimestamp(filepath);
    app->programs.push_back(program);

    return app->programs.size() - 1;
}

Image LoadImage(const char* filename)
{
    Image img = {};
    stbi_set_flip_vertically_on_load(true);
    img.pixels = stbi_load(filename, &img.size.x, &img.size.y, &img.nchannels, 0);
    if (img.pixels)
    {
        img.stride = img.size.x * img.nchannels;
    }
    else
    {
        ELOG("Could not open file %s", filename);
    }
    return img;
}

void FreeImage(Image image)
{
    stbi_image_free(image.pixels);
}

GLuint CreateTexture2DFromImage(Image image)
{
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat     = GL_RGB;
    GLenum dataType       = GL_UNSIGNED_BYTE;

    switch (image.nchannels)
    {
        case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
        case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
        default: ELOG("LoadTexture2D() - Unsupported number of channels");
    }

    GLuint texHandle;
    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size.x, image.size.y, 0, dataFormat, dataType, image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texHandle;
}

u32 LoadTexture2D(App* app, const char* filepath)
{
    for (u32 texIdx = 0; texIdx < app->textures.size(); ++texIdx)
        if (app->textures[texIdx].filepath == filepath)
            return texIdx;

    Image image = LoadImage(filepath);

    if (image.pixels)
    {
        Texture tex = {};
        tex.handle = CreateTexture2DFromImage(image);
        tex.filepath = filepath;

        u32 texIdx = app->textures.size();
        app->textures.push_back(tex);

        FreeImage(image);
        return texIdx;
    }
    else
    {
        return UINT32_MAX;
    }
}

void Init(App* app)
{
    // TODO: Initialize your resources here!
    // - vertex buffers
    // - element/index buffers
    // - vaos
    // - programs (and retrieve uniform indices)
    // - textures


	// OpenGL information ------------------
	app->info.version = (const char*)glGetString(GL_VERSION);
	app->info.renderer = (const char*)glGetString(GL_RENDERER);
	app->info.vendor = (const char*)glGetString(GL_VENDOR);
	app->info.shadingLanguageVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint num_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
	for (int i = 0; i < num_extensions; ++i)
	{
		app->info.extensions.push_back((const char*)glGetStringi(GL_EXTENSIONS, GLuint(i)));
	}



	//--------------------- TEXTURED QUAD ---------------------- //
	
	// Vertex Buffer
	glGenBuffers(1, &app->embeddedVertices);
	glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Index Buffer
	glGenBuffers(1, &app->embeddedElements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// VAO
	glGenVertexArrays(1, &app->vao);
	glBindVertexArray(app->vao);
	glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)12);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
	glBindVertexArray(0);

	// Program 
	app->texturedGeometryProgramIdx = LoadProgram(app, "shaders.glsl", "TEXTURED_GEOMETRY");
	Program& texturedGeometryProgram = app->programs[app->texturedGeometryProgramIdx];
	app->programUniformTexture = glGetUniformLocation(texturedGeometryProgram.handle, "uTexture");

	// Textures 
	app->diceTexIdx = LoadTexture2D(app, "dice.png");
	app->whiteTexIdx = LoadTexture2D(app, "color_white.png");
	app->blackTexIdx = LoadTexture2D(app, "color_black.png");
	app->normalTexIdx = LoadTexture2D(app, "color_normal.png");
	app->magentaTexIdx = LoadTexture2D(app, "color_magenta.png");



	//--------------------- PATRICK ---------------------- //

	

	// Camera -----------
	app->camera.position = vec3(0.0f, 4.0f, 15.0f);
	app->camera.target = vec3(0.0f);
	
	app->camera.aspect_ratio = (float)app->displaySize.x / (float)app->displaySize.y;
	app->camera.projectionMatrix = glm::perspective(glm::radians(app->camera.vertical_fov), app->camera.aspect_ratio, app->camera.nearPlane, app->camera.farPlane);
	app->camera.viewMatrix = glm::lookAt(app->camera.position, app->camera.target, glm::vec3(0, 1, 0));
	
	//app->worldMatrix = glm::mat4(1.0);
	//app->worldMatrix = glm::translate(app->worldMatrix, vec3(0.0, 0.0, 0.0));


	// Program ----------
	app->texturedMeshProgramIdx = LoadProgram(app, "shaders.glsl", "SHOW_TEXTURED_MESH");
	Program& texturedMeshProgram = app->programs[app->texturedMeshProgramIdx];

	// Attributes Program ----------
	int attributeCount;
	glGetProgramiv(texturedMeshProgram.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);

	GLchar attributeName[64];
	GLsizei attributeNameLength;
	GLint attributeSize;
	GLenum attributeType;

	for (int i = 0; i < attributeCount; ++i)
	{
		glGetActiveAttrib(texturedMeshProgram.handle, i, 64, &attributeNameLength, &attributeSize, &attributeType, attributeName);

		GLint attributeLocation = glGetAttribLocation(texturedMeshProgram.handle, attributeName);
		texturedMeshProgram.vertexInputLayout.attributes.push_back({(u8)attributeLocation,(u8)attributeSize }); 
	}

	// Uniform blocks ---------

	// Local Params
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &app->maxUniformBufferSize);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &app->uniformBlockAlignment);
	app->ubuffer = CreateConstantBuffer(app->maxUniformBufferSize);
	
	// Global Params
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &app->maxGlobalParamsBufferSize);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &app->globalParamsAlignment);
	app->gpBuffer = CreateConstantBuffer(app->maxGlobalParamsBufferSize);

	// Model ----------
	app->model = LoadModel(app, "Patrick/Patrick.obj");
	app->plane = app->geo.LoadPlane(app);
	app->mode = Mode_Model;

	//Entities --------
	Entity e0 = Entity(glm::mat4(1.0), app->plane, 0, 0, EntityType::PLANE);
	//e0.worldMatrix = glm::translate(e0.worldMatrix, vec3(0.0, -1.0, 0.0));
	e0.worldMatrix = TransformPositionScale(vec3(0.0, -1.0, 0.0), vec3(100.0, 1.0, 100.0));
	e0.worldMatrix = TransformRotation(e0.worldMatrix, 88, { 1, 0, 0 });
	app->entities.push_back(e0);

	Entity e1 = Entity(glm::mat4(1.0), app->model, 0, 0, EntityType::PATRICK);
	e1.worldMatrix = glm::translate(e1.worldMatrix, vec3(3.0, 1.0, -6.0));
	app->entities.push_back(e1);

	Entity e3 = Entity(glm::mat4(1.0), app->model, 0, 0,EntityType::PATRICK);
	e3.worldMatrix = glm::translate(e3.worldMatrix, vec3(-3.0, 1.0, -6.0));
	app->entities.push_back(e3);

	Entity e2 = Entity(glm::mat4(1.0), app->model, 0, 0, EntityType::PATRICK);
	e2.worldMatrix = glm::translate(e2.worldMatrix, vec3(0.0, 1.0, 2.0));
	app->entities.push_back(e2);

	// Lights -----------

	app->lights.push_back(Light(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	app->lights.push_back(Light(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	
	app->fbo.Initialize(app->displaySize.x, app->displaySize.y);
}

void Gui(App* app)
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("App"))
	{
		if (ImGui::MenuItem("OpenGL info")) { app->show_opengl_info = true; }

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	if (app->show_opengl_info)
	{
		ImGui::Begin("Info", &app->show_opengl_info);
		
		// FPS information ------------------
		ImGui::Text("FPS: %f", 1.0f/app->deltaTime);

		// OpenGL information ------------------
		ImGui::Text("OpenGL version: %s", app->info.version.c_str());
		ImGui::Text("OpenGL renderer: %s", app->info.renderer.c_str());
		ImGui::Text("GPU vendor: %s", app->info.vendor.c_str());
		ImGui::Text("GLSL version: %s", app->info.shadingLanguageVersion.c_str());
		//for (int i = 0; i < app->info.extensions.size(); ++i)
			//ImGui::Text("Extension %i: %s", i, app->info.extensions[i].c_str());

		// Camera information -------------------
		ImGui::Separator();
		ImGui::Text("Camera");
		ImGui::DragFloat3("Position", &app->camera.position.x);
		

		ImGui::End();
	}

	ImGui::Image((ImTextureID)app->fbo.GetTexture(RENDER_TEXTURE), ImVec2(app->displaySize.x, app->displaySize.y), ImVec2(1, 1), ImVec2(0,0));
}



void Update(App* app)
{
    // You can handle app->input keyboard/mouse here
	app->camera.Update(app);

	// Global params

	MapBuffer(app->gpBuffer, GL_WRITE_ONLY);
	app->globalParamsOffset = app->gpBuffer.head;

	PushUInt(app->gpBuffer, app->lights.size());

	for (u32 i = 0; i < app->lights.size(); ++i)
	{
		AlignHead(app->gpBuffer, sizeof(vec4));

		const Light& light = app->lights[i];
		PushUInt(app->gpBuffer, static_cast<u32>(light.type));
		PushVec3(app->gpBuffer, light.position);
		PushVec3(app->gpBuffer, light.color);
		PushVec3(app->gpBuffer, light.direction);
	}

	app->globalParamsSize = app->gpBuffer.head - app->globalParamsOffset;

	UnmapBuffer(app->gpBuffer);


	//Update uniform blocks ------
	MapBuffer(app->ubuffer, GL_WRITE_ONLY);

	for (int i = 0; i < app->entities.size(); ++i)
	{
		app->ubuffer.head = Align(app->ubuffer.head, app->uniformBlockAlignment);
		app->entities[i].localParamsOffset = app->ubuffer.head;

		app->worldMatrix = app->entities[i].worldMatrix;
		app->worldViewProjectionMatrix = app->camera.projectionMatrix * app->camera.viewMatrix * app->entities[i].worldMatrix;

		PushMat4(app->ubuffer, app->worldMatrix);
		PushMat4(app->ubuffer, app->worldViewProjectionMatrix);

		app->entities[i].localParamsSize = app->ubuffer.head - app->entities[i].localParamsOffset;
	}

	UnmapBuffer(app->ubuffer);
}

void Render(App* app)
{
	// Clear the screen (also ImGui...)
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, app->displaySize.x, app->displaySize.y);

    switch (app->mode)
    {
	case Mode::Mode_TexturedQuad:
            {
                // TODO: Draw your textured quad here!
                // - clear the framebuffer
                // - set the viewport
                // - set the blending state
                // - bind the texture into unit 0
                // - bind the program 
                //   (...and make its texture sample from unit 0)
                // - bind the vao
                // - glDrawElements() !!!

				
				Program& programTexturedGeometry = app->programs[app->texturedGeometryProgramIdx];
				glUseProgram(programTexturedGeometry.handle);
				glBindVertexArray(app->vao);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glUniform1i(app->programUniformTexture, 0);
				glActiveTexture(GL_TEXTURE0);
				GLuint textureHandle = app->textures[app->diceTexIdx].handle;
				glBindTexture(GL_TEXTURE_2D, textureHandle);

				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

				glBindVertexArray(0);
				glUseProgram(0);

            }
            break;
		case Mode::Mode_Model:
		{
			
			app->fbo.Bind();

			Program& texturedMeshProgram = app->programs[app->texturedMeshProgramIdx];
			glUseProgram(texturedMeshProgram.handle);

			glBindBufferRange(GL_UNIFORM_BUFFER, BINDING(0), app->gpBuffer.handle, app->globalParamsOffset, app->globalParamsSize);

			for (int i = 0; i < app->entities.size(); ++i)
			{
				
				
					Model& model = app->models[app->entities[i].modelIndex];
					Mesh& mesh = app->meshes[model.meshIdx];
					glBindBufferRange(GL_UNIFORM_BUFFER, BINDING(1), app->ubuffer.handle, app->entities[i].localParamsOffset, app->entities[i].localParamsSize);


					for (u32 i = 0; i < mesh.submeshes.size(); ++i)
					{
						GLuint vao = FindVAO(mesh, i, texturedMeshProgram);
						glBindVertexArray(vao);

						u32 submeshMaterialIdx = model.materialIdx[i];
						Material& submeshMaterial = app->materials[submeshMaterialIdx];

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, app->textures[submeshMaterial.albedoTextureIdx].handle);
						glUniform1i(app->texturedMeshProgram_uTexture, 0);

						Submesh& submesh = mesh.submeshes[i];
						glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, (void*)(u64)submesh.indexOffset);
					}

				}		
			}

			app->fbo.Unbind();
		}
		break;


        default:;
    }

	/*Mesh& mesh = app->meshes[app->plane];
	Submesh& submesh = mesh.submeshes[0];

	glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, (void*)(u64)submesh.indexOffset);*/

	glBindVertexArray(0);
	glUseProgram(0);



	

}

GLuint FindVAO(Mesh& mesh, u32 submeshIndex, const Program& program)
{
	Submesh& submesh = mesh.submeshes[submeshIndex];

	// Try finding a vao for this submesh/program
	for (u32 i = 0; i < (u32)submesh.vaos.size(); ++i)
	{
		if (submesh.vaos[i].programHandle == program.handle)
			return submesh.vaos[i].handle;
	}

	GLuint vaoHandle = 0;

	// Create a new vao for this submesh/program

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferHandle);

	// We have to link all vertex inputs attributes to attributes in the vertex buffer

	for (u32 i = 0; i < program.vertexInputLayout.attributes.size(); ++i)
	{
		bool attributeWasLinked = false;

		for (u32 j = 0; j < submesh.vertexBufferLayout.attributes.size(); ++j)
		{
			if (program.vertexInputLayout.attributes[i].location == submesh.vertexBufferLayout.attributes[j].location)
			{
				const u32 index = submesh.vertexBufferLayout.attributes[j].location;
				const u32 ncomp = submesh.vertexBufferLayout.attributes[j].componentCount;
				const u32 offset = submesh.vertexBufferLayout.attributes[j].offset + submesh.vertexOffset; // attribute offset + vertex offset
				const u32 stride = submesh.vertexBufferLayout.stride;
				glVertexAttribPointer(index, ncomp, GL_FLOAT, GL_FALSE, stride, (void*)(u64)offset);
				glEnableVertexAttribArray(index);

				attributeWasLinked = true;
				break;
			}
		}

		assert(attributeWasLinked); // The submesh should provide an attribute for each vertex inputs
	}

	glBindVertexArray(0);

	// Store it in the list of vaos for this submesh
	Vao vao = { vaoHandle, program.handle };
	submesh.vaos.push_back(vao);

	return vaoHandle;
}


glm::mat4 TransformScale(const vec3& scaleFactors)
{
	glm::mat4 transform = scale(scaleFactors);
	return scale(scaleFactors);
}

glm::mat4 TransformPositionScale(const vec3& pos, const vec3& scaleFactors)
{
	glm::mat4 transform = translate(pos);
	transform = scale(transform, scaleFactors);
	return transform;
}

glm::mat4 TransformRotation(const glm::mat4& matrix, float angle, vec3 axis)
{
	float radians = glm::radians(angle);
	glm::mat4 transform = glm::rotate(matrix, radians, axis);
	return transform;
}