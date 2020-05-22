#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CTexture.h"
#include "CMaterial.h"
#include "CMesh.h"

using namespace glm;

// КЛАСС ДЛЯ РАБОТЫ С ГРАФИЧЕСКИМ ОБЪЕКТОМ
class CGraphicObject
{
private:
	// уникальный идентификатор объекта
	GLuint ID;
	// тип объекта
	unsigned char Type;
	// используемый меш (индекс в менеджере ресурсов)
	int meshID;
	// используемый материал
	CMaterial Material;
	// используемая текстура
	int TexID;
	// матрица модели (задает позицию и ориентацию)
	mat4 modelMatrix;
	// координаты AABB
	vec3 AABB;
public:
	// конструктор по умолчанию
	CGraphicObject(void);
	// установка внутренних полей
	void setID(GLuint id);
	void setType(unsigned char type);
	void setMesh(int id);
	void setMaterial(CMaterial& material);
	void setPosition(vec3 position);
	void setRotation(float degree);
	void setTexture(int id);
	void setAABB(vec3 value);
	// получение различных параметров
	GLuint getID(void);
	unsigned char getType(void);
	int getMesh(void);
	CMaterial& getMaterial(void);
	mat4 getModelMatrix(void);
	int getTexture(void);
	vec3 getAABB();
	// оператор сравнения
	bool operator==(CGraphicObject &a);
	bool operator!=(CGraphicObject &a);
};