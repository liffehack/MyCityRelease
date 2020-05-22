#define _CRT_SECURE_NO_WARNINGS

#include "CCamera.h"

#include <fstream>

// �������� �������� ������
const float Mspeed = 30.0;
// �������� �������� ������
const float Rspeed = 10.0;
// �������� ����������� ������
const float Zspeed = 50.0;

// ���� ��� ������ ��������� ������
FILE *Config;

//�����������������������
CCamera::CCamera()
{
	// ���� ������������� �����
	bool ExistFile = 1;
	// ���� ������ ��� ������ �������� �������� �� �����
	bool MisRead = 0;
	// ������ ��� ������ �� �����
	char Str[20];

	// �������� ������������� �����
	Config = fopen("config.cfg", "r");
	if (Config == NULL)
		ExistFile = 0;
	// ������ �� �����
	if (ExistFile)
	{
		// ������ �������� �������
		Config = fopen("config.cfg", "r");

		// ������� ������������
		fgets(Str, 20, Config);
		Eye.x = atof(Str);
		fgets(Str, 20, Config);
		Eye.y = atof(Str);
		fgets(Str, 20, Config);
		Eye.z = atof(Str);

		// ����� ����������
		fgets(Str, 20, Config);
		Point.x = atof(Str);
		fgets(Str, 20, Config);
		Point.y = atof(Str);
		if (Point.y) MisRead = 1;
		fgets(Str, 20, Config);
		Point.z = atof(Str);
		fclose(Config);
	}

	// ��� ��������� ������ �� ����� ��������������� ��������� �� ���������
	if ((MisRead) || (!ExistFile))
	{
		Eye = vec3(20.0, 20.0, 20.0);
		Point = vec3(0.0, 0.0, 0.0);
	}

	// ������ ����������
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

//�����������
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

//��������������������������
void CCamera::SetProjectionMatrix(float fovy, float aspect, float zNear, float zFar)
{
	ProjectionMatrix = perspective(radians(fovy), aspect, zNear, zFar);
}

//��������������������������
mat4 CCamera::GetProjectionMatrix(void)
{
	return ProjectionMatrix;
}

//������������������������(����������)
mat4 CCamera::GetViewMatrix(void)
{
	return ViewMatrix;
}

// �������� ������� �����������
vec3 CCamera::GetPosition()
{
	return Point;
}

//������������������������������������������������������������oXZ
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

//����������������������������������������������
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

//������������/��������
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

//������������������
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