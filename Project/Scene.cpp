#pragma once
#include <iostream>
#include <vector>
#include <glm.hpp>
#include "Scene.h"
#include "shader.h"
#include "stb_image.h"

// ==============================================================
// Place for skybox
// ==============================================================
glm::mat4 OVR2glm(OVR::Matrix4f m)
{
	return glm::mat4(
		m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3],
		m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3],
		m.M[2][0], m.M[2][1], m.M[2][2], m.M[2][3],
		m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]
	);
}
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

GLuint Scene::CreateShader(GLenum type, const char* path)
{
	std::string Code;
	std::ifstream ShaderFile;
	ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		ShaderFile.open(path);
		std::stringstream ShaderStream;
		// read file's buffer contents into streams
		ShaderStream << ShaderFile.rdbuf();
		// close file handlers
		ShaderFile.close();
		// convert stream into string
		Code = ShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* src = Code.c_str();
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint r;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
	if (!r)
	{
		GLchar msg[1024];
		glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
		if (msg[0]) {
			OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
		}
		return 0;
	}

	return shader;
}


void Scene::Init(int includeIntensiveGPUobject)
{
	GLuint    vshader = CreateShader(GL_VERTEX_SHADER, "../../../Shader/normal.vs");
	GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, "../../../Shader/normal.fs");
	// Make textures
	ShaderFill * grid_material[4];
	for (int k = 0; k < 4; ++k)
	{
		static DWORD tex_pixels[256 * 256];
		for (int j = 0; j < 256; ++j)
		{
			for (int i = 0; i < 256; ++i)
			{
				if (k == 0) tex_pixels[j * 256 + i] = (((i >> 7) ^ (j >> 7)) & 1) ? 0xffb4b4b4 : 0xff505050;// floor
				if (k == 1) tex_pixels[j * 256 + i] = (((j / 4 & 15) == 0) || (((i / 4 & 15) == 0) && ((((i / 4 & 31) == 0) ^ ((j / 4 >> 4) & 1)) == 0)))
					? 0xff3c3c3c : 0xffb4b4b4;// wall
				if (k == 2) tex_pixels[j * 256 + i] = (i / 4 == 0 || j / 4 == 0) ? 0xff505050 : 0xffb4b4b4;// ceiling
				if (k == 3) tex_pixels[j * 256 + i] = 0xffffffff;// blank
			}
		}
		TextureBuffer * generated_texture = new TextureBuffer(false, Sizei(256, 256), 4, (unsigned char *)tex_pixels);
		grid_material[k] = new ShaderFill(vshader, fshader, generated_texture);
	}

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	// Construct geometry
	Model * m = new Model(Vector3f(0, 0, 0), grid_material[2]);  // Moving box
	m->AddSolidColorBox(0, 0, 0, +1.0f, +1.0f, 1.0f, 0xff404040);
	m->AllocateBuffers();
	Add(m);


	if (includeIntensiveGPUobject)// Why can't I remove this?
	{
		//m = new Model(Vector3f(0, 0, 0), grid_material[0]);  // Floors
		//for (float depth = 0.0f; depth > -3.0f; depth -= 0.1f)
		//    m->AddSolidColorBox(9.0f, 0.5f, -depth, -9.0f, 3.5f, -depth, 0x10ff80ff); // Partition
		//m->AllocateBuffers();
		////Add(m);
	}

	m = new Model(Vector3f(0, 0, 0), grid_material[0]);  // Floors
	m->AddSolidColorBox(-10.0f, -0.1f, -20.0f, 10.0f, 0.0f, 20.1f, 0xff808080); // Main floor
	m->AddSolidColorBox(-15.0f, -6.1f, 18.0f, 15.0f, -6.0f, 30.0f, 0xff808080); // Bottom floor
	m->AllocateBuffers();
	Add(m);


	m = new Model(Vector3f(0, 0, 0), grid_material[3]);  // Fixtures & furniture
	m->AddSolidColorBox(9.5f, 0.75f, 3.0f, 10.1f, 2.5f, 3.1f, 0xff383838);   // Right side shelf// Verticals
	m->AddSolidColorBox(9.5f, 0.95f, 3.7f, 10.1f, 2.75f, 3.8f, 0xff383838);   // Right side shelf
	m->AddSolidColorBox(9.55f, 1.20f, 2.5f, 10.1f, 1.30f, 3.75f, 0xff383838); // Right side shelf// Horizontals
	m->AddSolidColorBox(9.55f, 2.00f, 3.05f, 10.1f, 2.10f, 4.2f, 0xff383838); // Right side shelf
	m->AddSolidColorBox(5.0f, 1.1f, 20.0f, 10.0f, 1.2f, 20.1f, 0xff383838);   // Right railing   
	m->AddSolidColorBox(-10.0f, 1.1f, 20.0f, -5.0f, 1.2f, 20.1f, 0xff383838);   // Left railing  
	for (float f = 5.0f; f <= 9.0f; f += 1.0f)
	{
		m->AddSolidColorBox(f, 0.0f, 20.0f, f + 0.1f, 1.1f, 20.1f, 0xff505050);// Left Bars
		m->AddSolidColorBox(-f, 1.1f, 20.0f, -f - 0.1f, 0.0f, 20.1f, 0xff505050);// Right Bars
	}
	m->AddSolidColorBox(-1.8f, 0.8f, 1.0f, 0.0f, 0.7f, 0.0f, 0xff505000); // Table
	m->AddSolidColorBox(-1.8f, 0.0f, 0.0f, -1.7f, 0.7f, 0.1f, 0xff505000); // Table Leg 
	m->AddSolidColorBox(-1.8f, 0.7f, 1.0f, -1.7f, 0.0f, 0.9f, 0xff505000); // Table Leg 
	m->AddSolidColorBox(0.0f, 0.0f, 1.0f, -0.1f, 0.7f, 0.9f, 0xff505000); // Table Leg 
	m->AddSolidColorBox(0.0f, 0.7f, 0.0f, -0.1f, 0.0f, 0.1f, 0xff505000); // Table Leg 
	m->AddSolidColorBox(-1.4f, 0.5f, -1.1f, -0.8f, 0.55f, -0.5f, 0xff202050); // Chair Set
	m->AddSolidColorBox(-1.4f, 0.0f, -1.1f, -1.34f, 1.0f, -1.04f, 0xff202050); // Chair Leg 1
	m->AddSolidColorBox(-1.4f, 0.5f, -0.5f, -1.34f, 0.0f, -0.56f, 0xff202050); // Chair Leg 2
	m->AddSolidColorBox(-0.8f, 0.0f, -0.5f, -0.86f, 0.5f, -0.56f, 0xff202050); // Chair Leg 2
	m->AddSolidColorBox(-0.8f, 1.0f, -1.1f, -0.86f, 0.0f, -1.04f, 0xff202050); // Chair Leg 2
	m->AddSolidColorBox(-1.4f, 0.97f, -1.05f, -0.8f, 0.92f, -1.10f, 0xff202050); // Chair Back high bar

	for (float f = 3.0f; f <= 6.6f; f += 0.4f)
		m->AddSolidColorBox(-3, 0.0f, f, -2.9f, 1.3f, f + 0.1f, 0xff404040); // Posts

	m->AllocateBuffers();
	Add(m);


	// ===================================================
	// skybox 
	// ===================================================
	//GLuint    vshader_sky = CreateShader(GL_VERTEX_SHADER, "../../../Shader/skybox.vs");
	//GLuint    fshader_sky = CreateShader(GL_FRAGMENT_SHADER, "../../../Shader/skybox.fs");

	SkyBox.skyboxShader = new Shader("../../../Shader/skybox.vs", "../../../Shader/skybox.fs");
	glGenVertexArrays(1, &SkyBox.skyboxVAO);
	glGenBuffers(1, &SkyBox.skyboxVBO);
	glBindVertexArray(SkyBox.skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, SkyBox.skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------

	std::vector<std::string> faces
	{
		"../../../Src/ame_nebula/purplenebula_rt.tga",
		"../../../Src/ame_nebula/purplenebula_lf.tga",
		"../../../Src/ame_nebula/purplenebula_up.tga",
		"../../../Src/ame_nebula/purplenebula_dn.tga",
		"../../../Src/ame_nebula/purplenebula_ft.tga",
		"../../../Src/ame_nebula/purplenebula_bk.tga"
	};

	SkyBox.cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------
	//ShaderFill SkyboxShader(vshader_sky, fshader_sky, cubemapTexture);
	SkyBox.skyboxShader->use();
	SkyBox.skyboxShader->setInt("skybox", 0);

}

void Scene::Render(Matrix4f view, Matrix4f proj)
{
	for (int i = 0; i < numModels; ++i)
		Models[i]->Render(view, proj);

	// ==============================================================
	// Place for skybox
	// ==============================================================

	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	SkyBox.skyboxShader->use();
	SkyBox.skyboxShader->setMat4("view", OVR2glm(view));
	SkyBox.skyboxShader->setMat4("projection", OVR2glm(proj));
	// skybox cube
	glBindVertexArray(SkyBox.skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBox.cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}
