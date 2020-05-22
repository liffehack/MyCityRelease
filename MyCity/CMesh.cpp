#define _CRT_SECURE_NO_WARNINGS

#include "CMesh.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace glm;
using namespace std;

// конструктор
CMesh::CMesh()
{
	VBO_Index = -1;
	VAO_Index = -1;
	VertexCount = 0;
}

// загрузка меша из внешнего obj‐файла
int CMesh::Load(const char* filename)
{
	FILE *Mesh = fopen(filename, "r");
	if (Mesh)
	{
		// структура для представления одной вершины со всеми её атрибутами
		struct Vertex
		{
			// геометрические координаты вершины
			GLfloat vPosition[3];
			// нормали к поверхности в данной точке
			GLfloat vNormal[3];
			// текстурные координаты вершины
			GLfloat vTexCoord[2];
		};
		// вектор для хранения геометрических координат
		vector<vec3> VCoord;
		// вектор для хранения текстурных координат
		vector<vec2> TCoord;
		// вектор для хранения нормалей
		vector<vec3> NCoord;
		// структура для представления одного полигона
		struct Face
		{
			// индексы для геометрических координат 
			// текстурных координат, и вектора нормали 0‐ой вершины
			int v0, vt0, vn0;
			// индексы для геометрических координат 
			// текстурных координат, и вектора нормали 1‐ой вершины
			int v1, vt1, vn1;
			// индексы для геометрических координат 
			// текстурных координат, и вектора нормали 2‐ой вершины
			int v2, vt2, vn2;
		};
		// вектор для хранения информации о полигонах
		vector<Face> Faces;
		// строка для чтения из файла
		char* Str = new char[255];

		while (!feof(Mesh))
		{
			fscanf(Mesh, "%s", Str);

			if (!strcmp(Str, "v"))
			{
				// чтение одного комплекта геометрических координат вершины
				float x, y, z;
				fscanf(Mesh, "%f %f %f", &x, &y, &z);
				// помещение геометрических координат в контейнер
				VCoord.push_back(vec3(x, y, z));
				fgets(Str, 255, Mesh);
			}

			else
				if (!strcmp(Str, "vt"))
			{
				// чтение одного комплекта текстурных координат вершины
				float s, t;
				fscanf(Mesh, "%f %f", &s, &t);
				// помещение текстурных координат в контейнер
				TCoord.push_back(vec2(s, t));
				fgets(Str, 255, Mesh);
			}

			else
				if (!strcmp(Str, "vn"))
			{
				// чтение одного вектора нормали вершины
				float x, y, z;
				fscanf(Mesh, "%f %f %f", &x, &y, &z);
				// помещение вектора нормали в контейнер
				NCoord.push_back(vec3(x, y, z));
				fgets(Str, 255, Mesh);
			}

			else
				if (!strcmp(Str, "f"))
			{
				// чтение информации об одном полигоне
				Face face;
				fscanf(Mesh, "%i/%i/%i", &face.v0, &face.vt0, &face.vn0);
				fscanf(Mesh, "%i/%i/%i", &face.v1, &face.vt1, &face.vn1);
				fscanf(Mesh, "%i/%i/%i", &face.v2, &face.vt2, &face.vn2);
				Faces.push_back(face);
				fgets(Str, 255, Mesh);
			}

			else
				fgets(Str, 255, Mesh);
		}

		delete[] Str;

		VertexCount = Faces.size() * 3;
		Vertex *Vertexes = new Vertex[VertexCount];

		for (unsigned int i = 0; i < Faces.size(); i++)
		{
			// геометрические координаты
			Vertexes[i * 3].vPosition[0] = VCoord[Faces[i].v0 - 1].x;
			Vertexes[i * 3].vPosition[1] = VCoord[Faces[i].v0 - 1].y;
			Vertexes[i * 3].vPosition[2] = VCoord[Faces[i].v0 - 1].z;
			Vertexes[i * 3 + 1].vPosition[0] = VCoord[Faces[i].v1 - 1].x;
			Vertexes[i * 3 + 1].vPosition[1] = VCoord[Faces[i].v1 - 1].y;
			Vertexes[i * 3 + 1].vPosition[2] = VCoord[Faces[i].v1 - 1].z;
			Vertexes[i * 3 + 2].vPosition[0] = VCoord[Faces[i].v2 - 1].x;
			Vertexes[i * 3 + 2].vPosition[1] = VCoord[Faces[i].v2 - 1].y;
			Vertexes[i * 3 + 2].vPosition[2] = VCoord[Faces[i].v2 - 1].z;
			// нормали
			Vertexes[i * 3].vNormal[0] = NCoord[Faces[i].vn0 - 1].x;
			Vertexes[i * 3].vNormal[1] = NCoord[Faces[i].vn0 - 1].y;
			Vertexes[i * 3].vNormal[2] = NCoord[Faces[i].vn0 - 1].z;
			Vertexes[i * 3 + 1].vNormal[0] = NCoord[Faces[i].vn1 - 1].x;
			Vertexes[i * 3 + 1].vNormal[1] = NCoord[Faces[i].vn1 - 1].y;
			Vertexes[i * 3 + 1].vNormal[2] = NCoord[Faces[i].vn1 - 1].z;
			Vertexes[i * 3 + 2].vNormal[0] = NCoord[Faces[i].vn2 - 1].x;
			Vertexes[i * 3 + 2].vNormal[1] = NCoord[Faces[i].vn2 - 1].y;
			Vertexes[i * 3 + 2].vNormal[2] = NCoord[Faces[i].vn2 - 1].z;
			// текстурные координаты
			Vertexes[i * 3].vTexCoord[0] = TCoord[Faces[i].vt0 - 1].x;
			Vertexes[i * 3].vTexCoord[1] = TCoord[Faces[i].vt0 - 1].y;
			Vertexes[i * 3 + 1].vTexCoord[0] = TCoord[Faces[i].vt1 - 1].x;
			Vertexes[i * 3 + 1].vTexCoord[1] = TCoord[Faces[i].vt1 - 1].y;
			Vertexes[i * 3 + 2].vTexCoord[0] = TCoord[Faces[i].vt2 - 1].x;
			Vertexes[i * 3 + 2].vTexCoord[1] = TCoord[Faces[i].vt2 - 1].y;
		}

		glGenBuffers(1, &VBO_Index);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_Index);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*VertexCount, Vertexes, GL_STATIC_DRAW);
		glGenVertexArrays(1, &VAO_Index);
		glBindVertexArray(VAO_Index);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vPosition));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vNormal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vTexCoord));
		glBindVertexArray(0);
		return VAO_Index;
	}
	else
	{
		cout << "File " << filename << " not found" << endl;
		return -1;
	}
}

// вывод модели (используется текущий шейдер)
void CMesh::Render()
{
	if (VAO_Index == -1) return;
	glBindVertexArray(VAO_Index);
	glDrawArrays(GL_TRIANGLES, 0, VertexCount);
}