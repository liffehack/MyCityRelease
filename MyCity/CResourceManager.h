#pragma once

#include "CMesh.h"
#include "CTexture.h"
#include "CCubeMap.h"
#include <vector>
#include <map>

using namespace std;

// КЛАСС ДЛЯ РАБОТЫ С МЕНЕДЖЕРОМ РЕСУРСОВ (РЕАЛИЗОВАН НА ОСНОВЕ ШАБЛОНА SINGLETON)
class CResourceManager
{
private:
	// вектор для хранения мешей
	vector<CMesh*> meshes;
	// контейнер для хранения соответствия между именем запрашиваемого файла 
	// и индексом в векторе meshes
	map<const char*, int> meshes_index;
	// вектор для хранения текстур
	vector<CTexture*> textures;
	// контейнер для хранения соответствия между именем запрашиваемого файла 
	// и индексом в векторе textures
	map<const char*, int> tex_index;
	// вектор для хранения кубических текстур
	vector<CCubeMap*> cubemaps;
	// контейнер для хранения соответствия между именем запрашиваемого файла 
	// и индексом в векторе cubemaps
	map<int, int> cubemap_index;
	// конструктор по умолчанию (объявлен приватным)
	// в результате нельзя создать ни одного объекта данного класса вне самого класса
	CResourceManager() {};
	// конструктора копирования нет
	CResourceManager(const CResourceManager& root) = delete;
	// оператора присваивания нет
	CResourceManager& operator=(const CResourceManager&) = delete;

public:
	// Статик‐метод для получения экземпляра менеджера ресурса.
	// Всегда будет возвращена ссылка на статичный объект,
	// хранящийся в единственном экземпляре.
	static CResourceManager& Instance()
	{
		static CResourceManager ResourceManager;
		return ResourceManager;
	}
	// Загрузка одного меша.
	// Возвращаемое значение: индекс меша в менеджере ресурсов 
	// Осуществляется проверка на попытку загрузки ранее загруженного меша
	int LoadMesh(const char* filename);
	// Получение меша по его индексу.
	// Если такого меша нет (не действительный индекс) возвращается nullptr
	CMesh* GetMesh(int index);
	// загрузка текстуры
	int LoadTexture(const char* filename);
	// получение текстуры по индексу
	CTexture* GetTexture(int index);
	// загрузка кубической текстуры
	int LoadCubeMap(int Index);
	// получение кубической текстуры по индексу
	CCubeMap* GetCubeMap(int index);
	// удаление всех ресурсов
	void DeleteResources(void);
};