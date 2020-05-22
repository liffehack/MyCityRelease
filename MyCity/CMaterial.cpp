#pragma once

#include "CMaterial.h"

// конструктор по умолчанию
CMaterial::CMaterial()
{
	Ambient = vec4(0.5, 0.5, 0.5, 1);
	Diffuse = vec4(0.5, 0.5, 0.5, 1);
	Specular = vec4(0.5, 0.5, 0.5, 1);
	Shininess = 100;
}

// задание различных параметров источника света
void CMaterial::setAmbient(vec4 ambient)
{
	Ambient = ambient;
}

void CMaterial::setDiffuse(vec4 diffuse)
{
	Diffuse = diffuse;
}

void CMaterial::setSpecular(vec4 specular)
{
	Specular = specular;
}

void CMaterial::setShininess(float shininess)
{
	Shininess = shininess;
}


// получение различных параметров источника света
vec4 CMaterial::getAmbient()
{
	return Ambient;
}

vec4 CMaterial::getDiffuse()
{
	return Diffuse;
}

vec4 CMaterial::getSpecular()
{
	return Specular;
}

float CMaterial::getShininess()
{
	return Shininess;
}

// оператор сравнения
bool CMaterial::operator==(CMaterial &a)
{
	if ((this->Ambient == a.Ambient) && (this->Diffuse == a.Diffuse) && (this->Specular == a.Specular) && (this->Shininess == a.Shininess))
		return true;
	else
		return false;
}

bool CMaterial::operator!=(CMaterial &a)
{
	if ((this->Ambient == a.Ambient) && (this->Diffuse == a.Diffuse) && (this->Specular == a.Specular) && (this->Shininess == a.Shininess))
		return false;
	else
		return true;
}
