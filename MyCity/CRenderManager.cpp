#pragma once

#include "CRenderManager.h"

int CRenderManager::Samples = 8;
int CRenderManager::Filter = 3;
int CRenderManager::UBOUpdateCount = -1;

// переменные для вывода объекта (прямоугольника из двух треугольников)
GLuint	VAO_Index = 0;		// индекс VAO-буфера
GLuint	VBO_Index = 0;		// индекс VBO-буфера

// инициализация переменных для вывода объекта
void InitObject()
{
	// создание и заполнение VBO
	glGenBuffers(1, &VBO_Index);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Index);
	GLfloat	Verteces[] = { -1, 1, -1, -1, 1, 1, 1, 1, -1, -1, 1, -1 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(Verteces), Verteces, GL_STATIC_DRAW);
	// создание VAO
	glGenVertexArrays(1, &VAO_Index);
	glBindVertexArray(VAO_Index);
	// заполнение VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Index);
	int	k = 0;
	glVertexAttribPointer(k, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(k);
	// "отвязка" буфера VAO, чтоб случайно не испортить
	glBindVertexArray(0);
}

// инициализация объекта RenderManager
// выполняется после инициализации OpenGL: загрузка шейдеров и т.д.
void CRenderManager::init()
{
	// включаем необходимые режимы вывода
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_MULTISAMPLE);
	// загрузка шейдера для вывода объектов
	Shaders[0].LoadVertexShader("Shaders\\DirectLight.vsh", false);
	Shaders[0].LoadFragmentShader("Shaders\\DirectLight.fsh", false);
	Shaders[0].Link(false);
	// загрузка шейдера для вывода скайбокса
	Shaders[1].LoadVertexShader("Shaders\\SkyBox.vsh", false);
	Shaders[1].LoadFragmentShader("Shaders\\SkyBox.fsh", false);
	Shaders[1].Link(false);
	// загрузка шейдера для вывода AABB
	Shaders[2].LoadVertexShader("Shaders\\BoundingBox.vsh", false);
	Shaders[2].LoadFragmentShader("Shaders\\BoundingBox.fsh", false);
	Shaders[2].Link(false);
	// загрузка шейдера для вывода буфера кадра
	Shaders[3].LoadVertexShader("Shaders\\SimplePostProcessing.vsh", false);
	Shaders[3].LoadFragmentShader("Shaders\\SimplePostProcessing.fsh", false);
	Shaders[3].Link(false);
	// загрузка шейдера для ч/б фильтра
	Shaders[4].LoadVertexShader("Shaders\\GreyPostProcessing.vsh", false);
	Shaders[4].LoadFragmentShader("Shaders\\GreyPostProcessing.fsh", false);
	Shaders[4].Link(false);
	// загрузка шейдера для фильтра сепия
	Shaders[5].LoadVertexShader("Shaders\\SepiaPostProcessing.vsh", false);
	Shaders[5].LoadFragmentShader("Shaders\\SepiaPostProcessing.fsh", false);
	Shaders[5].Link(false);
	// загрузка меша для вывод скайбокса
	SkyBoxMeshId = CResourceManager::Instance().LoadMesh("Meshes\\box.obj");
	// загрузка меша для вывода AABB
	AABBMeshId = CResourceManager::Instance().LoadMesh("Meshes\\box.obj");
	// загрузка кубической текстуры для вывода скайбокса
	SkyBoxTextureId = CResourceManager::Instance().LoadCubeMap(3);
	// позиция uniform-блоков в шейдере
	int Scene_Block, Object_Block;
	// создаем UBO-буфер для всех объектов
	glGenBuffers(1, &PerSceneUBOIndex);
	// создаем точки привязки
	Scene_Block = glGetUniformBlockIndex(Shaders[0].GetShaderProgram(), "PerSceneBlock");
	glUniformBlockBinding(Shaders[0].GetShaderProgram(), Scene_Block, 0);
	Object_Block = glGetUniformBlockIndex(Shaders[0].GetShaderProgram(), "PerObjectBlock");
	glUniformBlockBinding(Shaders[0].GetShaderProgram(), Object_Block, 1);
	// обновляем сцену, т.к. до этого не обновлялась
	updatePerSceneBlock(PerSceneUBOIndex);
	// инициализация объекта для вывода буфера кадра
	InitObject();
}

