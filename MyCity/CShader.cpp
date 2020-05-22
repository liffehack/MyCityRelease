#define _CRT_SECURE_NO_WARNINGS

#include "CShader.h"
#include <iostream>


// получить номер шейдерной программы
GLuint CShader::GetShaderProgram()
{
	return Program;
}

// загрузить вершинный шейдер
int CShader::LoadVertexShader(const char *VertexFileName, bool DebugOutput)
{
	// файл с текстом шейдера
	FILE *File;
	// строка с текстом шейдера
	char **String = new char*[1];
	// длина строки с текстом шейдера
	int Size;
	// чтение из файла
	File = fopen(VertexFileName, "rb");
	if (DebugOutput)
		cout << endl << "Loading vertex shader \"" << VertexFileName << "\" ... ";
	if (!File)
	{
		delete[] String;
		if (DebugOutput)
			cout << "Vertex shader not found" << endl << endl;
		return -1;
	}
	else
	{
		fseek(File, 0, SEEK_END);
		Size = ftell(File);
		rewind(File);
		String[0] = new char[Size + 1];
		fread(String[0], 1, Size, File);
		String[0][Size] = (char)0;
	}
	// создание вершинного шейдера
	Vsh = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(Vsh, 1, String, 0);
	glCompileShader(Vsh);
	int param = 0;
	// проверка статуса шейдера
	glGetShaderiv(Vsh, GL_COMPILE_STATUS, &param);
	if (param && DebugOutput)
	{
		cout << "OK" << endl;
		cout << "File size: " << Size << " bytes" << endl;
		cout << "================================================================================";
		cout << String[0] << endl;
		cout << "================================================================================";
	}
	else
	{
		if (DebugOutput)
		{
			cout << "Failed" << endl;
			glGetShaderiv(Fsh, GL_INFO_LOG_LENGTH, &param);
			char *Log = new char[param];
			glGetShaderInfoLog(Fsh, param, &param, Log);
			cout << Log << endl;
			delete[] Log;
		}
	}
	if(DebugOutput)
		cout << endl;
	delete[] String[0];
	delete[] String;
	fclose(File);
	return Vsh;
}

// загрузить фрагментный шейдер
int CShader::LoadFragmentShader(const char *FragmentFileName, bool DebugOutput)
{
	// файл с текстом шейдера
	FILE *File;
	// строка с текстом шейдера
	char **String = new char*[1];
	// длина строки с текстом шейдера
	int Size;
	File = fopen(FragmentFileName, "rb");
	if (DebugOutput)
		cout << endl << "Loading fragment shader \"" << FragmentFileName << "\" ... ";
	if (!File)
	{
		delete[] String;
		if (DebugOutput)
			cout << "Fragment shader not found" << endl << endl;
		return -1;
	}
	else
	{
		fseek(File, 0, SEEK_END);
		Size = ftell(File);
		rewind(File);
		String[0] = new char[Size + 1];
		fread(String[0], 1, Size, File);
		String[0][Size] = (char)0;
	}
	// создание фрагментного шейдера
	Fsh = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Fsh, 1, String, 0);
	glCompileShader(Fsh);
	int param = 0;
	// проверка статуса шейдера
	glGetShaderiv(Fsh, GL_COMPILE_STATUS, &param);
	if (param && DebugOutput)
	{
		cout << "OK" << endl;
		cout << "File size: " << Size << " bytes" << endl;
		cout << "================================================================================";
		cout << String[0] << endl;
		cout << "================================================================================";
	}
	else
	{
		if (DebugOutput)
		{
			cout << "Failed" << endl;
			glGetShaderiv(Fsh, GL_INFO_LOG_LENGTH, &param);
			char *Log = new char[param];
			glGetShaderInfoLog(Fsh, param, &param, Log);
			cout << Log << endl;
			delete[] Log;
		}
	}
	if (DebugOutput)
		cout << endl;
	delete[] String[0];
	delete[] String;
	fclose(File);
	return Fsh;
}

// слинковать шейдерную программу
int CShader::Link(bool DebugOutput)
{
	if (DebugOutput)
		cout << "Linking shader program ... ";
	// создание шейдерной программы
	Program = glCreateProgram();
	glAttachShader(Program, Vsh);
	glAttachShader(Program, Fsh);
	glLinkProgram(Program);
	int param = 0;
	// проверка статуса программы
	glGetProgramiv(Program, GL_LINK_STATUS, &param);
	if (param && DebugOutput)
		cout << "OK" << endl;
	else
	{
		if (DebugOutput)
		{
			cout << "Failed" << endl;
			glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &param);
			char *Log = new char[param];
			glGetProgramInfoLog(Program, param, &param, Log);
			cout << Log << endl;
			delete[] Log;
		}
	}
	return Program;
}

//	активизировать шейдер (сделать текущим)
void CShader::Activate()
{
	glUseProgram(Program);
}

//	отключить шейдер (использовать нулевую шейдерную программу)
void CShader::Deactivate()
{
	glUseProgram(0);
}

// получение индекса атрибут-переменной
int CShader::GetAttribLocation(const char *name)
{
	return glGetAttribLocation(Program, name);
};

// запись вектора из 4-х комопнент в uniform-переменную
void CShader::SetUniformVec4(const char *name, vec4 value)
{
	int k = -1;
	auto it = Uniform.find(name);
	if (it != Uniform.end())
		k = it->second;
	else
	{
		k = glGetUniformLocation(Program, name);
		Uniform.insert(pair<const char*, int>(name, k));
	}
	glUniform4fv(k, 1, value_ptr(value));
}

// запись матрицы 4х4 в uniform‐переменную
void CShader::SetUniformMat4(const char *name, mat4 value)
{
	int k = -1;
	auto it = Uniform.find(name);
	if (it != Uniform.end())
		k = it->second;
	else
	{
		k = glGetUniformLocation(Program, name);
		Uniform.insert(pair<const char*, int>(name, k));
	}
	glUniformMatrix4fv(k, 1, 0, value_ptr(value));
}

// запись вещественного значения в uniform‐переменную
void CShader::SetUniformFloat(const char *name, float value)
{
	int k = -1;
	auto it = Uniform.find(name);
	if (it != Uniform.end())
		k = it->second;
	else
	{
		k = glGetUniformLocation(Program, name);
		Uniform.insert(pair<const char*, int>(name, k));
	}
	glUniform1f(k, value);
}