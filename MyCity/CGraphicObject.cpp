#pragma once

#include "CGraphicObject.h"
#include <iostream>

// конструктор по умолчанию
CGraphicObject::CGraphicObject()
{
	ID = -1;
	meshID = -1;
	modelMatrix = mat4(
		vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(0, 0, 0, 1));
}

// установка внутренних полей
void CGraphicObject::setID(GLuint id)
{
	ID = id;
}

void CGraphicObject::setType(unsigned char type)
{
	Type = type;
}

void CGraphicObject::setMesh(int id)
{
	meshID = id;
}

void CGraphicObject::setMaterial(CMaterial& material)
{
	Material = material;
}

void CGraphicObject::setPosition(vec3 position)
{
	modelMatrix[3] = vec4(position, 1.0);
}

void CGraphicObject::setRotation(float degree)
{
	modelMatrix[0][0] = cos(radians(degree));
	modelMatrix[0][2] = sin(radians(degree));
	modelMatrix[2][0] = -sin(radians(degree));
	modelMatrix[2][2] = cos(radians(degree));
}

void CGraphicObject::setTexture(int id)
{
	TexID = id;
}

void CGraphicObject::setAABB(vec3 value)
{
	AABB = value;
}

// получение различных параметров
GLuint CGraphicObject::getID()
{
	return ID;
}

unsigned char CGraphicObject::getType()
{
	return Type;
}

int CGraphicObject::getMesh()
{
	return meshID;
}


CMaterial& CGraphicObject::getMaterial()
{
	return Material;
}

mat4 CGraphicObject::getModelMatrix()
{
	return modelMatrix;
}

int CGraphicObject::getTexture()
{
	return TexID;
}

vec3 CGraphicObject::getAABB()
{
	return AABB;
}

// оператор сравнения
bool CGraphicObject::operator==(CGraphicObject &a)
{
	if ((this->ID == a.ID) && (this->Material == a.Material) && (this->meshID == a.meshID) && (this->modelMatrix == a.modelMatrix))
		return true;
	else
		return false;
}

bool CGraphicObject::operator!=(CGraphicObject &a)
{
	if ((this->ID == a.ID) && (this->Material == a.Material) && (this->meshID == a.meshID) && (this->modelMatrix == a.modelMatrix))
		return false;
	else
		return true;
}