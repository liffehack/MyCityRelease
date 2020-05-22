#pragma once

#include "CTexture.h"

class CCubeMap : public CTexture
{
public:
	// загрузка текстуры из внешнего файла
	virtual void Load(int Index);
	// применение текстуры
	virtual void Apply(GLenum texUnit = GL_TEXTURE0);
};