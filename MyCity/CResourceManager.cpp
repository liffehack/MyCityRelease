#pragma once

#include "CResourceManager.h"
#include <iostream>

// Загрузка одного меша.
// Возвращаемое значение: индекс меша в менеджере ресурсов 
// Осуществляется проверка на попытку загрузки ранее загруженного меша
int CResourceManager::LoadMesh(const char* filename)
{
	int k = -1;
	auto it = meshes_index.find(filename);
	if (it != meshes_index.end())
		k = it->second;
	else
	{
		CMesh* mesh = new CMesh;
		mesh->Load(filename);
		meshes.push_back(mesh);
		k = meshes.size() - 1;
		meshes_index.insert(pair<const char*, int>(filename, k));
	}
	//cout << filename << " " << k << endl;
	return k;
}

// Получение меша по его индексу.
// Если такого меша нет (не действительный индекс) возвращается nullptr
CMesh* CResourceManager::GetMesh(int index)
{
	if (index <= meshes.size())
	{
		CMesh* mesh = meshes[index];
		return mesh;
	}

	else
		return nullptr;
}

// загрузка текстуры
int CResourceManager::LoadTexture(const char* filename)
{
	int k = -1;
	auto it = tex_index.find(filename);
	if (it != tex_index.end())
		k = it->second;
	else
	{
		CTexture* tex = new CTexture;
		tex->Load(filename);
		textures.push_back(tex);
		k = textures.size() - 1;
		tex_index.insert(pair<const char*, int>(filename, k));
	}
	//cout << filename << " " << k << endl;
	return k;
}

// получение текстуры по индексу
CTexture* CResourceManager::GetTexture(int index)
{
	if (index <= textures.size())
	{
		CTexture* tex = textures[index];
		return tex;
	}

	else
		return nullptr;
}

// загрузка кубической текстуры
int CResourceManager::LoadCubeMap(int Index)
{
	int k = -1;
	auto it = cubemap_index.find(Index);
	if (it != cubemap_index.end())
		k = it->second;
	else
	{
		CCubeMap* cube = new CCubeMap;
		cube->Load(Index);
		cubemaps.push_back(cube);
		k = cubemaps.size() - 1;
		cubemap_index.insert(pair<int, int>(Index, k));
	}
	return k;
}

// получение кубической текстуры по индексу
CCubeMap* CResourceManager::GetCubeMap(int index)
{
	if (index <= cubemaps.size())
	{
		CCubeMap* cube = cubemaps[index];
		return cube;
	}

	else
		return nullptr;
}

// Удаление всех ресурсов
void CResourceManager::DeleteResources()
{
	for (int i = meshes.size(); i > 0; i--)
	{
		delete meshes[i - 1];
	}
	meshes.clear();

	for (int i = textures.size(); i > 0; i--)
	{
		delete textures[i - 1];
	}
	textures.clear();
}