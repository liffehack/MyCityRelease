#pragma once

#include "CLight.h"

// конструктор по умолчанию
CLight::CLight()
{
	Position = vec4(1, 1, 1, 0);
	Ambient = vec4(1, 1, 1, 1);
	Diffuse = vec4(1, 1, 1, 1);
	Specular = vec4(1, 1, 1, 1);
}

// задание различных параметров источника света
void CLight::setPosition(vec4 position)
{
	Position = position;
}

void CLight::setAmbient(vec4 ambient)
{
	Ambient = ambient;
}

void CLight::setDiffuse(vec4 diffuse)
{
	Diffuse = diffuse;
}

void CLight::setSpecular(vec4 specular)
{
	Specular = specular;
}

// получение различных параметров источника света
vec4 CLight::getPosition()
{
	return Position;
}

vec4 CLight::getAmbient()
{
	return Ambient;
}

vec4 CLight::getDiffuse()
{
	return Diffuse;
}

vec4 CLight::getSpecular()
{
	return Specular;
}

// оператор сравнения
bool CLight::operator==(CLight &a)
{
	if ((this->Ambient == a.Ambient) && (this->Diffuse == a.Diffuse) && (this->Specular == a.Specular) && (this->Position == a.Position))
		return true;
	else
		return false;
}

bool CLight::operator!=(CLight &a)
{

	if ((this->Ambient == a.Ambient) && (this->Diffuse == a.Diffuse) && (this->Specular == a.Specular) && (this->Position == a.Position))
		return false;
	else
		return true;
}