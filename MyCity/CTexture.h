#pragma once

#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/freeglut.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

//�����Ѡ��ߠ�����۠Ѡ���������
class CTexture
{
protected:
	//�������������������������
	GLuint TexIndex;
public:
	//����������������
	static int TexFilterMode;
	//���������������������������������
	virtual void Load(const char *filename);
	//�������������������
	virtual void Apply(GLenum texUnit = GL_TEXTURE0);
};