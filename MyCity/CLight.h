#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

//�����Ѡ��ߠ�����۠Ѡ���������̠�����
class CLight
{
	//������������������������������������
	vec4 Position;
	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular;
public:
	//�����������������������
	CLight(void);
	//������������������������������������������
	void setPosition(vec4 position);
	void setAmbient(vec4 ambient);
	void setDiffuse(vec4 diffuse);
	void setSpecular(vec4 specular);
	//��������������������������������������������
	vec4 getPosition(void);
	vec4 getAmbient(void);
	vec4 getDiffuse(void);
	vec4 getSpecular(void);
	//������������������
	bool operator==(CLight &a);
	bool operator!=(CLight &a);
};