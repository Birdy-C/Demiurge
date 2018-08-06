#pragma once
#include "Win32_GLAppUtil.h"
#include "shader.h"
#include <iostream>
#include <vector>
struct Scene
{
	struct Menu
	{
		//记录目前选中位置
		// 0 不开启 1 选择纹理的type 2 选择type对应的具体数值 3 继续 -1 正在编辑位置和速度等
		int mainstatus = 0;
		// 0 不开启 1  2 3  4  5 6
		int pointerstatus = 0;

		// 记录各种参数
		int textureType = 0;

		int color = 0;		// 5
		int segment = 0;	// 5
		int seed = 0;		// 5

		int speed = 1;		// 5
		int size = 1;		//5


		Model *menuModel[4];	// menu 的纹理
		Model *menuSphere[5];		// 显示球
		Model *pointer;			// 显示指针
		Model *Texture;			// 显示Texture
		Model *Slider;		// 两个滑条

		Model *colorChoose;


		~Menu()
		{
			for (int i = 0; i < 4; i++)
				delete menuModel[i];

			delete menuSphere;
			delete pointer;
			delete Texture;
			delete Slider;
		}
	} menu;


	int     numModels;
	Model * Models[10]; // 记录help的动画
	int numSkyModels;
	Model * SkyBoxModels[6];
	std::vector<Planet *> planets;
	void calculateUI();


	//struct EditedPlanet
	//{
	//	Planet *TempPlanet;
	//	Model *arrow;
	//	Model *line;
	//	float distance;
	//	float vector;

	//	EditedPlanet()
	//	{
	//		delete TempPlanet;
	//		delete arrow;
	//		delete line;
	//	}

	//} editedplanet;

	//void EditingPlanet();
	//void recalculateEdit();



	void    Add(Model * n)
	{
		Models[numModels++] = n;
	}

	void    AddSky(Model * n)
	{
		SkyBoxModels[numSkyModels++] = n;
	}

	void ChangeTexture();
	void ChangeColor();
	void drawMenu(Matrix4f view, Matrix4f proj);
	//void drawEdit(Matrix4f view, Matrix4f proj, Vector3f pos);
	void Render(Matrix4f view, Matrix4f proj, Vector3f pos);
	void drawHelp(Matrix4f view, Matrix4f proj);

	GLuint CreateShader(GLenum type, const char* path);

	void Calculate();
	void Event(int i);
	void menuEvent(int i);
	ShaderFill * generateShader(GLuint vertexShader, GLuint pixelShader, const char * path);
	//ShaderFill * generateShader(GLuint vertexShader, GLuint pixelShader, unsigned char * path);
	//ShaderFill * generateShader();
	void Init(int includeIntensiveGPUobject);

	Scene() : numModels(0), numSkyModels(0) {}
	Scene(bool includeIntensiveGPUobject) :
		numModels(0), numSkyModels(0)
	{
		Init(includeIntensiveGPUobject);
	}
	void Release()
	{
		while (numModels-- > 0)
			delete Models[numModels];

		for (int i = 0; i < 6; i++)
		{
			delete SkyBoxModels[i];
		}

		//glDeleteVertexArrays(1, &SkyBox.skyboxVAO);
		for (auto it = planets.begin(); it != planets.end(); it++)
			delete *it;
	}
	~Scene()
	{
		Release();
	}
};
