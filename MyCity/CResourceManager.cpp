#pragma once

#include "CResourceManager.h"
#include <iostream>

//�������������������.
//���������������������:�����������������������������
//��������������������������������������������������������������
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

//��������������������������.
//������������������(����������������������)��������������nullptr
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

// �������� ��������
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

// ��������� �������� �� �������
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

// �������� ���������� ��������
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

// ��������� ���������� �������� �� �������
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

// �������� ���� ��������
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