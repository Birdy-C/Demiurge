#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "Scene.h"
#include "shader.h"
#include "stb-master/stb_image.h"
#include "Extras/OVR_Math.h"

float settedZ = 7.9;
Vector3f PointPos[] = { Vector3f(0, 0, settedZ), Vector3f(0, 2, settedZ), Vector3f(0, 0, settedZ), Vector3f(0, -2, settedZ) };
DWORD Color[] = { 0xFFFFFF,0xFF0000,0xFFFF00,0x00FF00,0x00FFFF };

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

//J = 0 L K I
void Scene::Event(int i)
{
	switch (i)
	{
	case 0:
		// 1 2 3
		if (menu.status > 0 && menu.status <= 3)
		{
			menu.status = 0;
		}
		break;
	case 1:
		// 0
		if (0 == menu.status)
		{
			menu.status = 1;
			menu.pointer->Pos = PointPos[menu.status];
		}
		break;
	case 2:
		// 1 2 3
		if (menu.status > 0 && menu.status <= 3)
		{
			menu.status++;
			if (menu.status == 4)
				menu.status = 1;
			menu.pointer->Pos = PointPos[menu.status];
		}
		break;
	case 3:
		// 1 2 3
		if (menu.status > 0 && menu.status <= 3)
		{
			menu.status--;
			if (menu.status == 0)
				menu.status = 3;
			menu.pointer->Pos = PointPos[menu.status];
		}
		break;
	case 4:
		if (1 == menu.status)
		{
			menu.textureType++;
			ChangeShader();
		}
		break;
	case 5:
		if (1 == menu.status)
		{
			menu.textureType--;
			ChangeShader();
		}
		break;
	default:
		break;
	}
}

ShaderFill * Scene::generateShader(GLuint vertexShader, GLuint pixelShader, const char * path)
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
	ShaderFill * grid_material = new ShaderFill(vertexShader, pixelShader, generated_texture);
	return grid_material;
}

void Scene::Init(int includeIntensiveGPUobject)
{
	GLuint    vshader = CreateShader(GL_VERTEX_SHADER, "../../../Shader/normal.vs");
	GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, "../../../Shader/normal.fs");

	// 我绝对要给写这个的差评……
	// Make textures
	int width, height, nrChannels;

	ShaderFill * grid_material_sun = generateShader(vshader, fshader, "../../../Src/2k_uranus.jpg");
	ShaderFill * grid_material_menu0 = generateShader(vshader, fshader, "../../../Src/menu0.png");
	ShaderFill * grid_material_menu1 = generateShader(vshader, fshader, "../../../Src/menu1.png");
	ShaderFill * grid_material_menu2 = generateShader(vshader, fshader, "../../../Src/menu2.png");
	ShaderFill * grid_material_menu3 = generateShader(vshader, fshader, "../../../Src/menu3.png");
	ShaderFill * grid_material_menu4 = generateShader(vshader, fshader, "../../../Src/menu4.png");
	ShaderFill * grid_material_menuT = generateShader(vshader, fshader, "../../../Src/menu/texture0.png");

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	Model *m;
	// Init Sun
	m = new Model(Vector3f(0, 0, 0), grid_material_sun);
	m->AddSphere(0, 0, 0, 5, 20, 20);
	m->AllocateBuffers();
	Add(m);

	// Init Menu
	m = new Model(Vector3f(0, 0, 0), grid_material_menu0);
	m->AddPlane(-4, -3, 0, 4, 3, 0, 5);
	m->AllocateBuffers();
	m->Pos = Vector3f(0, 0, 8);
	menu.menuModel[0] = m;

	m = new Model(Vector3f(0, 0, 0), grid_material_menu1);
	m->AddPlane(-4, -3, 0, 4, 3, 0, 5);
	m->AllocateBuffers();
	m->Pos = Vector3f(0, 0, 8);
	menu.menuModel[1] = m;

	m = new Model(Vector3f(0, 0, 0), grid_material_menuT);
	m->AddPlane(-0.4, -0.3, 0, 0.4, 0.3, 0, 5);
	m->AllocateBuffers();
	m->Pos = Vector3f(0, 0.5, 7.9);
	menu.Texture = m;


	m = new Model(Vector3f(0, 0, 0), grid_material_sun);
	m->AddSphere(0, 0, 0, 0.5, 10, 10);
	m->AllocateBuffers();
	m->Pos = Vector3f(2.3, 0, 8);
	menu.pointer = m;


	// Init Model

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

		grid_materialsky[i] = generateShader(vshader, fshader, faces.at(i).c_str());

		Model *m_sky = new Model(Vector3f(0, 0, 0), grid_materialsky[i]);
		m_sky->AddPlane(-100.0f, -100.0f, -100.0f, 100.0f, 100.0f, 100.0f, i);
		m_sky->AllocateBuffers();
		AddSky(m_sky);
	}

	glDeleteShader(vshader_sky);
	glDeleteShader(fshader_sky);

}

void Scene::drawMenu(Matrix4f viewnew, Matrix4f proj)
{
	// draw menu
	if (0 == menu.status)
	{
		menu.menuModel[0]->Render(viewnew, proj, false);
	}
	else if (menu.status <= 3)
	{
		menu.menuSphere->Render(viewnew, proj, false);
		menu.menuModel[1]->Render(viewnew, proj, false);
		menu.pointer->Render(viewnew, proj, false);
		menu.Texture->Render(viewnew, proj, false);
	}
}

void Scene::Render(Matrix4f view, Matrix4f proj)
{
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//设置S轴拉伸方式为重复
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//设置T轴拉伸方式为重复

	for (int i = 0; i < numModels; ++i)
		Models[i]->Render(view, proj, false);


	//// 下面内容的显示需要无视位移

	Matrix4f viewnew(view.M[0][0], view.M[0][1], view.M[0][2], view.M[1][0], view.M[1][1], view.M[1][2], view.M[2][0], view.M[2][1], view.M[2][2]);

	//drawMenu(viewnew, proj);

	// draw skybox
	for (int i = 0; i < numSkyModels; ++i)
		SkyBoxModels[i]->Render(viewnew, proj, false);
}




void Scene::ChangeShader()
{
	{
		int width, height, nrChannels;
		char *load;
		while (menu.textureType < 0)
		{
			menu.textureType += 6;
		}
		switch (menu.textureType % 6)
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

	{
		int width, height, nrChannels;
		char *load;
		switch (menu.textureType % 6)
		{
		case 0:
			load = "../../../Src/menu/texture0.png"; break;
		case 1:
			load = "../../../Src/menu/texture1.png"; break;
		case 2:
			load = "../../../Src/menu/texture2.png"; break;
		case 3:
			load = "../../../Src/menu/texture3.png"; break;
		case 4:
			load = "../../../Src/menu/texture4.png"; break;
		case 5:
			load = "../../../Src/menu/texture5.png"; break;
		default:
			break;
		}
		unsigned char *data = stbi_load(load, &width, &height, &nrChannels, 0);
		TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
		menu.Texture->Fill->changTecture(generated_texture);
	}
}


void Scene::ChangeColor()
{
	menu.menuSphere->setColor(rand());
}

