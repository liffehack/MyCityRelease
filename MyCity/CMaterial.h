#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// КЛАСС ДЛЯ РАБОТЫ С МАТЕРИАЛОМ
class CMaterial
{
protected:
	// различные составляющие материала  
	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular;
	GLfloat Shininess;
public:
	// конструктор по умолчанию
	CMaterial(void);
	// задание различных параметров материала
	void setAmbient(vec4 ambient);
	void setDiffuse(vec4 diffuse);
	void setSpecular(vec4 specular);
	void setShininess(float shininess);
	// получение различных параметров материала
	vec4 getAmbient(void);
	vec4 getDiffuse(void);
	vec4 getSpecular(void);
	float getShininess(void);
	// оператор сравнения
	bool operator==(CMaterial &a);
	bool operator!=(CMaterial &a);
};