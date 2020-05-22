#pragma once

#include "CScene.h"
#include <stdio.h>
#include <thread>
#include <mutex>

int CScene::Optimization = 0;

// разделяемая память
vector<SGameObjectDescription> scene;
 mutex m;
float pos[3];
int r;

// Инициализация сцены
void CScene::init()
{
	// загрука xml‐документа
	xml_parse_result result = resources_description.load_file("Resources.xml");
	printf_s("XML parsing result: %s\n\n", result.description());
	// загрузка параметров источника света
	xml_node Resources = resources_description.child("Resources");
	xml_node light = Resources.child("Light");
	xml_node Direction = light.child("Direction");
	xml_attribute attr = Direction.attribute("vector");
	GLfloat Value[3];
	sscanf_s(attr.value(), "%f %f %f", &Value[0], &Value[1], &Value[2]);
	Light.setPosition(vec4(Value[0], Value[1], Value[2], 0.0));
	xml_node PhongParameters = light.child("PhongParameters");
	attr = PhongParameters.attribute("diffuse");
	sscanf_s(attr.value(), "%f %f %f", &Value[0], &Value[1], &Value[2]);
	Light.setDiffuse(vec4(Value[0], Value[1], Value[2], 1.0));
	attr = PhongParameters.attribute("ambient");
	sscanf_s(attr.value(), "%f %f %f", &Value[0], &Value[1], &Value[2]);
	Light.setAmbient(vec4(Value[0], Value[1], Value[2], 1.0));
	attr = PhongParameters.attribute("specular");
	sscanf_s(attr.value(), "%f %f %f", &Value[0], &Value[1], &Value[2]);
	Light.setSpecular(vec4(Value[0], Value[1], Value[2], 1.0));
	// инициализация рендер-менеджера
	CRenderManager::Instance().init();
	// установка источника света
	CRenderManager::Instance().setLight(Light);
	// инициализация объекта класса для работы с сервером
	NetProtocol.init("127.0.0.1", 27000);
	char *Message;
	Message = NetProtocol.getMessage();
	if (Message) cout << Message << endl;
	delete[] Message;

	vec3 p = Camera.GetPosition();
	pos[0] = p.x;
	pos[1] = p.y;
	pos[2] = p.z;
	r = 200;

	thread Network(&CNetProtocol::getNearbyObjects, NetProtocol);
	Network.detach();
}


// Симуляция сцены
void CScene::simulate(float sec)
{
	// позиция курсора
	static POINT Prev, Cur;
	// вращение камеры
	bool RMB = GetAsyncKeyState(VK_RBUTTON);
	static bool Rotate = false;

	if (RMB && Rotate)
	{
		GetCursorPos(&Cur);
		Camera.Rotate(float(Cur.x - Prev.x), float(Cur.y - Prev.y), sec);
		Prev = Cur;
	}
	if (RMB && !Rotate)
	{
		GetCursorPos(&Prev);
		Rotate = true;
	}
	if (!RMB && Rotate)
		Rotate = false;
	// движение камеры
	bool CameraUp = GetAsyncKeyState(VK_UP);
	bool CameraDown = GetAsyncKeyState(VK_DOWN);
	bool CameraLeft = GetAsyncKeyState(VK_LEFT);
	bool CameraRight = GetAsyncKeyState(VK_RIGHT);

	if (CameraUp)
		Camera.MoveOXZ(-1, 0.0, sec);
	if (CameraDown)
		Camera.MoveOXZ(1, 0.0, sec);
	if (CameraLeft)
		Camera.MoveOXZ(0.0, 1, sec);
	if (CameraRight)
		Camera.MoveOXZ(0.0, -1, sec);

	vec3 p = Camera.GetPosition();

	m.lock();
	pos[0] = p.x;
	pos[1] = p.y;
	pos[2] = p.z;
	
	for (auto obj = scene.begin(); obj != scene.end(); obj++)
	{
		CGraphicObject Object;
		Object = CreateGraphicObject(obj->Model);
		Object.setID(obj->ObjectID);
		Object.setType(obj->ObjectType);
		Object.setPosition(vec3(obj->x, obj->y, obj->z));
		Object.setRotation(-obj->Yaw);
		Object.setAABB(vec3(obj->AABB[0], obj->AABB[1], obj->AABB[2]));
		auto it = GraphicObjects.find(obj->ObjectID);
		if (it != GraphicObjects.end())
		{
			if (it->second != Object)
				it->second = Object;
		}
		else
			GraphicObjects.insert(pair<int, CGraphicObject>(obj->ObjectID, Object));
	}
	scene.clear();
	m.unlock();
}

