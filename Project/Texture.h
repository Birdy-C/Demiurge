#pragma once
#include "stb-master\stb_perlin.h"
#include "stb-master\stb_image_write.h"
class Texture
{
public:
	Texture() {}
	static char* generateTecture(int width, int height)
	{
		char *texture = new char[width*height];
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				float noise = stb_perlin_noise3(float(i) / width * 10, float(j) / height * 10, 0, 0, 0, 0) * 128 + 128;
				texture[i*width + j] = noise;
			}
		}
		stbi_write_bmp("../../../test.bmp", width, height, 1, texture);
		return texture;
	}
};