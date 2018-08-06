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
DWORD Color[] = { 0xFFFFFF,0xFF0000,0xFFFF00,0x00FF00,0x00FFFF };

Vector3f PointPos[] = {
	Vector3f(0, 0, settedZ), Vector3f(0.4, 1.4, settedZ), Vector3f(0.4, -0.377, settedZ),Vector3f(0, 0, settedZ),
	Vector3f(-2.6, 1.4, settedZ),Vector3f(-2.6, -0.377, settedZ),Vector3f(-2.6, -2.04, settedZ) };




//void Scene::EditingPlanet()
//{
//}



// ================================
// Main UI event
// ================================
void Scene::ChangeTexture()
{
	{
		int width, height, nrChannels;
		char *load;
		while (menu.textureType < 0)
		{
			menu.textureType += 6;
		}
		menu.textureType %= 6;

		switch (menu.textureType)
		{
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
		unsigned char *data;
		if (menu.textureType > 1)
		{
			data = stbi_load(load, &width, &height, &nrChannels, 0);
		}
		else if (menu.textureType == 0)
		{
			data = (unsigned char *)Texture::generateTecture(200, 200, (Color[menu.color]), true);
		}
		else
		{
			data = (unsigned char *)Texture::generateTecture(200, 200, (Color[menu.color]), true);
		}
		TextureBuffer * generated_texture = new TextureBuffer(true, Sizei(width, height), 4, (unsigned char *)data, nrChannels);
		menu.menuSphere->Fill->changTecture(generated_texture);
	}

	{
		int width, height, nrChannels;
		char *load;
		switch (menu.textureType)
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
		else
		{
			assert(!"should not get there");
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
			//EditingPlanet();

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
				menu.color = (menu.color++) % 5;
				break;
			case 5:
				menu.segment = (menu.segment++) % 5;
				break;
			case 6:
				menu.segment = (menu.segment++) % 5;
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
				menu.speed = (menu.speed++) % 5;
				break;
			case 5:
				menu.size = (menu.size++) % 5;
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
		switch (menu.pointerstatus)
		{
		case 1:
			switch (menu.pointerstatus)
			{
				menu.textureType = menu.textureType--;
				break;
			case 4:
				menu.color = (menu.color--) % 5;
				break;
			case 5:
				menu.segment = (menu.segment--) % 5;
				break;
			case 6:
				menu.segment = (menu.segment--) % 5;
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
				menu.speed = (menu.speed--) % 5;
				break;
			case 5:
				menu.size = (menu.size--) % 5;
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
		//switch (i)
		//{
		//case 0: // J
		//	editedplanet.distance += 1;
		//	break;
		//case 1:
		//	editedplanet.distance -= 1;
		//	break;
		//case 2: // J
		//	editedplanet.vector += 1;
		//	break;
		//case 3:
		//	editedplanet.vector -= 1;
		//	break;
		//case 4:case 5:
		//{
		//	Vector3f vec;
		//	vec = editedplanet.TempPlanet->Pos.Cross(Vector3f(0, 1, 0));
		//	editedplanet.TempPlanet->Velocity = vec;
		//	planets.push_back(editedplanet.TempPlanet);
		//	menu.mainstatus = 0;
		//}
		//break;
		//default:
		//	break;
		//}
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
	menu.menuSphere->Render(viewnew, proj, false);
	if (menu.pointerstatus != 0 && menu.pointerstatus != 3)
		menu.pointer->Render(viewnew, proj, false);

	menu.Texture->Render(viewnew, proj, false);
	menu.colorChoose->Render(viewnew, proj, false);
}

//void Scene::recalculateEdit()
//{
//
//}
//
//void Scene::drawEdit(Matrix4f view, Matrix4f proj, Vector3f pos_t)
//{
//	pos_t.Normalize();
//	editedplanet.TempPlanet->Pos = pos_t * editedplanet.distance;
//	editedplanet.TempPlanet->Render(view, proj, pos_t);
//}
//
