#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "Scene.h"
#include "shader.h"
#include "stb-master/stb_image.h"
#include "Extras/OVR_Math.h"
#include <string>     // std::string, std::to_string


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
	for (auto it = planets.begin(); it != planets.end(); it++)
		(*it)->calculate();

	if (menu.mainstatus > 0)
	{
		calculateUI();
	}
	else if (menu.mainstatus == -1)
	{
		calculateEdit();
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


void Scene::drawHelp(Matrix4f view, Matrix4f proj)
{
	static int timer = 0;
	static int index = 0;
	timer++;

	if (timer > 200)
	{
		timer -= 200;
		index = (index + 1) % numModels;
	}
	Models[index]->Render(view, proj, false);

}


void Scene::ChangeColor()
{
	//menu.menuSphere->setColor(rand());
}


void Scene::Init(int includeIntensiveGPUobject)
{
	GLuint    vshader = CreateShader(GL_VERTEX_SHADER, "../../../Shader/normal.vs");
	GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, "../../../Shader/normal.fs");

	// Make textures
	int width, height, nrChannels;

	ShaderFill * grid_material_sun = generateShader(vshader, fshader, "../../../Src/2k_sun.jpg");
	ShaderFill * grid_material_earth = generateShader(vshader, fshader, "../../../Src/2k_earth_daymap.jpg");

	ShaderFill * grid_material_menu0 = generateShader(vshader, fshader, "../../../Src/menu0.png");
	ShaderFill * grid_material_menu1 = generateShader(vshader, fshader, "../../../Src/menu1.png");
	ShaderFill * grid_material_menu2 = generateShader(vshader, fshader, "../../../Src/menu2.png");
	ShaderFill * grid_material_menu3 = generateShader(vshader, fshader, "../../../Src/menu3.png");
	ShaderFill * grid_material_menuT = generateShader(vshader, fshader, "../../../Src/menu/texture0.png");
	ShaderFill * grid_material_pointer = generateShader(vshader, fshader, "../../../Src/pointer.png");
	ShaderFill * grid_material_slider = generateShader(vshader, fshader, "../../../Src/slider.png");
	ShaderFill *grid_material_color = generateShader(vshader, fshader, "../../../Src/color.bmp");

	ShaderFill * grid_material_Texture = generateShader(vshader, fshader, "../../../Src/2k_sun.jpg");


	Model *m;

	Planet *p;
	p = new Planet(Vector3f(0, 0, 0), grid_material_sun, 3, 2, Vector3f(0, 0, 0));
	planets.push_back(p);

	p = new Planet(Vector3f(10, 0, 0), grid_material_earth, 1, 2, Vector3f(0, 0, 10));
	planets.push_back(p);
	//editedplanet.TempPlanet = new Planet(Vector3f(10, 0, 0), grid_material_earth, 1, 2, Vector3f(0, 0, 10));
	//planets.push_back(p);

	// Init Sun
	//m = new Model(Vector3f(0, 0, 0), grid_material_sun);
	//m->AddSphere(0, 0, 0, 5, 20, 20);
	//m->AllocateBuffers();
	//Add(m);

	// Init Menu
	{
		m = new Model(Vector3f(0, 0, 0), grid_material_menu0);
		m->AddPlane(-6, -4, 0, 6, 4, 0, 5);
		m->AllocateBuffers();
		m->Pos = Vector3f(0, 0, 8);
		menu.menuModel[0] = m;

		m = new Model(Vector3f(0, 0, 0), grid_material_menu1);
		m->AddPlane(-6, -4, 0, 6, 4, 0, 5);
		m->AllocateBuffers();
		m->Pos = Vector3f(0, 0, 8);
		menu.menuModel[1] = m;

		m = new Model(Vector3f(0, 0, 0), grid_material_menu2);
		m->AddPlane(-6, -4, 0, 6, 4, 0, 5);
		m->AllocateBuffers();
		m->Pos = Vector3f(0, 0, 8);
		menu.menuModel[2] = m;

		m = new Model(Vector3f(0, 0, 0), grid_material_menu3);
		m->AddPlane(-6, -4, 0, 6, 4, 0, 5);
		m->AllocateBuffers();
		m->Pos = Vector3f(0, 0, 8);
		menu.menuModel[3] = m;

		// Texture
		m = new Model(Vector3f(0, 0, 0), grid_material_menuT);
		m->AddPlane(-0.5, -0.3, 0, 0.5, 0.3, 0, 5);
		m->AllocateBuffers();
		m->Pos = Vector3f(0.42, 0.7, 7.9);
		menu.Texture = m;

		// pointer
		m = new Model(Vector3f(0, 0, 0), grid_material_pointer);
		m->AddPlane(-0.4, -0.1, 0, 0.4, 0.1, 0, 5);
		m->AllocateBuffers();
		m->Pos = Vector3f(2.3, 0, 7.9);
		menu.pointer = m;

		// Color
		m = new Model(Vector3f(0, 0, 0), grid_material_color);
		m->AddPlane(-0.5, -0.4, 0, 0.5, 0.4, 0, 5);
		m->AllocateBuffers();
		m->Pos = Vector3f(-3.2, 0.8, 7.9);
		menu.colorChoose = m;

		// Slider
		m = new Model(Vector3f(0, 0, 0), grid_material_slider);
		m->AddPlane(-0.1, -0.2, 0, 0.1, 0.2, 0, 5);
		m->AllocateBuffers();
		m->Pos = Vector3f(-3.2, 0.8, 7.9);
		menu.Slider = m;


		// Init Model
		float r[] = { 0.8,0.9,1,1.1,1.2 };
		for (int i = 0; i < 5; i++)
		{
			m = new Model(Vector3f(0, 0, 0), grid_material_Texture);
			m->AddSphere(0, 0, 0, r[i], 20, 10);
			m->AllocateBuffers();
			m->Pos = Vector3f(3.7, 0.119, 8);
			menu.menuSphere[i] = m;
		}
	}


	// Init Help

	{
		numModels = 6;
		ShaderFill * material;
		for (int i = 0; i < numModels; i++)
		{
			std::string path = "../../../Src/Help/A" + std::to_string(i) + ".png";
			material = generateShader(vshader, fshader, path.c_str());
			m = new Model(Vector3f(0, 0, 0), material);
			m->AddPlane(-60, -40, 0, 60, 40, 0, 5);
			m->AllocateBuffers();
			m->Pos = Vector3f(0, 0, 149);
			Models[i] = m;
		}
	}


	glDeleteShader(vshader);
	glDeleteShader(fshader);

	// Init skybox 
	{
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
			m_sky->AddPlane(-150.0f, -150.0f, -150.0f, 150.0f, 150.0f, 150.0f, i);
			m_sky->AllocateBuffers();
			AddSky(m_sky);
		}
		glDeleteShader(vshader_sky);
		glDeleteShader(fshader_sky);

	}


	// 为了同步
	ChangeTexture();

}


void Scene::Render(Matrix4f view, Matrix4f proj, Vector3f pos)
{
	for (auto it = planets.begin(); it != planets.end(); it++)
		(*it)->Render(view, proj, pos);

	// 下面内容的显示需要无视位移

	Matrix4f viewnew(view.M[0][0], view.M[0][1], view.M[0][2], view.M[1][0], view.M[1][1], view.M[1][2], view.M[2][0], view.M[2][1], view.M[2][2]);

	if (menu.mainstatus > 0)
		drawMenu(viewnew, proj);
	else if (menu.mainstatus == -1)
	{
		drawEdit(view, proj, pos);
	}
	// draw skybox
	for (int i = 0; i < numSkyModels; ++i)
		SkyBoxModels[i]->Render(viewnew, proj, false);

	drawHelp(viewnew, proj);

}


