#pragma once

#include "stdio.h"
#include <map>

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// КЛАСС ДЛЯ РАБОТЫ С ШЕЙДЕРОМ
class CShader
{
private:
	// вершинный шейдер
	GLuint Vsh;
	// фрагментный шейдер
	GLuint Fsh;
	// шейдерная программа (шейдер)
	GLuint Program;
	// ассоциативный контейнер
	map<const char*, int> Uniform;

public:
	// получить номер шейдерной программы
	GLuint GetShaderProgram(void);
	// загрузить вершинный шейдер
	int LoadVertexShader(const char *VertexFileName, bool DebugOutput = true);
	// загрузить фрагментный шейдер
	int LoadFragmentShader(const char *FragmentFileName, bool DebugOutput = true);
	// слинковать шейдерную программу
	int Link(bool DebugOutput = true);
	//	активизировать шейдер (сделать текущим)
	void Activate(void);
	//	отключить шейдер (использовать нулевую шейдерную программу)
	static void	Deactivate(void);
	// получение индекса атрибут-переменной
	int GetAttribLocation(const char *name);
	// запись вектора из 4‐х комопнент в uniform‐переменную
	void SetUniformVec4(const char *name, vec4 value);
	// запись матрицы 4х4 в uniform‐переменную
	void SetUniformMat4(const char *name, mat4 value);
	// запись вещественного значения в uniform‐переменную
	void SetUniformFloat(const char *name, float value);
};