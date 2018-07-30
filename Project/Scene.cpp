#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "Scene.h"
#include "shader.h"
#include "stb-master/stb_image.h"
#include "Extras/OVR_Math.h"

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


void Scene::Calculate()
{
	static float cubeClock = 0;
	cubeClock += 0.015f;
	// roomScene->Models[0]->Pos = Vector3f(9 * (float)sin(cubeClock), 3, 9 * (float)cos(cubeClock));
	// TODO change position and recalculate
	menu.menuSphere->Rot = Quatf(Axis::Axis_Y, cubeClock);


}

void Scene::Init(int includeIntensiveGPUobject)
{
	GLuint    vshader = CreateShader(GL_VERTEX_SHADER, "../../../Shader/normal.vs");
	GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, "../../../Shader/normal.fs");

	// Make textures
	int width, height, nrChannels;
	unsigned char *data = stbi_load("../../../Src/2k_sun.jpg", &width, &height, &nrChannels, 0);
	TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
	ShaderFill * grid_material_sun = new ShaderFill(vshader, fshader, generated_texture);

	unsigned char *datamenu = stbi_load("../../../Src/menu.png", &width, &height, &nrChannels, 0);
	TextureBuffer * generated_texture_menu = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)datamenu, nrChannels);
	ShaderFill * grid_material_menu = new ShaderFill(vshader, fshader, generated_texture_menu);


	glDeleteShader(vshader);
	glDeleteShader(fshader);

	Model *m;
	// Init Sun
	m = new Model(Vector3f(0, 0, 0), grid_material_sun);
	m->AddSphere(0, 0, 0, 5, 10, 10);
	m->AllocateBuffers();
	Add(m);

	// Init Menu
	m = new Model(Vector3f(0, 0, 0), grid_material_menu);
	m->AddPlane(-4, -3, 0, 4, 3, 0, 5);

	//m->AddSphere(0, 0, 0, 1, 10, 10);
	m->AllocateBuffers();
	m->Pos = Vector3f(0, 0, 8);
	menu.menuModel = m;

	m = new Model(Vector3f(0, 0, 0), grid_material_sun);
	m->AddSphere(0, 0, 0, 1, 10, 10);
	m->AllocateBuffers();
	m->Pos = Vector3f(2.3, 0, 8);
	menu.menuSphere = m;

	// ===================================================
	// Init skybox 
	// ===================================================
	GLuint    vshader_sky = CreateShader(GL_VERTEX_SHADER, "../../../Shader/skybox.vs");
	GLuint    fshader_sky = CreateShader(GL_FRAGMENT_SHADER, "../../../Shader/skybox.fs");

	std::vector<std::string> faces
	{
		"../../../Src/ame_starfield/starfield_up.tga",
		"../../../Src/ame_starfield/starfield_dn.tga",
		"../../../Src/ame_starfield/starfield_ft.tga",
		"../../../Src/ame_starfield/starfield_bk.tga",
		"../../../Src/ame_starfield/starfield_lf.tga",
		"../../../Src/ame_starfield/starfield_rt.tga",
	};

	ShaderFill *grid_materialsky[6];
	TextureBuffer * generated_texturesky;
	for (int i = 0; i < 6; i++)
	{
		data = stbi_load(faces.at(i).c_str(), &width, &height, &nrChannels, 0);
		generated_texturesky = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
		grid_materialsky[i] = new ShaderFill(vshader, fshader, generated_texturesky);

		Model *m_sky = new Model(Vector3f(0, 0, 0), grid_materialsky[i]);
		m_sky->AddPlane(-100.0f, -100.0f, -100.0f, 100.0f, 100.0f, 100.0f, i);
		m_sky->AllocateBuffers();
		AddSky(m_sky);
	}

	glDeleteShader(vshader_sky);
	glDeleteShader(fshader_sky);

}


void Scene::Render(Matrix4f view, Matrix4f proj)
{
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//����S�����췽ʽΪ�ظ�
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//����T�����췽ʽΪ�ظ�

	for (int i = 0; i < numModels; ++i)
		Models[i]->Render(view, proj, false);


	// draw skybox
	// �������ݵ���ʾ��Ҫ����λ��

	Matrix4f viewnew(view.M[0][0], view.M[0][1], view.M[0][2], view.M[1][0], view.M[1][1], view.M[1][2], view.M[2][0], view.M[2][1], view.M[2][2]);

	menu.menuSphere->Render(viewnew, proj, false);
	menu.menuModel->Render(viewnew, proj, false);
	for (int i = 0; i < numSkyModels; ++i)
		SkyBoxModels[i]->Render(viewnew, proj, false);
}


void Scene::ChangeShader(int i)
{
	int width, height, nrChannels;
	char *load;
	switch (i % 6)
	{
	case 0:
		load = "../../../Src/2k_sun.jpg"; break;
	case 1:
		load = "../../../Src/2k_earth_daymap.jpg"; break;
	case 2:
		load = "../../../Src/2k_eris_fictional.jpg"; break;
	case 3:
		load = "../../../Src/2k_haumea_fictional.jpg"; break;
	case 4:
		load = "../../../Src/2k_jupiter.jpg"; break;
	case 5:
		load = "../../../Src/2k_mars.jpg"; break;
	default:
		break;
	}
	unsigned char *data = stbi_load(load, &width, &height, &nrChannels, 0);
	TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
	menu.menuSphere->Fill->changTecture(generated_texture);
}


void Scene::ChangeColor()
{
	menu.menuSphere->setColor(rand());
}

