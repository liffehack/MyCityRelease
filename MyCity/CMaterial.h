#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

//�����Ѡ��ߠ�����۠Ѡ����������
class CMaterial
{
protected:
	//������������������������������ࠠ
	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular;
	GLfloat Shininess;
public:
	//�����������������������
	CMaterial(void);
	//�������������������������������������
	void setAmbient(vec4 ambient);
	void setDiffuse(vec4 diffuse);
	void setSpecular(vec4 specular);
	void setShininess(float shininess);
	//���������������������������������������
	vec4 getAmbient(void);
	vec4 getDiffuse(void);
	vec4 getSpecular(void);
	float getShininess(void);
	//������������������
	bool operator==(CMaterial &a);
	bool operator!=(CMaterial &a);
};