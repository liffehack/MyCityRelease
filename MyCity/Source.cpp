#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "CScene.h"

// сцена
CScene Scene;

// время симуляции
float Simulation_Time_Passed;

// заголовок окна
char *Title = new char[255];
char *Title1 = new char[50];
char *Title2 = new char[50];
char *Title3 = new char[50];
char *Title4 = new char[50];
char *Title5 = new char[50];
char *Title6 = new char[50];

// счетчик FPS
int FPS = 0;

// функция вызывается при перерисовке окна
// в том числе и принудительно, по командам glutPostRedisplay
void Display(void)
{
	// начинаем вывод нового кадра
	// в том числе очищаем список объектов для вывода, переданный в прошлом кадре
	CRenderManager::Instance().start();

	// выводим всю сцену на экран
	// т.е. передаем используемые камеру, источник света и все объекты
	Scene.draw();

	// завершаем вывод всей сцены
	// выводятся все ранее переданные объекты с учетом переданных настроек,
	// таких как: камера и источник света
	CRenderManager::Instance().finish();

	// подсчет количества объектов
	sprintf(Title2, "%s%i%s ", "[MSAA: x", CRenderManager::Instance().Samples, "]");
	// используемый фильтр изображения
	switch (CRenderManager::Instance().Filter)
	{
		case 3:
			sprintf(Title3, "%s", "[Фильтр: отключен]");
			break;
		case 4:
			sprintf(Title3, "%s", "[Фильтр: черно-белое изображение]");
			break;
		case 5:
			sprintf(Title3, "%s", "[Фильтр: сепия]");
			break;
	}
	// подсчет количества выводимых объектов
	sprintf(Title4, " %s%i%s%i%s", "[Objects: ", CRenderManager::Instance().getObjectCount(), " / ", Scene.GetObjectCount(), "]");
	// используемый механизм оптимизации
	switch (Scene.Optimization)
	{
		case 0:
			sprintf(Title5, "%s", " [Оптимизация: отключена]");
			break;
		case 1:
			sprintf(Title5, "%s", " [Оптимизация: frustrum culling]");
			break;
		case 2:
			sprintf(Title5, "%s", " [Оптимизация: frustrum culling + lod]");
			break;
	}
	// подсчет количества обновлений UBO
	sprintf(Title6, " %s %i%s", "[UBO:", CRenderManager::Instance().UBOUpdateCount, "]");
	// устанавливаем заголовок окна
	memset(Title, char(0), 255);
	strcat(Title, Title1);
	strcat(Title, Title2);
	strcat(Title, Title3);
	strcat(Title, Title4);
	strcat(Title, Title5);
	glutSetWindowTitle(Title);
};

// функция, вызываемая при изменении размеров окна
void Reshape(int w, int h)
{
	// установить новую область просмотра, равную всей области окна
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	// установить матрицу проекции камеры
	Scene.getCamera()->SetProjectionMatrix(45, float(w) / h, 1, 1000);
	CRenderManager::Instance().UpdateFBO(w, h);
};

// функция вызывается когда процессор простаивает, т.е. максимально часто
void Simulation()
{
	static LARGE_INTEGER OldValue, NewValue, Frequency;
	static float Time = 0.0;

	// определение времени, прошедшего с момента предыдущей симуляции
	QueryPerformanceCounter(&NewValue);
	QueryPerformanceFrequency(&Frequency);
	Simulation_Time_Passed = (float)(NewValue.QuadPart - OldValue.QuadPart) / (float)Frequency.QuadPart;
	OldValue = NewValue;
	// симуляция
	if (Simulation_Time_Passed < 100)
	{
		Scene.simulate(Simulation_Time_Passed);
		Time += Simulation_Time_Passed;
		FPS++;
		if (Time > 1.0)
		{
			sprintf(Title1, "%s %i%s ", "Lab5 [FPS:", FPS, "]");
			Time -= 1.0;
			FPS = 0;
		}
	}
	//	устанавливаем признак того, что окно нуждается в перерисовке
	glutPostRedisplay();
};

// функция обработки вращения колесика мышки
void MouseWheel(int wheel, int direction, int x, int y)
{
	float delta = 1.0;
	// определяем на сколько необходимо приблизить/удалить камеру
	if (direction > 0)
		delta = -1.0;
	Scene.getCamera()->Zoom(delta, Simulation_Time_Passed);
}

// функция для обработки событий клавиатуры
void Button(unsigned char key, int X, int Y)
{
	switch (key)
	{
		case 27:
		{
			Scene.getCamera()->~CCamera();
			CResourceManager::Instance().DeleteResources();
			delete[] Title;
			delete[] Title1;
			delete[] Title2;
			delete[] Title3;
			delete[] Title4;
			delete[] Title5;
			delete[] Title6;
			ExitProcess(0);
		} break;

		case'1':
		{
			switch (CRenderManager::Instance().Samples)
			{
				case 8:
					CRenderManager::Instance().Samples = 1;
					break;
				default:
					CRenderManager::Instance().Samples *= 2;
					break;
			}
			CRenderManager::Instance().UpdateFBO(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), CRenderManager::Instance().Samples);
		} break;

		case '2':
		{
			switch (CRenderManager::Instance().Filter)
			{
				case 5:
					CRenderManager::Instance().Filter = 3;
					break;
				default:
					CRenderManager::Instance().Filter++;
					break;
			}
		} break;

		case '3':
		{
			switch (Scene.Optimization)
			{
				case 2:
					Scene.Optimization = 0;
					CRenderManager::Instance().UpdateUBO();
					break;
				default:
					Scene.Optimization++;
					break;
			}
		} break;

		case '4':
		{
			CRenderManager::Instance().changeAABBRenderMode();
		} break;
	}
}

int main(int argc, char **argv)
{
	// инициализация библиотеки GLUT
	glutInit(&argc, argv);
	// инициализация дисплея (формат вывода)
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
	// требования к версии OpenGL (версия 3.3 без поддержки обратной совместимости)
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	// устанавливаем верхний левый угол окна
	glutInitWindowPosition(300, 100);
	// устанавливаем размер окна
	glutInitWindowSize(800, 600);
	// создание окна
	glutCreateWindow("Lab5");
	sprintf(Title1, "%s ", "Lab5");
	memset(Title2, char(0), 100);

	// инициализация GLEW 
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Glew error: %s\n", glewGetErrorString(err));
		return 0;
	}

	// определение текущей версии OpenGL
	printf("OpenGL Version = %s\n\n", glGetString(GL_VERSION));

	// инициализация библиотеки DevIL для загрузки изображений
	ilInit();
	iluInit();
	ilutInit();

	// инициализация сцены
	Scene.init();

	// устанавливаем функцию, которая будет вызываться для перерисовки окна
	glutDisplayFunc(Display);
	// устанавливаем функцию, которая будет вызываться при изменении размеров окна
	glutReshapeFunc(Reshape);
	// устанавливаем функцию которая вызывается всякий раз, когда процессор простаивает
	glutIdleFunc(Simulation);
	// устанавливаем функцию обработки колесика мыши
	glutMouseWheelFunc(MouseWheel);
	// устанавливаем функцию обработки событий клавиатуры
	glutKeyboardFunc(Button);

	// основной цикл обработки сообщений ОС
	glutMainLoop();
	return 0;
};