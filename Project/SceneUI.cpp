#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "Scene.h"
#include "shader.h"
#include "stb-master/stb_image.h"
#include "Extras/OVR_Math.h"
#include "Texture.h"
#include <string>     // std::string, std::to_string

const float settedZ = 7.9;
DWORD Color[] = { 0xFFFFFF,0xedd36a,0x5c9459,0x5691b7,0xac2719};

int SEGMENT[2][5] = { { 8,10,12,15,20}, {3,5,8,10,12} };
float SLIDERX[5] = { -1.9,-2.5,-3.1,-3.7,-4.3 };
float SLIDERY[3] = { 0.8,-0.9,-2.4 };
float SPEED[5] = { 0.5,1,1.5,2,3 };
float RADIUS[5] = { 0.8,0.9,1,1.1,1.2 };

Vector3f PointPos[] = {
	Vector3f(0, 0, settedZ), Vector3f(0.4, 1.35, settedZ), Vector3f(0.4, -0.377, settedZ),Vector3f(0, 0, settedZ),
	Vector3f(-2.9, 1.4, settedZ),Vector3f(-2.9, -0.377, settedZ),Vector3f(-2.9, -2.04, settedZ) };



void Scene::calculateUI()
{
	static float clock = 0;
	clock += SPEED[menu.speed] * 0.004;
	menu.menuSphere[menu.size]->Rot = Quatf(Axis::Axis_Y, clock);

}

void Scene::calculateEdit()
{
	editedplanet.TempPlanet->calculate();
}

// ================================
// Main UI event
// ================================
void Scene::ChangeTexture()
{
	// 设置总的Texture
	{
		int width, height, nrChannels;
		char *load;
		while (menu.textureType < 0)
		{
			menu.textureType += 15;
		}
		menu.textureType %= 15;

		switch (menu.textureType)
		{
		case 2:
			load = "../../../Src/2k_earth_daymap.jpg"; break;
		case 3:
			load = "../../../Src/2k_mercury.jpg"; break;
		case 4:
			load = "../../../Src/2k_venus_surface.jpg"; break;
		case 5:
			load = "../../../Src/2k_venus_atmosphere.jpg"; break;
		case 6:
			load = "../../../Src/2k_mars.jpg"; break;
		case 7:
			load = "../../../Src/2k_jupiter.jpg"; break;
		case 8:
			load = "../../../Src/2k_saturn.jpg"; break;
		case 9:
			load = "../../../Src/2k_uranus.jpg"; break;
		case 10:
			load = "../../../Src/2k_neptune.jpg"; break;
		case 11:
			load = "../../../Src/2k_ceres_fictional.jpg"; break;
		case 12:
			load = "../../../Src/2k_haumea_fictional.jpg"; break;
		case 13:
			load = "../../../Src/2k_makemake_fictional.jpg"; break;
		case 14:
			load = "../../../Src/2k_eris_fictional.jpg"; break;
		default:
			break;
		}
		unsigned char *data;
		if (menu.textureType > 1)
		{
			data = stbi_load(load, &width, &height, &nrChannels, 0);
			TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
			menu.menuSphere[menu.size]->Fill->changTecture(generated_texture);
		}
		else if (menu.textureType == 0)
		{
			data = (unsigned char *)Texture::generateTecture(200, 200, (Color[menu.color]), false, SEGMENT[0][menu.segment], menu.seed);
			TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(200, 200), 4, (unsigned char *)data, 3);
			menu.menuSphere[menu.size]->Fill->changTecture(generated_texture);


			// 不知道为什么这里直接读纯色的会出错 后来改成了1 * 1
			//data = stbi_load("../../../test.bmp", &width, &height, &nrChannels, 0);
			//generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
			//menu.colorChoose->Fill->changTecture(generated_texture);

			data = Texture::generateTecturePure(1, 1, (Color[menu.color]));
			generated_texture = new TextureBuffer(true, Sizei(1, 1), 4, data, 3);
			menu.colorChoose->Fill->changTecture(generated_texture);

		}
		else
		{
			data = (unsigned char *)Texture::generateTecture(200, 200, (Color[menu.color]), true, SEGMENT[1][menu.segment], menu.seed);
			TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(200, 200), 4, (unsigned char *)data, 3);
			menu.menuSphere[menu.size]->Fill->changTecture(generated_texture);

			data = Texture::generateTecturePure(1, 1, (Color[menu.color]));
			generated_texture = new TextureBuffer(true, Sizei(1, 1), 4, data, 3);
			menu.colorChoose->Fill->changTecture(generated_texture);

		}
	}

	// 设置Texture对应的显示
	{
		int width, height, nrChannels;
		std::string path = "../../../Src/menu/texture" + std::to_string(menu.textureType) + ".png";
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
		menu.Texture->Fill->changTecture(generated_texture);
	}
}

