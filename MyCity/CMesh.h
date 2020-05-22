#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// КЛАСС ДЛЯ РАБОТЫ С МЕШЕМ
class CMesh
{
private:
	GLuint VAO_Index; // индекс VAO‐буфера
	GLuint VBO_Index; // индекс VBO‐буфера
	int VertexCount; // количество вершин
public:
	// конструктор
	CMesh(void);
	// загрузка меша из внешнего obj‐файла
	int Load(const char* filename);
	// вывод модели (используется текущий шейдер)
	void Render(void);
};