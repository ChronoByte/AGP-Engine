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
#include<time.h>



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
	app->finalPassShaderIdx = LoadProgram(app, "shaders.glsl", "FINAL_PASS_SHADER");
	Program& finalPassShader = app->programs[app->finalPassShaderIdx];

	glUseProgram(finalPassShader.handle);
	glUniform1i(glGetUniformLocation(finalPassShader.handle, "sceneTexture"), 0);
	glUniform1i(glGetUniformLocation(finalPassShader.handle, "bloomBlurTexture"), 1);
	glUseProgram(0);



	app->geometryPassShaderID = LoadProgram(app, "shaders.glsl", "GEOMETRY_PASS_SHADER");
	Program& geometryPassShader = app->programs[app->geometryPassShaderID];
	app->programGPassUniformTexture = glGetUniformLocation(geometryPassShader.handle, "uTexture");

	{
		int attributeCount;
		glGetProgramiv(geometryPassShader.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);

		GLchar attributeName[64];
		GLsizei attributeNameLength;
		GLint attributeSize;
		GLenum attributeType;

		for (int i = 0; i < attributeCount; ++i)
		{
			glGetActiveAttrib(geometryPassShader.handle, i, 64, &attributeNameLength, &attributeSize, &attributeType, attributeName);

			GLint attributeLocation = glGetAttribLocation(geometryPassShader.handle, attributeName);
			geometryPassShader.vertexInputLayout.attributes.push_back({ (u8)attributeLocation,(u8)attributeSize });
		}
	}
	
	app->shadingPassShaderID = LoadProgram(app, "shaders.glsl", "SHADING_PASS_SHADER");
	Program& shadingPassShader = app->programs[app->shadingPassShaderID];
	app->programShadingPassUniformTexturePosition = glGetUniformLocation(shadingPassShader.handle, "gPosition");
	app->programShadingPassUniformTextureNormals = glGetUniformLocation(shadingPassShader.handle, "gNormal");
	app->programShadingPassUniformTextureAlbedo = glGetUniformLocation(shadingPassShader.handle, "gAlbedoSpec");
	app->programShadingPassUniformTextureDepth = glGetUniformLocation(shadingPassShader.handle, "gDepth");

	{
		int attributeCount;
		glGetProgramiv(shadingPassShader.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);

		GLchar attributeName[64];
		GLsizei attributeNameLength;
		GLint attributeSize;
		GLenum attributeType;

		for (int i = 0; i < attributeCount; ++i)
		{
			glGetActiveAttrib(shadingPassShader.handle, i, 64, &attributeNameLength, &attributeSize, &attributeType, attributeName);

			GLint attributeLocation = glGetAttribLocation(shadingPassShader.handle, attributeName);
			shadingPassShader.vertexInputLayout.attributes.push_back({ (u8)attributeLocation,(u8)attributeSize });
		}

	}

	app->lightsShaderID = LoadProgram(app, "shaders.glsl", "LIGHTS_SHADER");
	Program& lightsShader = app->programs[app->lightsShaderID];
	app->programLightsUniformColor = glGetUniformLocation(lightsShader.handle, "lightColor");
	app->programLightsUniformWorldMatrix = glGetUniformLocation(lightsShader.handle, "uWorldViewProjectionMatrix");

	{
		int attributeCount;
		glGetProgramiv(lightsShader.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);

		GLchar attributeName[64];
		GLsizei attributeNameLength;
		GLint attributeSize;
		GLenum attributeType;

		for (int i = 0; i < attributeCount; ++i)
		{
			glGetActiveAttrib(lightsShader.handle, i, 64, &attributeNameLength, &attributeSize, &attributeType, attributeName);

			GLint attributeLocation = glGetAttribLocation(lightsShader.handle, attributeName);
			lightsShader.vertexInputLayout.attributes.push_back({ (u8)attributeLocation,(u8)attributeSize });
		}

	}

	app->blurShaderID = LoadProgram(app, "shaders.glsl", "BLUR_SHADER");
	Program& blurShader = app->programs[app->blurShaderID];

	glUseProgram(blurShader.handle);
	glUniform1i(glGetUniformLocation(blurShader.handle, "image"), 0);

	// Attributes Program ----------
	{
		int attributeCount;
		glGetProgramiv(blurShader.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);

		GLchar attributeName[64];
		GLsizei attributeNameLength;
		GLint attributeSize;
		GLenum attributeType;

		for (int i = 0; i < attributeCount; ++i)
		{
			glGetActiveAttrib(blurShader.handle, i, 64, &attributeNameLength, &attributeSize, &attributeType, attributeName);

			GLint attributeLocation = glGetAttribLocation(blurShader.handle, attributeName);
			blurShader.vertexInputLayout.attributes.push_back({ (u8)attributeLocation,(u8)attributeSize });
		}
	}
	glUseProgram(0);


	// Textures 
	app->diceTexIdx = LoadTexture2D(app, "dice.png");
	app->whiteTexIdx = LoadTexture2D(app, "color_white.png");
	app->blackTexIdx = LoadTexture2D(app, "color_black.png");
	app->normalTexIdx = LoadTexture2D(app, "color_normal.png");
	app->magentaTexIdx = LoadTexture2D(app, "color_magenta.png");



	//--------------------- MODEL ---------------------- //
	


	
	// Camera -----------
	app->camera = Camera(vec3(31, 17.0f, 45.0f), vec3(-129.0f, -27.f, 0.f));
	
	app->camera.aspect_ratio = (float)app->displaySize.x / (float)app->displaySize.y;
	app->camera.projectionMatrix = glm::perspective(glm::radians(app->camera.vertical_fov), app->camera.aspect_ratio, app->camera.nearPlane, app->camera.farPlane);
	app->camera.viewMatrix = app->camera.CalculateCameraRotation(app);

	//app->camera.viewMatrix = glm::lookAt(app->camera.position, app->camera.target, glm::vec3(0, 1, 0));
	

	// Program ----------
	app->texturedMeshProgramIdx = LoadProgram(app, "shaders.glsl", "SHOW_TEXTURED_MESH");
	Program& texturedMeshProgram = app->programs[app->texturedMeshProgramIdx];


	// Attributes Program ----------
	{
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
			texturedMeshProgram.vertexInputLayout.attributes.push_back({ (u8)attributeLocation,(u8)attributeSize });
		}
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
	app->sphere = app->geo.LoadSphere(app);

	app->mode = Mode_Model;

	// -------------------------------- ENTITIES --------------------------------

	Entity e0 = Entity(glm::mat4(1.0), app->plane, 0, 0, EntityType::PLANE);
	e0.worldMatrix = TransformPositionScale(vec3(0.0, -1.0, 0.0), vec3(100.0, 1.0, 100.0));
	e0.worldMatrix = TransformRotation(e0.worldMatrix, 90, { 1, 0, 0 });
	app->entities.push_back(e0);

	const int COLUMNS = 6;
	const int ROWS = 6;
	const u32 distance = 6;

	for (int x = -ROWS; x < ROWS; ++x)
	{
		for (int y = -COLUMNS; y < COLUMNS; ++y)
		{
			Entity e1 = Entity(glm::mat4(1.0), app->model, 0, 0, EntityType::PATRICK);
			e1.worldMatrix = TransformPositionScale(vec3((float)x * (float)distance, 2.4f, (float)y * (float)distance), vec3(1.0f));
			app->entities.push_back(e1);
		}
	}
	
	// -------------------------------- LIGHTS --------------------------------

	// ----------- Point Lights -----------
	const int LIGHTS = 6;
	const u32 offset = 2;
	std::srand(time(NULL));

	for (int x = -LIGHTS; x < LIGHTS; ++x)
	{
		for (int y = -LIGHTS; y < LIGHTS; ++y)
		{
			vec3 color = GenerateRandomBrightColor();
			app->lights.push_back(Light(glm::vec3((float)x * (float)distance, 1.0f, y * (float)distance + (float)offset), glm::vec3(color.x, color.y, color.z)));
		}
	}

	// ----------- Directional Lights -----------
	app->lights.push_back(Light(glm::vec3(-20.0f, 45.0f, 3.f), glm::vec3(1.0f, 1.0f, 1.0f), LightType::LIGHT_TYPE_DIRECTIONAL, glm::vec3(-1.0, -1.0, 0.0), 10U));
	app->lights.push_back(Light(glm::vec3(22.0f, 35.0f, -6.f), glm::vec3(0.3f, 0.0f, 0.0f), LightType::LIGHT_TYPE_DIRECTIONAL, glm::vec3(0.0, -1.0, 0.0), 10U));
	

	// -------------------------------- RELIEF MAPPING --------------------------------

	//Shader
	app->reliefMapShaderID = LoadProgram(app, "shaders.glsl", "RELIEF_MAPPING_SHADER");
	Program& reliefMapShader = app->programs[app->reliefMapShaderID];


	// Attributes Program ----------
	{
		int attributeCount;
		glGetProgramiv(reliefMapShader.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);

		GLchar attributeName[64];
		GLsizei attributeNameLength;
		GLint attributeSize;
		GLenum attributeType;

		for (int i = 0; i < attributeCount; ++i)
		{
			glGetActiveAttrib(reliefMapShader.handle, i, 64, &attributeNameLength, &attributeSize, &attributeType, attributeName);

			GLint attributeLocation = glGetAttribLocation(reliefMapShader.handle, attributeName);
			reliefMapShader.vertexInputLayout.attributes.push_back({ (u8)attributeLocation,(u8)attributeSize });
		}
	}

	//Relief Textures
	app->reliefDiffuseIdx = LoadTexture2D(app, "Relief/bricks2.jpg");
	app->reliefNormalIdx = LoadTexture2D(app, "Relief/bricks2_normal.jpg");
	app->reliefHeightIdx = LoadTexture2D(app, "Relief/bricks2_disp.jpg");

	app->relief2DiffuseIdx = LoadTexture2D(app, "Relief/LeatherPadded_03_BC.png");
	app->relief2NormalIdx = LoadTexture2D(app, "Relief/LeatherPadded_03_NOpenGL.png");
	app->relief2HeightIdx = LoadTexture2D(app, "Relief/LeatherPadded_03_H.png");
	

	glUseProgram(reliefMapShader.handle);
	glUniform1i(glGetUniformLocation(reliefMapShader.handle, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(reliefMapShader.handle, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(reliefMapShader.handle, "depthMap"), 2);


	srand(20);
	const int RELIEFS = 3;
	const u32 distance2 = 12;
	for (int i = 0; i < 36; i++)
	{
		app->ReliefRotationRate.push_back((float)std::rand() / (RAND_MAX) * 6.28 - 3.14);
		app->ReliefAngles.push_back(0.0f);
	}
	for (int x = -RELIEFS; x < RELIEFS; ++x)
	{
		for (int y = -RELIEFS; y < RELIEFS; ++y)
		{
			app->ReliefPositions.push_back(glm::vec3((float)x * (float)distance2, 15.0f, (float)y * (float)distance));
		}
	}
	
	// FBO --------------
	app->gFbo.Initialize(app->displaySize.x, app->displaySize.y);
	app->shadingFbo.Initialize(app->displaySize.x, app->displaySize.y);
	app->blurFbo.Initialize(app->displaySize.x, app->displaySize.y);
}

void Gui(App* app)
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("App"))
	{
		if (ImGui::MenuItem("OpenGL info")) { app->show_opengl_info = true; }
		if (ImGui::MenuItem("Effects")) { app->show_effects = true; }

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	if (app->show_opengl_info)
	{
		ImGui::Begin("Info", &app->show_opengl_info);

		// FPS information ------------------
		ImGui::Text("FPS: %f", 1.0f / app->deltaTime);

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
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("X", &app->camera.position.x);
		ImGui::SameLine();
		ImGui::DragFloat("Y", &app->camera.position.y);
		ImGui::SameLine();
		ImGui::DragFloat("Z", &app->camera.position.z);
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(80);
		ImGui::DragFloat("Yaw", &app->camera.yaw);
		ImGui::SameLine();
		ImGui::DragFloat("Pitch", &app->camera.pitch);
		ImGui::SameLine();
		ImGui::DragFloat("Roll", &app->camera.roll);
		ImGui::PopItemWidth();

		// Render Pipeline information -------------------

		ImGui::Separator();
		ImGui::Text("Render Pipeline");
		const char* items_pipeline_list[] = { "Forward Shading", "Deferred Shading" };
		static int item_pipeline = (int)app->render_pipeline;
		if (ImGui::Combo("Render Pipeline", &item_pipeline, items_pipeline_list, IM_ARRAYSIZE(items_pipeline_list)))
		{
			app->render_pipeline = (RenderPipeline)item_pipeline;
		}

		// Render target information -------------------
		ImGui::Separator();
		ImGui::Text("Render Targets");
		const char* items[] = { "Default", "G_Position", "G_Normals", "G_Albedo", "Depth Texture", "Bright Color Texture", "Blurred Bloom Texture" };
		static int item_current = 0;
		if (ImGui::Combo("Render Target", &item_current, items, IM_ARRAYSIZE(items)))
		{
			app->displayedTexture = (RenderTargetType)item_current;
		}

		

	
		// Light information -------------------
		ImGui::Separator();
		ImGui::Text("Lights");
		ImGui::Spacing();
		if (ImGui::TreeNode("Directional Lights"))
		{
			int j = 1;
			for (int i = 0; i < app->lights.size(); ++i) {
				if (app->lights[i].type == LightType::LIGHT_TYPE_DIRECTIONAL)
				{
					ImGui::PushID(i);

					ImGui::Text("Directional Light %d", j);
					ImGui::ColorEdit3("color", glm::value_ptr(app->lights[i].color), ImGuiColorEditFlags_::ImGuiColorEditFlags_Uint8);
					ImGui::DragFloat3("direction", glm::value_ptr(app->lights[i].direction), 0.01f);
					ImGui::DragInt("intensity", (int*)&app->lights[i].intensity, 0.5f, 0, 100);
					j++;

					ImGui::PopID();
				}
				//ImGui::NewLine();
			}
		ImGui::TreePop();
		}
	

		if (ImGui::TreeNode("Point Lights"))
		{
			int j = 1;
			for (int i = 0; i < app->lights.size(); ++i) {
				if (app->lights[i].type == LightType::LIGHT_TYPE_POINT)
				{
					ImGui::PushID(i);
					ImGui::Text("Point Light %d", j);
					ImGui::ColorEdit3("color", glm::value_ptr(app->lights[i].color), ImGuiColorEditFlags_::ImGuiColorEditFlags_Uint8);
					ImGui::DragFloat3("position", glm::value_ptr(app->lights[i].position), 0.01f);
					ImGui::DragInt("intensity", (int*)&app->lights[i].intensity, 0.5f, 0, 100);
					j++;
					ImGui::PopID();
				}
				//ImGui::NewLine();
			}
		ImGui::TreePop();
		}


	ImGui::End();

	}
	if (app->show_effects)
	{
		ImGui::Begin("Effects", &app->show_effects);

		
		ImGui::Text("Relief Mapping");
		ImGui::Spacing();
		ImGui::Checkbox("Clip borders", &app->clip_borders);
		ImGui::SliderFloat("Height Scale", &app->heigth_scale, 0.0f, 1.f);
		ImGui::SliderInt("Min layers", &app->min_layers, 0.0f, 100.f);
		ImGui::SliderInt("Max layers", &app->max_layers, 0.0f, 100.f);
		ImGui::SliderInt("Textures", &app->reliefIdx, 0.0f, 1.f);
		ImGui::Separator();

		
		ImGui::Text("Bloom");
		ImGui::Spacing();
		ImGui::Checkbox("Bloom", &app->using_bloom);
		ImGui::DragInt("Blur Iterations", &app->blurIterations, 2.0f, 0, 50);
		ImGui::DragFloat("Threshold", &app->bright_threshold, 0.05f, 0.0f, 20.f);



		ImGui::End();
	}
}