void Scene::ChangeTextureToNew()
{
	// 设置总的Texture
	{
		int width, height, nrChannels;
		char *load;
		while (menu.textureType < 0)
		{
			menu.textureType += 15;
		}
		menu.textureType %= 15;

		switch (menu.textureType)
		{
		case 2:
			load = "../../../Src/2k_earth_daymap.jpg"; break;
		case 3:
			load = "../../../Src/2k_mercury.jpg"; break;
		case 4:
			load = "../../../Src/2k_venus_surface.jpg"; break;
		case 5:
			load = "../../../Src/2k_venus_atmosphere.jpg"; break;
		case 6:
			load = "../../../Src/2k_mars.jpg"; break;
		case 7:
			load = "../../../Src/2k_jupiter.jpg"; break;
		case 8:
			load = "../../../Src/2k_saturn.jpg"; break;
		case 9:
			load = "../../../Src/2k_uranus.jpg"; break;
		case 10:
			load = "../../../Src/2k_neptune.jpg"; break;
		case 11:
			load = "../../../Src/2k_ceres_fictional.jpg"; break;
		case 12:
			load = "../../../Src/2k_haumea_fictional.jpg"; break;
		case 13:
			load = "../../../Src/2k_makemake_fictional.jpg"; break;
		case 14:
			load = "../../../Src/2k_eris_fictional.jpg"; break;
		default:
			break;
		}
		unsigned char *data;
		if (menu.textureType > 1)
		{
			data = stbi_load(load, &width, &height, &nrChannels, 0);
			TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
			menu.menuSphere[menu.size]->Fill->texture = generated_texture;
		}
		else if (menu.textureType == 0)
		{
			data = (unsigned char *)Texture::generateTecture(200, 200, (Color[menu.color]), false, SEGMENT[0][menu.segment], menu.seed);
			TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(200, 200), 4, (unsigned char *)data, 3);
			menu.menuSphere[menu.size]->Fill->texture = generated_texture;
		}
	}

	// 设置Texture对应的显示
	{
		int width, height, nrChannels;
		std::string path = "../../../Src/menu/texture" + std::to_string(menu.textureType) + ".png";
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
		menu.Texture->Fill->changTecture(generated_texture);
	}
}


//Vector3f SliderPos[3][3] = {
//	Vector3f(3, 2, settedZ),Vector3f(3, 2, settedZ),Vector3f(3, 2, settedZ),
//	Vector3f(3, 0, settedZ) ,Vector3f(3, 0, settedZ) ,Vector3f(3, 0, settedZ),
//	Vector3f(3, -2, settedZ) ,Vector3f(3, -2, settedZ),Vector3f(3, -2, settedZ)
//};

