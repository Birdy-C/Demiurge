#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "Scene.h"
#include "shader.h"
#include "stb-master/stb_image.h"

// ==============================================================
// Place for skybox
// ==============================================================
glm::mat4 OVR2glm(OVR::Matrix4f m)
{
	glm::mat4 T(
		m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3],
		m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3],
		m.M[2][0], m.M[2][1], m.M[2][2], m.M[2][3],
		m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]
	);
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
	ShaderFill * grid_material;
	static DWORD tex_pixels[256 * 256];
	for (int j = 0; j < 256; ++j)
	{
		for (int i = 0; i < 256; ++i)
		{
			tex_pixels[j * 256 + i] = (((i >> 7) ^ (j >> 7)) & 1) ? 0xffb4b4b4 : 0xff505050;// floor			}
		}
	}
	int width, height, nrChannels;

	unsigned char *data = stbi_load("../../../Src/skybox/right.jpg", &width, &height, &nrChannels, 0);

	//TextureBuffer * generated_texture = new TextureBuffer(false, Sizei(width, height), 4, (unsigned char *)tex_pixels);
	TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);

	grid_material = new ShaderFill(vshader, fshader, generated_texture);

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	Model *m = new Model(Vector3f(0, 0, 0), grid_material);  // Floors
	m->AddSolidColorBox(-10.0f, -0.1f, -20.0f, 10.0f, 0.0f, 20.1f, 0xff808080); // Main floor
	m->AddSolidColorBox(-15.0f, -6.1f, 18.0f, 15.0f, -6.0f, 30.0f, 0xff808080); // Bottom floor
	m->AllocateBuffers();
	//Add(m);



	// ===================================================
	// skybox 
	// ===================================================
	GLuint    vshader_sky = CreateShader(GL_VERTEX_SHADER, "../../../Shader/skybox.vs");
	GLuint    fshader_sky = CreateShader(GL_FRAGMENT_SHADER, "../../../Shader/skybox.fs");

	std::vector<std::string> faces
	{
		"../../../Src/skybox/top.jpg",
		"../../../Src/skybox/bottom.jpg",
		"../../../Src/skybox/front.jpg",
		"../../../Src/skybox/back.jpg",
		"../../../Src/skybox/left.jpg",
		"../../../Src/skybox/right.jpg"
		//	"../../../Src/ame_nebula/purplenebula_rt.tga",
		//	"../../../Src/ame_nebula/purplenebula_lf.tga",
		//	"../../../Src/ame_nebula/purplenebula_up.tga",
		//	"../../../Src/ame_nebula/purplenebula_dn.tga",
		//	"../../../Src/ame_nebula/purplenebula_ft.tga",
		//	"../../../Src/ame_nebula/purplenebula_bk.tga"
	};

	SkyBox.cubemapTexture = loadCubemap(faces);

	ShaderFill *grid_materialsky[6];
	TextureBuffer * generated_texturesky;
	for (int i = 0; i < 6; i++)
	{
		data = stbi_load(faces.at(i).c_str(), &width, &height, &nrChannels, 0);
		generated_texturesky = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
		grid_materialsky[i] = new ShaderFill(vshader, fshader, generated_texturesky);

		Model *m_sky = new Model(Vector3f(0, 0, 0), grid_materialsky[i]);
		m_sky->AddPlane(-10.0f, -10.0f, -10.0f, 10.0f, 10.0f, 10.0f, i);
		m_sky->AllocateBuffers();
		Add(m_sky);
	}
	//data = stbi_load(faces.at(0).c_str(), &width, &height, &nrChannels, 0);
	//generated_texturesky = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
	//grid_materialsky[0] = new ShaderFill(vshader, fshader, generated_texturesky);

	//Model *m_sky = new Model(Vector3f(0, 0, 0), grid_materialsky[0]);
	//m_sky->AddSolidSkyBox(-10.0f, -10.0f, -10.0f, 10.0f, 10.0f, 10.0f, 0xFFFFFFFF);
	////m_sky->AddPlane(-10.0f, -10.0f, -10.0f, 10.0f, 10.0f, 10.0f, i);
	//m_sky->AllocateBuffers();
	//Add(m_sky);

	//TextureBuffer * generated_texturesky = new TextureBuffer(SkyBox.cubemapTexture);
	//ShaderFill * grid_materialsky = new ShaderFill(vshader, fshader, generated_texturesky);

	glDeleteShader(vshader_sky);
	glDeleteShader(fshader_sky);


}

void Scene::Render(Matrix4f view, Matrix4f proj)
{
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < numModels; ++i)
		Models[i]->Render(view, proj, false);

	// draw skybox
	//view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
	//glDepthMask(GL_FALSE);
	//Models[numModels - 1]->Render(view, proj, true);
	//glDepthMask(GL_TRUE);


}
