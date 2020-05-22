#pragma once

#include <winsock2.h>
#include <windows.h>

#include <iostream>
#include <vector>
using namespace std;

// СТРУКТУРА ДЛЯ РАБОТЫ С ЗАГОЛОВКОМ ЗАПРОСА/ОТВЕТА
#pragma pack(push,1)
struct SNetworkHeader
{
	// номер транзакции 
	unsigned short transactionID;
	// номер пакета среди группы пакетов одного запроса/ответа
	unsigned short frameNumber;
	// всего пакетов в группе пакетов одного запроса/ответа
	unsigned short frameCount;
	// длина оставшейся части данных текущего пакета (включая funcID)
	unsigned int dataLen;
	// код запроса/ответа в соответствии с протоколом
	unsigned char funcID;
};
#pragma pack(pop)

// СТРУКТУРА, ОПИСЫВАЮЩАЯ ОДИН ОБЪЕКТ
#pragma pack(push,1)
struct SGameObjectDescription
{
	// идентификатор объекта
	unsigned int ObjectID;
	// тип объекта 
	unsigned char ObjectType;
	// позиция объекта (x, y, z)
	float x, y, z;
	// угол поворота в градусах по часовой стрелке вокруг оси oY
	float Yaw;
	// имя модели (в файле Resources.xml)
	char Model[16];
	// координаты AABB
	float AABB[3];
	// зарезервированные поля
	char Reserved[47];
};
#pragma pack(pop)

// КЛАСС РАБОТЫ С СЕРВЕРОМ ПО ОПРЕДЕЛЕННОМУ ПРОТОКОЛУ
class CNetProtocol
{
private:
	// сокет, используемый для обмена сообщениями
	SOCKET s;
	// признак установленного соединения
	bool connected = false;
public:
	// установка соединения (параметры ‐ IP‐адрес и номер порта)
	bool init(const char* ip, short port);
	// получение приветственного сообщения (команда 00)
	char* getMessage(void);
	// получить демо‐сцену (команда 01)
	void getDemoScene(void);
	// получить рядом лежащие объекты (команда 02)
	void getNearbyObjects(void);// (float x, float y, float z, float r);
private:
	// ожидание нужного количества байт в буфере приема
	void waitingToRead(unsigned int count);
};