void Update(App* app)
{
    // You can handle app->input keyboard/mouse here
	app->camera.Update(app);

	// Resize window

	if (app->lastFrameDisplaySize != app->displaySize)
	{
		app->gFbo.Resize(app->displaySize.x, app->displaySize.y);
		app->shadingFbo.Resize(app->displaySize.x, app->displaySize.y);
	}
	app->lastFrameDisplaySize = app->displaySize;

	// Global params

	MapBuffer(app->gpBuffer, GL_WRITE_ONLY);
	app->globalParamsOffset = app->gpBuffer.head;

	PushVec3(app->gpBuffer, app->camera.position);
	PushUInt(app->gpBuffer, app->lights.size());

	for (u32 i = 0; i < app->lights.size(); ++i)
	{
		AlignHead(app->gpBuffer, sizeof(vec4));

		const Light& light = app->lights[i];
		PushUInt(app->gpBuffer, static_cast<u32>(light.type));
		PushVec3(app->gpBuffer, light.position);
		PushVec3(app->gpBuffer, light.color);
		PushVec3(app->gpBuffer, light.direction);
		PushUInt(app->gpBuffer, light.intensity);
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

void renderQuad();
void renderQuadTangentSpace();
void BindReliefTextures(int reliefIndex, App* app);
u32 GetFinalTextureToRender(App* app);



void Render(App* app)
{
	switch (app->render_pipeline)
	{
		case RenderPipeline::DEFERRED:
			RenderUsingDeferredPipeline(app);
			break;
		case RenderPipeline::FORWARD:
			RenderUsingForwardPipeline(app);
			break;
		default:
			break;
	}

}

void RenderUsingDeferredPipeline(App* app)
{
	// Clear the screen (also ImGui...)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, app->displaySize.x, app->displaySize.y);
	glEnable(GL_DEPTH_TEST);

	app->gFbo.Bind();

	// --------------------------------------- RELIEF MAPPING -------------------------------------

	Program& reliefMapShading = app->programs[app->reliefMapShaderID];
	glUseProgram(reliefMapShading.handle);

	glUniformMatrix4fv(glGetUniformLocation(reliefMapShading.handle, "projection"), 1, GL_FALSE, (GLfloat*)&app->camera.projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(reliefMapShading.handle, "view"), 1, GL_FALSE, (GLfloat*)&app->camera.viewMatrix);



	glUniform3f(glGetUniformLocation(reliefMapShading.handle, "viewPos"), app->camera.position.x, app->camera.position.y, app->camera.position.z);
	glUniform3f(glGetUniformLocation(reliefMapShading.handle, "lightPos"), app->lights[0].position.x, app->lights[0].position.y, app->lights[0].position.z);
	glUniform1f(glGetUniformLocation(reliefMapShading.handle, "heightScale"), app->heigth_scale);
	glUniform1i(glGetUniformLocation(reliefMapShading.handle, "clipBorders"), app->clip_borders);
	glUniform1i(glGetUniformLocation(reliefMapShading.handle, "minLayers"), app->min_layers);
	glUniform1i(glGetUniformLocation(reliefMapShading.handle, "maxLayers"), app->max_layers);

	
	BindReliefTextures(app->reliefIdx, app);

	//for (int i = 0; i < 36; i++)
	//{	
	//modelMatrix = glm::rotate(modelMatrix, app->ReliefAngles[i] += app->ReliefRotationRate[i] * app->deltaTime, glm::vec3(1.0f, 0.50f, 0.70f));
	//}

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = TransformPositionScale(vec3(0.f, 25.0f, 0.f), vec3(15.0f));
	modelMatrix = TransformRotation(modelMatrix, app->ReliefAngles[0] += app->ReliefRotationRate[0] * app->deltaTime * 5.0f, glm::vec3(1.0f, 0.50f, 0.70f));
	glUniformMatrix4fv(glGetUniformLocation(reliefMapShading.handle, "model"), 1, GL_FALSE, (GLfloat*)&modelMatrix);
	renderQuadTangentSpace();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	// --------------------------------------- RENDERING ENTITIES -------------------------------------

	Program& texturedMeshProgram = app->programs[app->geometryPassShaderID];
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

	app->gFbo.Unbind();

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);


	// --------------------------------------- SHADING PASS -------------------------------------

	glDisable(GL_DEPTH_TEST);

	app->shadingFbo.Bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Program& shaderPassProgram = app->programs[app->shadingPassShaderID];
	glUseProgram(shaderPassProgram.handle);

	glBindBufferRange(GL_UNIFORM_BUFFER, BINDING(0), app->gpBuffer.handle, app->globalParamsOffset, app->globalParamsSize);

	glUniform1i(app->programShadingPassUniformTexturePosition, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, app->gFbo.GetTexture(G_POSITION_TEXTURE));
	glUniform1i(app->programShadingPassUniformTextureNormals, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, app->gFbo.GetTexture(G_NORMALS_TEXTURE));
	glUniform1i(app->programShadingPassUniformTextureAlbedo, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, app->gFbo.GetTexture(G_ALBEDO_TEXTURE));
	glUniform1i(app->programShadingPassUniformTextureDepth, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, app->gFbo.GetTexture(DEPTH_TEXTURE));

	glUniform1f(glGetUniformLocation(shaderPassProgram.handle, "bright_color_threshold"), app->bright_threshold);


	renderQuad();

	app->shadingFbo.Unbind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);

	// --------------------------------------- LIGHTS RENDERING --------------------------------------

	// ================ COPY DEPTH ATTACHMENT ================

	glBindFramebuffer(GL_READ_FRAMEBUFFER, app->gFbo.GetTexture(FBO));
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, app->shadingFbo.GetTexture(FBO)); // write to default framebuffer
	glBlitFramebuffer(0, 0, app->displaySize.x, app->displaySize.y, 0, 0, app->displaySize.x, app->displaySize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ================  RENDER LIGHT MESHES  ================

	app->shadingFbo.Bind(false);

	Program& lightsShader = app->programs[app->lightsShaderID];
	glUseProgram(lightsShader.handle);

	for (u32 i = 0; i < app->lights.size(); ++i)
	{
		// ------------------  Model  ------------------

		u32 modelIndex = 0U;
		glm::mat4 worldMatrix;
		switch (app->lights[i].type)
		{
		case LightType::LIGHT_TYPE_DIRECTIONAL:
			modelIndex = app->plane;
			worldMatrix = TransformPositionScale(app->lights[i].position, vec3(3.0f, 3.0f, 3.0f));
			worldMatrix = TransformRotation(worldMatrix, 90.0, vec3(1.f, 0.f, 0.f));
			//worldMatrix += app->lights[i].CalculateLightRotation();
			//worldMatrix += glm::lookAt(app->lights[i].position, app->lights[i].position + app->lights[i].direction, vec3(0, 1, 0));
			// Translate it here
			break;
		case LightType::LIGHT_TYPE_POINT:
			modelIndex = app->sphere;
			worldMatrix = TransformPositionScale(app->lights[i].position, vec3(0.3f, 0.3f, 0.3f));
			break;
		}

		// ------------------  Uniforms  ------------------
		glm::mat4 worldViewProjectionMatrix = app->camera.projectionMatrix * app->camera.viewMatrix * worldMatrix;
		glUniformMatrix4fv(app->programLightsUniformWorldMatrix, 1, GL_FALSE, (GLfloat*)&worldViewProjectionMatrix);
		glUniform3f(app->programLightsUniformColor, app->lights[i].color.x, app->lights[i].color.y, app->lights[i].color.z);

		// ----------------------------------------------

		Mesh& mesh = app->meshes[app->models[modelIndex].meshIdx];
		GLuint vao = FindVAO(mesh, 0, lightsShader);
		glBindVertexArray(vao);

		glDrawElements(GL_TRIANGLES, mesh.submeshes[0].indices.size(), GL_UNSIGNED_INT, (void*)(u64)mesh.submeshes[0].indexOffset);

	}

	app->shadingFbo.Unbind();

	// --------------------------------------- BLOOM PASS -------------------------------------

	Program& blurShader = app->programs[app->blurShaderID];
	app->blurFbo.BlurImage(app->blurIterations, app->shadingFbo.GetTexture(BRIGHT_COLOR_TEXTURE), blurShader, renderQuad);

	// --------------------------------------- RENDER SCREEN QUAD -------------------------------------

	Program& finalPassShader = app->programs[app->finalPassShaderIdx];
	glUseProgram(finalPassShader.handle);

	glUniform1i(glGetUniformLocation(finalPassShader.handle, "using_bloom"), app->using_bloom);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GetFinalTextureToRender(app));

	if (app->using_bloom && app->displayedTexture == RENDER_TEXTURE)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, app->blurFbo.GetBlurredTexture());
	}

	renderQuad();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	// -------------------------------------------------------------------------------------------------
}

