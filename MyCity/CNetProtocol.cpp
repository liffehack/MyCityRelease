#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <mutex>

#include "CNetProtocol.h"
#include <iostream>

// разделяемая память
extern vector<SGameObjectDescription> scene;
extern mutex m;
extern float pos[3];
extern int r;

// установка соединения (параметры ‐ IP‐адрес и номер порта)
bool CNetProtocol::init(const char* ip, short port)
{
	// инициализация библиотеки winsock
	WSADATA wsa;
	printf("Initialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		printf("Failed. Error Code : %d\n", WSAGetLastError());
	else
		printf("OK\n");
	// создание сокета
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
		printf("Could not create socket : %d\n", WSAGetLastError());
	else
		printf("Socket created.\n");
	// задание адреса сервера и номера порта
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_port = htons(port);
	// подключение к серверу 
	int connect_error = connect(s, (struct sockaddr *)&server, sizeof(server));
	if (connect_error == SOCKET_ERROR)
		printf("Connection failed : %d\n", WSAGetLastError());
	else
	{
		printf("Connection is established.\n");
		connected = true;
	}
	return connected;
}

// получение приветственного сообщения (команда 00)
char* CNetProtocol::getMessage()
{
	SNetworkHeader header;
	header.transactionID = 0;	// номер транзакции
	header.frameNumber = 1;		// первый пакет ...
	header.frameCount = 1;		// ... из одного
	header.dataLen = 1;			// отправляется один байт данных (код запроса)
	header.funcID = 0;			// код запроса = 0 (запрос приветственного сообщения)
	int send_bytes_count = send(s, (const char*)&header, sizeof(SNetworkHeader), 0);
	if (send_bytes_count < sizeof(SNetworkHeader))
		printf("Data was not send: %d.\n", WSAGetLastError());
	int receive_data_count = recv(s, (char*)&header, sizeof(SNetworkHeader), 0);
	if (receive_data_count <= 0)
		printf("Data was not received: %d.\n", WSAGetLastError());
	else
	{
		waitingToRead(header.dataLen - 1);
		char *Result = new char[header.dataLen - 1];
		recv(s, Result, header.dataLen - 1, 0);
		return Result;
	}
	return NULL;
}

// получить демо‐сцену (команда 01)
void CNetProtocol::getDemoScene()
{
	if (connected)
	{
		SNetworkHeader header;
		header.transactionID = 1;
		header.frameNumber = 1;
		header.frameCount = 1;
		header.dataLen = 1;
		header.funcID = 1;
		// отправка запроса
		int send_bytes_count = send(s, (const char*)&header, sizeof(SNetworkHeader), 0);
		if (send_bytes_count < sizeof(SNetworkHeader))
			printf("Data was not send: %d.\n", WSAGetLastError());
		// получение ответа
		int receive_data_count = recv(s, (char*)&header, sizeof(SNetworkHeader), 0);
		if (receive_data_count <= 0)
			printf("Data was not received: %d.\n", WSAGetLastError());
		else
		{
			unsigned int Count;
			SGameObjectDescription GameObject;
			waitingToRead(4);
			recv(s, (char*)&Count, 4, 0);
			while (Count > 0)
			{
				waitingToRead(sizeof(SGameObjectDescription));
				recv(s, (char*)&GameObject, sizeof(SGameObjectDescription), 0);
				scene.push_back(GameObject);
				Count--;
			}
		};
	}
}

// получить рядом лежащие объекты (команда 02)
void CNetProtocol::getNearbyObjects()//(float x, float y, float z, float r)
{
	while (connected)
	{
		static struct Data { float x, y, z, r; } Send;
		m.lock();
		Send.x = pos[0];
		Send.y = pos[1];
		Send.z = pos[2];
		Send.r = r;
		m.unlock();

		SNetworkHeader header;
		header.transactionID = 1;
		header.frameNumber = 1;
		header.frameCount = 1;
		header.dataLen = 17;
		header.funcID = 2;
		// отправка заголовка
		int send_bytes_count = send(s, (const char*)&header, sizeof(SNetworkHeader), 0);
		if (send_bytes_count < sizeof(SNetworkHeader))
			printf("Data was not send: %d.\n", WSAGetLastError());
		// отправка данных
		send_bytes_count = send(s, (const char*)&Send, sizeof(Data), 0);
		if (send_bytes_count < sizeof(Data))
			printf("Data was not send: %d.\n", WSAGetLastError());
		// получение ответа
		int receive_data_count = recv(s, (char*)&header, sizeof(SNetworkHeader), 0);
		if (receive_data_count <= 0)
			printf("Data was not received: %d.\n", WSAGetLastError());
		else
		{
			unsigned int Count;
			SGameObjectDescription GameObject;
			waitingToRead(4);
			recv(s, (char*)&Count, 4, 0);
			while (Count > 0)
			{
				waitingToRead(sizeof(SGameObjectDescription));
				recv(s, (char*)&GameObject, sizeof(SGameObjectDescription), 0);
				m.lock();
				scene.push_back(GameObject);
				m.unlock();
				Count--;
			}
		};
		Sleep(1000);
	}
}

// ожидание нужного количества байт в буфере приема
void CNetProtocol::waitingToRead(unsigned int count)
{
	// проверка того, сколько байт доступно для чтения
	unsigned long bytes_to_read = 0;
	ioctlsocket(s, FIONREAD, &bytes_to_read);
	while (bytes_to_read < count)
	{
		ioctlsocket(s, FIONREAD, &bytes_to_read);
	}
}