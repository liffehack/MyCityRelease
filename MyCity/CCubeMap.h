#pragma once

#include "CTexture.h"

class CCubeMap : public CTexture
{
public:
	//���������������������������������
	virtual void Load(int Index);
	//�������������������
	virtual void Apply(GLenum texUnit = GL_TEXTURE0);
};