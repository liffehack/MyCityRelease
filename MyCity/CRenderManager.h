#pragma once

#include "CShader.h"
#include "CCamera.h"
#include "CResourceManager.h"
#include "CLight.h"
#include "CGraphicObject.h"
#include "CFBO.h"
#include <vector>

using namespace std;

// СТРУКТУРА, ХРАНЯЩАЯ ИНФОРМАЦИЮ О ПАРАМЕТРАХ МОДЕЛИ,
// КОТОРЫЕ БЫЛИ ИСПОЛЬЗОВАННЫ ДЛЯ ВЫВОДА МОДЕЛИ В ПРЕДЫДУЩИЙ РАЗ
// ЕСЛИ ПАРАМЕТРЫ БУДУТ ОТЛИЧАТЬСЯ, UNIFORM‐БУФЕР НЕОБХОДИМО ПЕРЕИНИЦИАЛИЗИРОВАТЬ
struct SRenderManagerObjectState
{
	// предыдущее состояние объекта (в том числе его цвет и матрица модели)
	CGraphicObject GraphicObject;
	// признак необходимости обновить буфер UBO потому, что обновились глобальные данные,
	// такие как, например, матрица наблюдения (то есть камера)
	bool UBOIndexUpdate = true;
	// индекс буфера UBO
	GLuint UBOIndex;
};

// КЛАСС ДЛЯ ВЗАИМОДЕЙСТВИЯ С OPENGL
// ВЕСЬ ВЫВОД ОСУЩЕСТВЛЯЕТСЯ ЧЕРЕЗ ЕДИНСТВЕННЫЙ ЭКЗЕМПЛЯР ДАННОГО КЛАССА
class CRenderManager
{
private:
	// используемые шейдеры (и их идентификаторы)
	enum { DIFFUSE_TEXTURE_SHADER_ID = 0, SKYBOX_SHADER_ID, AABB_SHADER_ID, FRAMEBUFFER_SHADER, GREY_SHADER, SEPIA_SHADER, MAX_SHADER_ID };
	CShader Shaders[MAX_SHADER_ID];
	// камера 
	CCamera Camera;
	// источник света
	CLight Light;
	// все графические объекты
	vector<CGraphicObject> GraphicObjects;
	// предыдущее состояние графических объектов
	// в качестве ключа выступает идентификатор объекта
	map<int, SRenderManagerObjectState> RenderManagerObjectStates;
	// индекс буфера UBO для данных, общих для всех объектов 
	GLuint PerSceneUBOIndex;
	// переменные для вывода скайбокса (индекс меша и кубической текстуры)
	int SkyBoxMeshId;
	int SkyBoxTextureId;
	// переменные для вывода ААВВ (индекс меша и режим вывода ААВВ)
	int AABBMeshId;
	bool AABBRenderEnable = false;
	// буфер кадра
	CFBO FBO;
	// буфер кадра со множественной выборкой
	CFBO MSAA;

private:
	// конструктор по умолчанию (объявлен приватным)
	// в результате нельзя создать ни одного объекта данного класса вне самого класса
	CRenderManager() {};
	// конструктора копирования нет
	CRenderManager(const CRenderManager& root) = delete;
	// оператора присваивания нет
	CRenderManager& operator = (const CRenderManager&) = delete;

public:
	// количество выборок
	static int Samples;
	// используемый фильтр изображения:
	// 3 - без фильтра
	// 4 - черно-белое изображение
	// 5 - сепия
	static int Filter;
	// количество обновлений буферов UBO
	static int UBOUpdateCount;
	// Статик‐метод для получения экземпляра менеджера ресурса.
	// Всегда будет возвращена ссылка на объект, хранящийся в единственном экземпляре.
	static CRenderManager& Instance()
	{
		static CRenderManager RenderManager;
		return RenderManager;
	}
	// инициализация объекта RenderManager
	// выполняется после инициализации OpenGL: загрузка шейдеров и т.д.
	void init(void);
	// начало вывода очередного кадра
	void start(void);
	// установка используемой камеры
	void setCamera(CCamera& camera);
	// установка используемого источника света
	void setLight(CLight& light);
	// добавление в очередь рендеринга очередного объекта для вывода
	void addToRenderQueue(CGraphicObject& GraphicObject);
	// завершение вывода кадра
	void finish(void);
	// получение количества объектов, выведенных в данном кадре
	int getObjectCount(void);
	// смена режима вывода ААВВ
	void changeAABBRenderMode(void);
	// обновление UBO
	void UpdateUBO(void);
	// обновление FBO
	void UpdateFBO(int w, int h, int s = Samples);

private:
	// вывод SkyBox'а
	void SkyBoxRender(void);
	// вывод всех объектов в нормальном режиме
	void ObjectsRender(void);
	// вывод ААВВ всех объектов
	void AABBRender(void);
	// обновление буфера, общего для всех объектов сцены
	void updatePerSceneBlock(int UBOindex);
	// обновление буфера, индивидуального для одного указанного объекта
	void updatePerObjectBlock(int UBOindex, CGraphicObject& GraphicObject);
};