// начало вывода очередного кадра
void CRenderManager::start()
{
	// очищаем очередь вывода
	GraphicObjects.clear();
}

// установка используемой камеры
void CRenderManager::setCamera(CCamera& camera)
{
	if (Camera != camera)
	{
		Camera = camera;
		updatePerSceneBlock(PerSceneUBOIndex);
		for (auto it = RenderManagerObjectStates.begin(); it != RenderManagerObjectStates.end(); it++)
			it->second.UBOIndexUpdate = true;
	};
}

// установка используемого источника света
void CRenderManager::setLight(CLight& light)
{
	Light = light;
}

// добавление в очередь рендеринга очередного объекта для вывода
void CRenderManager::addToRenderQueue(CGraphicObject& GraphicObject)
{
	GraphicObjects.push_back(GraphicObject);
}

// обновление FBO
void CRenderManager::UpdateFBO(int w, int h, int s)
{
	FBO.init(w, h);
	MSAA.init(w, h, s);
}

// завершение вывода кадра
void CRenderManager::finish(void)
{
	// привязка FBO со множественной выборкой
	MSAA.bind();
	// вывод скайбокса
	SkyBoxRender();
	// вывод объектов
	ObjectsRender();
	// вывод AABB
	if (AABBRenderEnable)
		AABBRender();
	// привязка стандартного буфера кадра	
	MSAA.unbind();
	// копироване текстуры в FBO без множественной выборки
	MSAA.resolveToFbo(FBO);
	// вывод текстуры на экран
	glDisable(GL_DEPTH_TEST);
	Shaders[Filter].Activate();
	FBO.bindColorTexture();
	FBO.bindDepthTexture();
	glBindVertexArray(VAO_Index);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	Shaders[Filter].Deactivate();
	glEnable(GL_DEPTH_TEST);
	// смена переднего и заднего буферов
	glutSwapBuffers();
}

// обновление буфера, общего для всех объектов сцены
void CRenderManager::updatePerSceneBlock(int Index)
{
	UBOUpdateCount++;
	// структура для передачи данных в шейдер
	struct SceneData
	{
		mat4 ProjectionMatrix;
		vec4 lAmbient;
		vec4 lDiffuse;
		vec4 lSpecular;
		vec4 lPosition;
	} UBO_Scene;
	// установка параметров сцены
	mat4 ViewMatrix = Camera.GetViewMatrix();
	UBO_Scene.ProjectionMatrix = Camera.GetProjectionMatrix();
	UBO_Scene.lAmbient = Light.getAmbient();
	UBO_Scene.lDiffuse = Light.getDiffuse();
	UBO_Scene.lSpecular = Light.getSpecular();
	UBO_Scene.lPosition = ViewMatrix * Light.getPosition();
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Index);
	glBindBuffer(GL_UNIFORM_BUFFER, Index);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneData), &UBO_Scene, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// обновление буфера, индивидуального для одного указанного объекта
void CRenderManager::updatePerObjectBlock(int UBOindex, CGraphicObject& GraphicObject)
{
	UBOUpdateCount++;
	// структура для передачи данных в шейдер
	struct ObjectData
	{
		mat4 ModelViewMatrix;
		vec4 mAmbient;
		vec4 mDiffuse;
		vec4 mSpecular;
		float Shininess;
	} UBO_Object;
	// установка параметров объекта
	mat4 ViewMatrix = Camera.GetViewMatrix();
	UBO_Object.ModelViewMatrix = ViewMatrix * GraphicObject.getModelMatrix();
	UBO_Object.mAmbient = GraphicObject.getMaterial().getAmbient();
	UBO_Object.mDiffuse = GraphicObject.getMaterial().getDiffuse();
	UBO_Object.mSpecular = GraphicObject.getMaterial().getSpecular();
	UBO_Object.Shininess = GraphicObject.getMaterial().getShininess();
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBOindex);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOindex);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ObjectData), &UBO_Object, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// вывод SkyBox'а
void CRenderManager::SkyBoxRender()
{
	Shaders[1].Activate();
	Shaders[1].SetUniformMat4("uProjectionMatrix", Camera.GetProjectionMatrix());
	Shaders[1].SetUniformMat4("uModelViewMatrix", Camera.GetViewMatrix());
	// настраиваем конвейер рендеринга
	glDepthFunc(GL_ALWAYS);
	glCullFace(GL_FRONT);
	// рисуем куб
	CCubeMap* Texture = CResourceManager::Instance().GetCubeMap(SkyBoxTextureId);
	if (Texture != nullptr) Texture->Apply();
	CMesh *mesh = CResourceManager::Instance().GetMesh(SkyBoxMeshId);
	mesh->Render();
	Shaders[1].Deactivate();
	// восстанавливаем конвейер рендеринга
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
}

