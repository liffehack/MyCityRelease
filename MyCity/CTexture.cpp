#pragma once

#include "CTexture.h"
#include <iostream>

using namespace std;

// режим фильтрации (точечная, билинейная, ближайший мипмап, трилинейная и т.д.)
int CTexture::TexFilterMode = 8;

// загрузка текстуры из внешнего файла
void CTexture::Load(const char *filename)
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &TexIndex);
	glBindTexture(GL_TEXTURE_2D, TexIndex);
	// создаем новое "изображение"
	int Image = ilGenImage();
	// задаем текущее "изображение"
	ilBindImage(Image);
	// загружаем изображение
	bool result = ilLoadImage(filename);
	if (!result) cout << filename << " is not loaded" << endl;
	// получение параметров загруженной текстуры
	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	ILubyte* Data = new ILubyte[width * height * 4];
	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, Data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	// удаление ненужной информации
	delete[] Data;
	ilDeleteImage(Image);
}

// применение текстуры
void CTexture::Apply(GLenum texUnit)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexIndex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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