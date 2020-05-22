#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "CCubeMap.h"

// загрузка текстуры из внешнего файла
void CCubeMap::Load(int Index)
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &TexIndex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TexIndex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// пути до изображений
	const char* Path[5] =
	{
		"Textures\\skybox\\CloudyCrown_01_Midday\\CloudyCrown_Midday_",
		"Textures\\skybox\\CloudyCrown_02_Evening\\CloudyCrown_Evening_",
		"Textures\\skybox\\CloudyCrown_03_Midnight\\CloudyCrown_Midnight_",
		"Textures\\skybox\\CloudyCrown_04_Sunset\\CloudyCrown_Sunset_",
		"Textures\\skybox\\CloudyCrown_05_Daybreak\\CloudyCrown_Daybreak_"
	};
	// грани кубической текстуры
	const char* Face[6] = { "Front", "Back", "Up", "Down", "Right", "Left" };
	// строка для имени файла
	char Name[255];
	// загрузка всех граней кубической текстуры
	for (int i = 0; i < 6; i++)
	{
		// генерация имени файла
		memset(Name, char(0), 255);
		strcpy(Name, Path[Index - 1]);
		strcat(Name, Face[i]);
		strcat(Name, ".png");
		// загрузка изображения
		int Image = ilGenImage();
		ilBindImage(Image);
		bool Result = ilLoadImage(Name);
		if (!Result) printf("%s%s\n\n", Name, " not loaded");
		int width = ilGetInteger(IL_IMAGE_WIDTH);
		int height = ilGetInteger(IL_IMAGE_HEIGHT);
		ILubyte *Data = new ILubyte[width * height * 4];
		ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, Data);
		GLenum mode;
		switch (i)
		{
			case 0: mode = GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
			case 1: mode = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
			case 2: mode = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
			case 3: mode = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
			case 4: mode = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
			case 5: mode = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
		}
		glTexImage2D(mode, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
		delete[] Data;
		ilDeleteImage(Image);
	}
	// установка параметров адресации текселей
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// генерирование mipmap'ов всех граней
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

// применение текстуры
void CCubeMap::Apply(GLenum texUnit)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TexIndex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	switch (CTexture::TexFilterMode)
	{
		case 0:
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		} break;
		case 1: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
		case 2: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
		case 3: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
		case 4: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2); break;
		case 5: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4); break;
		case 6: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8); break;
		case 7: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16); break;
		case 8: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 32); break;
	}
}