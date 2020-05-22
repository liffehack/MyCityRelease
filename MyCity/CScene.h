#pragma once

#include "CShader.h"
#include "CCamera.h"
#include "CResourceManager.h"
#include "CLight.h"
#include "CGraphicObject.h"
#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include "CRenderManager.h"
#include "CNetProtocol.h"

using namespace pugi;

// КЛАСС ДЛЯ РАБОТЫ СО СЦЕНОЙ
class CScene
{
private:
	// камера 
	CCamera Camera;
	// источник света
	CLight Light;
	// контейнер для хранения всех графических объектов сцены
	map<int, CGraphicObject> GraphicObjects;
	// xml‐документ в котором приводится описание всех объектов   
	// чтобы не загружать xml‐файл каждый раз, он загружается на этапе инициализации
	xml_document resources_description;
public:
	// объект класса для работы с сервером
	CNetProtocol NetProtocol;
	// Инициализация сцены
	void init(void);
	// Симуляция сцены
	void simulate(float sec);
	// Вывод всей сцены
	void draw(void);
	// Получение камеры для модификации её в некоторых функциях обратного вызова
	// например: reshape или mouseWheel
	CCamera* getCamera(void);
	// получение количества объектов
	int GetObjectCount(void);
	// используемый режим оптимизации:
	// 0 - без оптимизации
	// 1 - frustrum culling
	// 2 - frustrum culling + lod
	static int Optimization;
private:
	// создание объекта, используя описание модели из файла "Resources.xml"
	CGraphicObject CreateGraphicObject(const char* name);
};