// вывод всех объектов в нормальном режиме
void CRenderManager::ObjectsRender()
{
	// активируем шейдер для вывода объектов
	Shaders[0].Activate();
	for (int i = 0; i < GraphicObjects.size(); i++)
	{
		SRenderManagerObjectState Prev;
		// проверяем наличие объекта в контейнере
		auto it = RenderManagerObjectStates.find(GraphicObjects[i].getID());
		if (it != RenderManagerObjectStates.end())
		{
			Prev = it->second;
			if (Prev.GraphicObject != GraphicObjects[i])
				Prev.UBOIndexUpdate = true;
		}
		else
		{
			// добавляем элемент в контейнер
			Prev.GraphicObject = GraphicObjects[i];
			glGenBuffers(1, &Prev.UBOIndex);
			RenderManagerObjectStates.insert(pair<int, SRenderManagerObjectState>(GraphicObjects[i].getID(), Prev));
		}
		// обновление UBO-блока
		if (Prev.UBOIndexUpdate)
			updatePerObjectBlock(Prev.UBOIndex, Prev.GraphicObject);
		// вывод модели
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, Prev.UBOIndex);
		glBindBuffer(GL_UNIFORM_BUFFER, Prev.UBOIndex);
		CTexture* Texture = CResourceManager::Instance().GetTexture(GraphicObjects[i].getTexture());
		if (Texture != nullptr) Texture->Apply();
		CMesh* Mesh = CResourceManager::Instance().GetMesh(GraphicObjects[i].getMesh());
		if (Mesh != nullptr) Mesh->Render();
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	for (auto it = RenderManagerObjectStates.begin(); it != RenderManagerObjectStates.end(); it++)
		it->second.UBOIndexUpdate = false;
	// деактивируем шейдер для вывода объектов
	Shaders[0].Deactivate();
}

// вывод ААВВ всех объектов
void CRenderManager::AABBRender()
{
	// ВЫВОДИМ ВСЕ ААВВ
	// настраиваем конвейер рендеринга
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	// активируем шейдер (+устанавливаем некоторые его параметры)
	Shaders[AABB_SHADER_ID].Activate();
	Shaders[AABB_SHADER_ID].SetUniformMat4("ProjectionMatrix", Camera.GetProjectionMatrix());
	Shaders[AABB_SHADER_ID].SetUniformVec4("Color", vec4(1, 0, 0, 1));
	// выводим все модели (их ААВВ)
	for (auto it = GraphicObjects.begin(); it < GraphicObjects.end(); it++)
	{
		vec3 AABB = it->getAABB();
		mat4 scale = mat4(
			vec4(AABB.x, 0, 0, 0),
			vec4(0, AABB.y, 0, 0),
			vec4(0, 0, AABB.z, 0),
			vec4(0, 0, 0, 1));
		mat4 ModelViewMatrix = Camera.GetViewMatrix() * it->getModelMatrix() * scale;
		Shaders[AABB_SHADER_ID].SetUniformMat4("ModelViewMatrix", ModelViewMatrix);
		CMesh* mesh = CResourceManager::Instance().GetMesh(AABBMeshId);
		if (mesh != nullptr) mesh->Render();
	}
	Shaders[AABB_SHADER_ID].Deactivate();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}

// получение количества объектов, выведенных в данном кадре
int CRenderManager::getObjectCount()
{
	return GraphicObjects.size();
}

// смена режима вывода ААВВ
void CRenderManager::changeAABBRenderMode()
{
	AABBRenderEnable = !AABBRenderEnable;
}

// обновление UBO
void CRenderManager::UpdateUBO()
{
	for (auto it = RenderManagerObjectStates.begin(); it != RenderManagerObjectStates.end(); it++)
		it->second.UBOIndexUpdate = true;
}