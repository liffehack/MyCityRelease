#define _CRT_SECURE_NO_WARNINGS

#include "CCamera.h"

#include <fstream>

// скорость движения камеры
const float Mspeed = 30.0;
// скорость вращения камеры
const float Rspeed = 10.0;
// скорость приближения камеры
const float Zspeed = 50.0;

// файл для записи положения камеры
FILE *Config;

// конструктор по умолчанию
CCamera::CCamera()
{
	// флаг существования файла
	bool ExistFile = 1;
	// флаг ошибки при чтении числовой величины из файла
	bool MisRead = 0;
	// строка для чтения из файла
	char Str[20];

	// проверка существования файла
	Config = fopen("config.cfg", "r");
	if (Config == NULL)
		ExistFile = 0;
	// чтение из файла
	if (ExistFile)
	{
		// чтение числовых величин
		Config = fopen("config.cfg", "r");

		// позиция наблююдателя
		fgets(Str, 20, Config);
		Eye.x = atof(Str);
		fgets(Str, 20, Config);
		Eye.y = atof(Str);
		fgets(Str, 20, Config);
		Eye.z = atof(Str);

		// точка наблюдения
		fgets(Str, 20, Config);
		Point.x = atof(Str);
		fgets(Str, 20, Config);
		Point.y = atof(Str);
		if (Point.y) MisRead = 1;
		fgets(Str, 20, Config);
		Point.z = atof(Str);
		fclose(Config);
	}

	// при ошибочном чтении из файла устанавливаются параметры по умолчанию
	if ((MisRead) || (!ExistFile))
	{
		Eye = vec3(20.0, 20.0, 20.0);
		Point = vec3(0.0, 0.0, 0.0);
	}

	// расчет параметров
	Up = vec3(0.0, 1.0, 0.0);
	Radius = sqrt(Eye.x * Eye.x + Eye.y * Eye.y + Eye.z * Eye.z);
	Zenith = degrees(acos(Eye.y / Radius));
	Azimuth = degrees(atan(Eye.z / Eye.x));
	if (Eye.x <= 0)
		Azimuth += 180;
	if ((Eye.x > 0) && (Eye.z < 0))
		Azimuth += 360;

	vec3 temp = Point;
	Point -= temp;
	Eye -= temp;

	Eye.x = Radius * sin(radians(Zenith)) * cos(radians(Azimuth));
	Eye.y = Radius * cos(radians(Zenith));
	Eye.z = Radius * sin(radians(Zenith)) * sin(radians(Azimuth));

	Point += temp;
	Eye += temp;

	ViewMatrix = lookAt(Eye, Point, Up);
	ProjectionMatrix = perspective(radians(35.0), 800.0 / 600.0, 1.0, 100.0);
}

// деструктор 
CCamera::~CCamera()
{
	Config = fopen("config.cfg", "w");
	fprintf(Config, "%f\n", Eye.x);
	fprintf(Config, "%f\n", Eye.y);
	fprintf(Config, "%f\n", Eye.z);
	fprintf(Config, "%f\n", Point.x);
	fprintf(Config, "%f\n", Point.y);
	fprintf(Config, "%f\n", Point.z);
	fclose(Config);
}

// установка матрицы проекции
void CCamera::SetProjectionMatrix(float fovy, float aspect, float zNear, float zFar)
{
	ProjectionMatrix = perspective(radians(fovy), aspect, zNear, zFar);
}

// получение матрицы проекции
mat4 CCamera::GetProjectionMatrix(void)
{
	return ProjectionMatrix;
}

// получить матрицу камеры (наблюдения)
mat4 CCamera::GetViewMatrix(void)
{
	return ViewMatrix;
}

// получить позицию наблюдателя
vec3 CCamera::GetPosition()
{
	return Point;
}

// передвинуть камеру и точку наблюдения в горизонтальной плоскости oXZ
void CCamera::MoveOXZ(float dForward, float dRight, float time)
{
	if ((dForward) || (dRight))
	{
		dForward *= time * Mspeed;
		dRight *= time * Mspeed;

		vec3 Forward = normalize(Eye - Point);
		vec3 Right = normalize(cross(Eye - Point, Up));
		Forward = normalize(cross(Up, Right));

		Forward *= dForward;
		Right *= dRight;

		Point.x += Forward.x + Right.x;
		Point.z += Forward.z + Right.z;
		Eye.x += Forward.x + Right.x;
		Eye.z += Forward.z + Right.z;

		vec3 temp = Point;
		Point -= temp;
		Eye -= temp;

		Azimuth = degrees(atan(Eye.z / Eye.x));
		if (Eye.x <= 0)
			Azimuth += 180;
		if ((Eye.x > 0) && (Eye.z < 0))
			Azimuth += 360;

		Point += temp;
		Eye += temp;
		ViewMatrix = lookAt(Eye, Point, Up);
	}
}

// вращение в горизонтальной и вертикальной плоскости
void CCamera::Rotate(float dHorizAngle, float dVertAngle, float time)
{
	if ((dHorizAngle) || (dVertAngle))
	{
		dHorizAngle *= time * Rspeed;
		dVertAngle *= time * Rspeed;
		float x, z;

		vec3 temp = Point;
		Point -= temp;
		Eye -= temp;

		Azimuth += dHorizAngle;
		x = sqrt(Eye.x * Eye.x + Eye.z * Eye.z)*cos(radians(Azimuth));
		z = sqrt(Eye.x * Eye.x + Eye.z * Eye.z)*sin(radians(Azimuth));
		Eye.x = x;
		Eye.z = z;

		if (Azimuth > 360) Azimuth -= 360;
		if (Azimuth < 0) Azimuth += 360;

		if ((Zenith - dVertAngle > 5) && (Zenith - dVertAngle < 85))
			Zenith -= dVertAngle;

		Eye.x = Radius * sin(radians(Zenith)) * cos(radians(Azimuth));
		Eye.y = Radius * cos(radians(Zenith));
		Eye.z = Radius * sin(radians(Zenith)) * sin(radians(Azimuth));

		Point += temp;
		Eye += temp;
		ViewMatrix = lookAt(Eye, Point, Up);
	}
}

// приближение/удаление
void CCamera::Zoom(float dR, float time)
{
	dR *= time * Zspeed;
	if (Radius + dR > 10)
		Radius += dR;

	vec3 temp = Point;
	Point -= temp;
	Eye -= temp;

	Eye.x = Radius * sin(radians(Zenith)) * cos(radians(Azimuth));
	Eye.y = Radius * cos(radians(Zenith));
	Eye.z = Radius * sin(radians(Zenith)) * sin(radians(Azimuth));

	Point += temp;
	Eye += temp;
	ViewMatrix = lookAt(Eye, Point, Up);
}

// оператор сравнения
bool CCamera::operator==(CCamera &a)
{
	if ((this->Eye == a.Eye) && (this->Point == a.Point) && (this->ProjectionMatrix == a.ProjectionMatrix) && (this->ViewMatrix == a.ViewMatrix))
		return true;
	else
		return false;
}

bool CCamera::operator!=(CCamera &a)
{
	if ((this->Eye == a.Eye) && (this->Point == a.Point) && (this->ProjectionMatrix == a.ProjectionMatrix) && (this->ViewMatrix == a.ViewMatrix))
		return false;
	else
		return true;
}