void Scene::menuEvent(int i)
{

	switch (i)
	{
	case 0: // J
			// 1 2 3
		if (menu.pointerstatus > 0 && menu.pointerstatus <= 3)
		{
			menu.pointerstatus = 0;
			menu.mainstatus = 0;
		}
		else if (menu.pointerstatus > 3)
		{
			menu.pointerstatus = menu.mainstatus;
		}

		break;

	case 1: // L
			// 0
		if (0 == menu.pointerstatus)
		{
			menu.pointerstatus = 1;
			menu.mainstatus = 1;
		}
		else if (menu.pointerstatus == 1)
		{
			if (0 == menu.textureType || 1 == menu.textureType)
				menu.pointerstatus = 4;
		}
		else if (menu.pointerstatus == 2)
			menu.pointerstatus = 4;
		else if (menu.pointerstatus == 3)
		{
			menu.mainstatus = -1;
			menu.pointerstatus = 0;
			EditingInit();
		}
		break;
	case 2: // I
			// 1 2 3
		if (menu.pointerstatus > 0 && menu.pointerstatus <= 3)
		{
			menu.pointerstatus++;
			if (menu.pointerstatus == 4)
				menu.pointerstatus = 1;
			menu.mainstatus = menu.pointerstatus;
		}
		else if (menu.pointerstatus > 3)
		{
			menu.pointerstatus++;
			if (menu.mainstatus == 1 && menu.pointerstatus == 7)
			{
				menu.pointerstatus = 4;
			}
			if (menu.mainstatus == 2 && menu.pointerstatus == 6)
			{
				menu.pointerstatus = 4;
			}
		}
		break;
	case 3: //K
			// 1 2 3

		if (menu.pointerstatus > 0 && menu.pointerstatus <= 3)
		{
			menu.pointerstatus--;
			if (menu.pointerstatus == 0)
				menu.pointerstatus = 3;
			menu.mainstatus = menu.pointerstatus;
		}
		else if (menu.pointerstatus > 3)
		{
			menu.pointerstatus--;
			if (menu.pointerstatus == 3)
			{
				if (menu.mainstatus == 1)
					menu.pointerstatus = 6;
				if (menu.mainstatus == 2)
					menu.pointerstatus = 5;
			}
		}
		break;
	case 4: // U
		switch (menu.mainstatus)
		{
		case 1:
			switch (menu.pointerstatus)
			{
			case 1:
				menu.textureType = menu.textureType++;
				break;
			case 4:
				menu.color = (menu.color + 1) % 5;
				break;
			case 5:
				menu.segment++;
				if (menu.segment > 4)
					menu.segment = 4;
				break;
			case 6:
				menu.seed++;
				if (menu.seed > 4)
					menu.seed = 4;

				break;
			default:
				break;
			}
			ChangeTexture();
			break;
		case 2:
			switch (menu.pointerstatus)
			{
			case 4:
				menu.speed++;
				if (menu.speed > 4)
				{
					menu.speed = 4;
				}
				break;
			case 5:
				menu.size++;
				if (menu.size > 4)
				{
					menu.size = 4;
				}

				break;
			default:
				break;
			}
			ChangeTexture();
			break;
		default:
			break;
		}
		break;
	case 5: // O
		switch (menu.mainstatus)
		{
		case 1:
			switch (menu.pointerstatus)
			{
			case 1:
				menu.textureType = menu.textureType--;
				break;
			case 4:
				menu.color = (menu.color + 4) % 5;
				break;
			case 5:
				menu.segment--;
				if (menu.segment < 0)
					menu.segment = 0;
				break;
			case 6:
				menu.seed--;
				if (menu.seed < 0)
					menu.seed = 0;

				break;
			default:
				break;
			}
			ChangeTexture();
			break;
		case 2:
			switch (menu.pointerstatus)
			{
			case 4:
				menu.speed--;
				if (menu.speed < 0)
				{
					menu.speed = 0;
				}
				break;

				break;
			case 5:
				menu.size--;
				if (menu.size < 0)
				{
					menu.size = 0;
				}
				break;
			default:
				break;
			}
			ChangeTexture();
			break;
		default:
			break;
		}		break;
	default:
		break;
	}
	menu.pointer->Pos = PointPos[menu.pointerstatus];

}



//J = 0 L K I
void Scene::Event(int i)
{
	if (menu.mainstatus != -1)
	{
		menuEvent(i);
	}
	else
	{
		switch (i)
		{
		case 0: // J
			editedplanet.distance += 0.015;
			if (editedplanet.distance > 15)
				editedplanet.distance = 15;
			break;
		case 1:
			editedplanet.distance -= 0.015;
			if (editedplanet.distance < 5)
				editedplanet.distance = 5;
			break;
		case 2: // J
			editedplanet.vector += 0.015;
			if (editedplanet.vector > 5)
				editedplanet.vector = 5;
			break;
		case 3:
			editedplanet.vector -= 0.015;
			if (editedplanet.vector < 1)
				editedplanet.vector = 1;
			break;
		case 4:case 5:
		{
			Vector3f vec;
			vec = editedplanet.TempPlanet->Pos.Cross(Vector3f(0, 1, 0));
			editedplanet.TempPlanet->Velocity = vec;
			planets.push_back(editedplanet.TempPlanet);
			menu.mainstatus = 0;
		}
		break;
		default:
			break;
		}
		//recalculateEdit();
	}

}