// Вывод всей сцены
void CScene::draw()
{
	// передаем в рендер‐менеджер используемую камеру
	CRenderManager::Instance().setCamera(Camera);
	// передаем в рендер‐менеджер используемый источник света
	CRenderManager::Instance().setLight(Light);
	// передаем все модели
	for (auto it = GraphicObjects.begin(); it != GraphicObjects.end(); it++)
	{
		// без оптимизации
		if (!Optimization)
		{
			// добавление объекта в очередь вывода
			CRenderManager::Instance().addToRenderQueue(it->second);
		}
		else
		{
			// проверка попадания модели в кадр
			bool f = false;
			// координаты ограничивающих объемов
			vec4 Box[8] =
			{
				vec4(it->second.getAABB().x, it->second.getAABB().y, it->second.getAABB().z, 1.0),
				vec4(it->second.getAABB().x, it->second.getAABB().y, -it->second.getAABB().z, 1.0),
				vec4(it->second.getAABB().x, -it->second.getAABB().y, it->second.getAABB().z, 1.0),
				vec4(it->second.getAABB().x, -it->second.getAABB().y, -it->second.getAABB().z, 1.0),
				vec4(-it->second.getAABB().x, it->second.getAABB().y, it->second.getAABB().z, 1.0),
				vec4(-it->second.getAABB().x, it->second.getAABB().y, -it->second.getAABB().z, 1.0),
				vec4(-it->second.getAABB().x, -it->second.getAABB().y, it->second.getAABB().z, 1.0),
				vec4(-it->second.getAABB().x, -it->second.getAABB().y, -it->second.getAABB().z, 1.0),
			};
			// матрица преобразования в систему нормализованных координат устройства
			mat4 PVM = Camera.GetProjectionMatrix() * Camera.GetViewMatrix() * it->second.getModelMatrix();
			// расчет frustrum culling
			for (int i = 0; i < 8; i++)
			{
				Box[i] = PVM * Box[i];
				Box[i] /= Box[i].w;
				if ((Box[i].x > -1.0) && (Box[i].x < 1.0) && (Box[i].y > -1.0) && (Box[i].y < 1.0) && (Box[i].z > -1.0) && (Box[i].z < 1.0))
				{
					f = true;
					break;
				}
			}
			// расчет lod
			if ((f) && (Optimization == 2))
			{
				// расстояние отсечения объектов в зависимости от их типа
				static float lodDistance[8] = { 500, 500, 500, 500, 125, 125, 75, 75 };
				// определение расстояния отсечения
				float Req = lodDistance[it->second.getType()];
				// проверка расстояния до наблюдателя
				float Distance = distance(vec3(it->second.getModelMatrix()[3]), vec3(Camera.GetPosition()));
				if ((Distance > Req) || (Distance > 700))
					f = false;
			}			
			// объекты, прошедшие все тесты, отправляются в рендер-менеджер
			if (f)
			{
				// добавление объекта в очередь вывода
				CRenderManager::Instance().addToRenderQueue(it->second);
			}
		}
	}
}

// Получение камеры для модификации её в некоторых функциях обратного вызова
// например: reshape или mouseWheel
CCamera* CScene::getCamera()
{
	return &Camera;
}

// создание объекта, используя описание модели из файла "Resources.xml"
CGraphicObject CScene::CreateGraphicObject(const char* name)
{
	// загружаем всю сцену
	xml_node Resources = resources_description.child("Resources");
	// узел для считывания всех моделей
	xml_node Models = Resources.child("Models");
	// узел для считывания одной модели
	xml_node Model = Models.child("Model");
	// узлы для считывания параметров модели
	xml_node Mesh;
	xml_node PhongParameters;
	xml_node Material;
	xml_node Texture;
	// переменная для атрибутов узлов
	xml_attribute attr = Model.attribute("id");
	// массив для считывания параметров материала
	GLfloat Value[3];
	// временные объекты
	CGraphicObject temp_object;
	CMaterial temp_material;

	while (strcmp(name, attr.as_string()))
	{
		if (!attr)
			return temp_object;
		Model = Model.next_sibling("Model");
		attr = Model.attribute("id");
	}

	// загружаем меш
	Mesh = Model.child("Mesh");
	attr = Mesh.attribute("path");
	temp_object.setMesh(CResourceManager::Instance().LoadMesh(attr.as_string()));
	// загружаем материал
	Material = Model.child("Material");
	// загружаем текстуру
	Texture = Material.child("Texture");
	attr = Texture.attribute("path");
	temp_object.setTexture(CResourceManager::Instance().LoadTexture(attr.as_string()));
	// загружаем параметры материала по Фонгу
	PhongParameters = Material.child("PhongParameters");
	// загружаем диффузную составляющую материала 
	attr = PhongParameters.attribute("diffuse");
	sscanf_s(attr.value(), "%f %f %f", &Value[0], &Value[1], &Value[2]);
	temp_material.setDiffuse(vec4(Value[0], Value[1], Value[2], 1.0));
	// загружаем фоновый коэффициент
	attr = PhongParameters.attribute("ambient");
	sscanf_s(attr.value(), "%f %f %f", &Value[0], &Value[1], &Value[2]);
	temp_material.setAmbient(vec4(Value[0], Value[1], Value[2], 1.0));
	// загружаем зеркальный коэффициент
	attr = PhongParameters.attribute("specular");
	sscanf_s(attr.value(), "%f %f %f", &Value[0], &Value[1], &Value[2]);
	temp_material.setSpecular(vec4(Value[0], Value[1], Value[2], 1.0));
	// загружаем степень отполированности
	attr = PhongParameters.attribute("shininess");
	sscanf_s(attr.value(), "%f", &Value[0]);
	temp_material.setShininess(Value[0]);
	temp_object.setMaterial(temp_material);
	return temp_object;
}

// получение количества объектов
int CScene::GetObjectCount()
{
	return GraphicObjects.size();
}