void RenderUsingForwardPipeline(App* app)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, app->displaySize.x, app->displaySize.y);
	glEnable(GL_DEPTH_TEST);


}


unsigned int quadVAO2 = 0;
unsigned int quadVBO2;
void renderQuadTangentSpace()
{
	if (quadVAO2 == 0)
	{
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO2);
		glGenBuffers(1, &quadVBO2);
		glBindVertexArray(quadVAO2);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO2);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void BindReliefTextures(int reliefIndex, App* app)
{

	switch (reliefIndex)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, app->textures[app->reliefDiffuseIdx].handle);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, app->textures[app->reliefNormalIdx].handle);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, app->textures[app->reliefHeightIdx].handle);

		break;
	case 1:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, app->textures[app->relief2DiffuseIdx].handle);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, app->textures[app->relief2NormalIdx].handle);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, app->textures[app->relief2HeightIdx].handle);

		break;

	}

}


unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] =
		{
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
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

vec3 GenerateRandomBrightColor()
{
	int rgb[3];
	rgb[0] = std::rand() % 256;  // red
	rgb[1] = std::rand() % 256;  // green
	rgb[2] = std::rand() % 256;  // blue

	// find max and min indexes.
	int max, min;

	if (rgb[0] > rgb[1])
	{
		max = (rgb[0] > rgb[2]) ? 0 : 2;
		min = (rgb[1] < rgb[2]) ? 1 : 2;
	}
	else
	{
		max = (rgb[1] > rgb[2]) ? 1 : 2;
		int notmax = 1 + max % 2;
		min = (rgb[0] < rgb[notmax]) ? 0 : notmax;
	}
	rgb[max] = 255;
	rgb[min] = 0;


	return vec3((float)rgb[0], (float)rgb[1], (float)rgb[2]) / 255.f;
}

u32 GetFinalTextureToRender(App* app)
{
	u32 texture = 0U; 

	if (app->displayedTexture == RENDER_TEXTURE || app->displayedTexture == BRIGHT_COLOR_TEXTURE)
	{
		texture = app->shadingFbo.GetTexture(app->displayedTexture);
	}
	else if (app->displayedTexture == BLURRED_TEXTURE)
	{
		texture = app->blurFbo.GetBlurredTexture();
	}
	else
	{
		texture = app->gFbo.GetTexture(app->displayedTexture);
	}

	return texture;
}

