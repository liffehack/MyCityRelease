#pragma once

#include "stdio.h"

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

//�����Ѡ��ߠ�����۠Ѡ�������
class CCamera
{
private:
	// ���������� ������� �����������
	vec3 Eye;
	// ���������� ����� ����������
	vec3 Point;
	// ������ �����
	vec3 Up;
	// ���������� �� ����������� �� ����� ����������
	float Radius;
	// ���� � ������������ ���������
	float Zenith;
	// ���� � �������������� ���������
	float Azimuth;
	//����������������
	mat4 ProjectionMatrix;
	//�������������������
	mat4 ViewMatrix;

public:
	//�����������������������
	CCamera(void);
	//�����������
	~CCamera(void);
	//��������������������������
	void SetProjectionMatrix(float fovy, float aspect, float zNear, float zFar);
	//��������������������������
	mat4 GetProjectionMatrix(void);
	//���������������������������
	mat4 GetViewMatrix(void);
	// �������� ������� �����������
	vec3 GetPosition(void);
	//������������������������������������������������������������oXZ
	void MoveOXZ(float dForward, float dRight, float time);
	//����������������������������������������������
	void Rotate(float dHorizAngle, float dVertAngle, float time);
	//������������/��������
	void Zoom(float dR, float time);
	//������������������
	bool operator==(CCamera &a);
	bool operator!=(CCamera &a);
};