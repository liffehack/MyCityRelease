#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// КЛАСС ДЛЯ РАБОТЫ С ИСТОЧНИКОМ СВЕТА
class CLight
{
	// различные составляющие источника света 
	vec4 Position;
	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular;
public:
	// конструктор по умолчанию
	CLight(void);
	// задание различных параметров источника света
	void setPosition(vec4 position);
	void setAmbient(vec4 ambient);
	void setDiffuse(vec4 diffuse);
	void setSpecular(vec4 specular);
	// получение различных параметров источника света
	vec4 getPosition(void);
	vec4 getAmbient(void);
	vec4 getDiffuse(void);
	vec4 getSpecular(void);
	// оператор сравнения
	bool operator==(CLight &a);
	bool operator!=(CLight &a);
};