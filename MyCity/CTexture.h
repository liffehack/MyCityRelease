#pragma once

#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/freeglut.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

// КЛАСС ДЛЯ РАБОТЫ С ТЕКСТУРОЙ
class CTexture
{
protected:
	// индекс текстурного объекта
	GLuint TexIndex;
public:
	// режим фильтрации 
	static int TexFilterMode;
	// загрузка текстуры из внешнего файла
	virtual void Load(const char *filename);
	// применение текстуры
	virtual void Apply(GLenum texUnit = GL_TEXTURE0);
};