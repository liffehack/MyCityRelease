#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

// КЛАСС ДЛЯ РАБОТЫ С БУФЕРОМ FBO
class CFBO
{
	// индекс буфера FBO
	GLuint FBO_Index;
	// текстурный объект для цвета
	GLuint Color_tex;
	// текстурный объект для глубины
	GLuint Depth_tex;
	// ширина и высота буфера FBO
	int Width;
	int Height;
	// количество выборок в текстуре (1 ‐ обычная текстура, без мультисэмплинга)
	int Samples;
public:
	// конструктор по умолчанию
	CFBO(void);
	// инициализация буфера FBO (создание самого буфера, текстуры цвета и текстуры глубины)
	void init(int w, int h, int samples = 1);
	// выбор буфера FBO в качестве текущего для рендеринга
	void bind(void);
	// возврат к буферу кадра по умолчанию (для вывода на экран)
	static void unbind(void);
	// копирование данного буфера FBO в другой
	void resolveToFbo(CFBO& fbo);
	// привязать текстуру цвета к нужному текстурному блоку
	void bindColorTexture(GLenum TexUnit = GL_TEXTURE0);
	// привязать текстуру глубины к нужному текстурному блоку
	void bindDepthTexture(GLenum TexUnit = GL_TEXTURE1);
};