void Scene::drawMenu(Matrix4f viewnew, Matrix4f proj)
{
	// draw menu
	switch (menu.mainstatus)
	{
	case 0:
		return; break;
	case 1:
		if (0 == menu.textureType || 1 == menu.textureType)
		{
			menu.menuModel[0]->Render(viewnew, proj, false);
		}
		else
		{
			menu.menuModel[1]->Render(viewnew, proj, false);
		}
		break;
	case 2:
		menu.menuModel[2]->Render(viewnew, proj, false);
		break;
	case 3:
		menu.menuModel[3]->Render(viewnew, proj, false);
		break;
	default:
		break;
	}

	menu.menuSphere[menu.size]->Render(viewnew, proj, false);
	if (menu.pointerstatus != 0 && menu.pointerstatus != 3)
		menu.pointer->Render(viewnew, proj, false);

	menu.Texture->Render(viewnew, proj, false);
	if (menu.mainstatus == 1 && menu.textureType < 2)
	{
		menu.colorChoose->Render(viewnew, proj, false);
		menu.Slider->Pos = Vector3f(SLIDERX[menu.segment], SLIDERY[1], settedZ);
		menu.Slider->Render(viewnew, proj, false);
		menu.Slider->Pos = Vector3f(SLIDERX[menu.seed], SLIDERY[2], settedZ);
		menu.Slider->Render(viewnew, proj, false);
	}
	else if (menu.mainstatus == 2)
	{
		menu.Slider->Pos = Vector3f(SLIDERX[menu.speed], SLIDERY[0], settedZ);
		menu.Slider->Render(viewnew, proj, false);
		menu.Slider->Pos = Vector3f(SLIDERX[menu.size], SLIDERY[1], settedZ);
		menu.Slider->Render(viewnew, proj, false);

	}
}




void Scene::EditingInit()
{
	editedplanet.TempPlanet = new Planet(Vector3f(10, 0, 0), new ShaderFill(vshadert, fshadert, menu.menuSphere[0]->Fill->texture), RADIUS[menu.size], SPEED[menu.speed], Vector3f(0, 0, 0));
	ChangeTextureToNew();
	editedplanet.vector = 3;
	editedplanet.distance = 8;
}


void Scene::recalculateEdit()
{

}

void Scene::drawEdit(Matrix4f view, Matrix4f proj, Vector3f pos_m, Matrix4f viewfollow)
{

	Vector3f pos_t = pos_m;
	editedplanet.TempPlanet->Render(view, proj, pos_t);
	pos_t.Normalize();
	Vector3f temp = pos_t.Cross(Vector3f(0, 1, 0));
	temp.Normalize();
	Vector3f des = temp + pos_t;
	editedplanet.TempPlanet->Pos = des * editedplanet.distance;


	Model *m;
	m = new Model(Vector3f(0, 0, 0), material_line);
	Vector3f temp2 = (pos_m - editedplanet.TempPlanet->Pos).Cross(Vector3f(0, 1, 0));
	temp2.Normalize();
	Vector3f temp1 = temp2*editedplanet.vector/2;
	Vector3f ver[4] = { -temp1 + Vector3f(0,0.2,0),temp1 + Vector3f(0,0.2,0),-temp1 - Vector3f(0,0.2,0),temp1 - Vector3f(0,0.2,0) };
	m->AddNormalPlane(ver);
	m->AllocateBuffers();
	m->Pos = Vector3f((editedplanet.TempPlanet->Pos).x, -RADIUS[menu.size] - 1, (editedplanet.TempPlanet->Pos).z);
	m->Render(view, proj, false);
	delete m;
}

