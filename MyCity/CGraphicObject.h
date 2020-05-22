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

//�����Ѡ��ߠ�����۠Ѡ����������̠��������
class CGraphicObject
{
private:
	//�������������������������������
	GLuint ID;
	// ��� �������
	unsigned char Type;
	//�����������������(������������������������)
	int meshID;
	//���������������������
	CMaterial Material;
	// ������������ ��������
	int TexID;
	//��������������(�������������������������)
	mat4 modelMatrix;
	// ���������� AABB
	vec3 AABB;
public:
	//�����������������������
	CGraphicObject(void);
	//��������������������������
	void setID(GLuint id);
	void setType(unsigned char type);
	void setMesh(int id);
	void setMaterial(CMaterial& material);
	void setPosition(vec3 position);
	void setRotation(float degree);
	void setTexture(int id);
	void setAABB(vec3 value);
	//������������������������������
	GLuint getID(void);
	unsigned char getType(void);
	int getMesh(void);
	CMaterial& getMaterial(void);
	mat4 getModelMatrix(void);
	int getTexture(void);
	vec3 getAABB();
	//������������������
	bool operator==(CGraphicObject &a);
	bool operator!=(CGraphicObject &a);
};