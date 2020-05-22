#pragma once

#include "stdio.h"

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// КЛАСС ДЛЯ РАБОТЫ С КАМЕРОЙ
class CCamera
{
private:
	// координаты позиции наблюдателя
	vec3 Eye;
	// координаты точки наблюдения
	vec3 Point;
	// вектор вверх
	vec3 Up;
	// расстояние от наблюдателя до точки наблюдения
	float Radius;
	// угол в вертикальной плоскости
	float Zenith;
	// угол в горизонтальной плоскости
	float Azimuth;
	// матрица проекции
	mat4 ProjectionMatrix;
	// матрица наблюдения 
	mat4 ViewMatrix;

public:
	// конструктор по умолчанию
	CCamera(void);
	// деструктор 
	~CCamera(void);
	// установка матрицы проекции
	void SetProjectionMatrix(float fovy, float aspect, float zNear, float zFar);
	// получение матрицы проекции
	mat4 GetProjectionMatrix(void);
	// получить матрицу наблюдения
	mat4 GetViewMatrix(void);
	// получить позицию наблюдателя
	vec3 GetPosition(void);
	// передвинуть камеру и точку наблюдения в горизонтальной плоскости oXZ
	void MoveOXZ(float dForward, float dRight, float time);
	// вращение в горизонтальной и вертикальной плоскости
	void Rotate(float dHorizAngle, float dVertAngle, float time);
	// приближение/удаление
	void Zoom(float dR, float time);
	// оператор сравнения
	bool operator==(CCamera &a);
	bool operator!=(CCamera &a);
};