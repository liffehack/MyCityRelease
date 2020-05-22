#include "CFBO.h"

// конструктор по умолчанию
CFBO::CFBO()
{
	glGenFramebuffers(1, &FBO_Index);
	glGenTextures(1, &Color_tex);
	glGenTextures(1, &Depth_tex);
	Width = 0;
	Height = 0;
	Samples = 1;
}

// инициализация буфера FBO (создание самого буфера, текстуры цвета и текстуры глубины)
void CFBO::init(int w, int h, int samples)
{
	if ((Width != w) || (Height != h) || (Samples != samples))
	{
		glDeleteFramebuffers(1, &FBO_Index);
		glDeleteTextures(1, &Color_tex);
		glDeleteTextures(1, &Depth_tex);

		if (samples == 1)
		{
			glGenFramebuffers(1, &FBO_Index);
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_Index);
			glGenTextures(1, &Color_tex);
			glBindTexture(GL_TEXTURE_2D, Color_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Color_tex, 0);
			glGenTextures(1, &Depth_tex);
			glBindTexture(GL_TEXTURE_2D, Depth_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Depth_tex, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			glGenFramebuffers(1, &FBO_Index);
			glBindFramebuffer(GL_FRAMEBUFFER, FBO_Index);
			glGenTextures(1, &Color_tex);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, Color_tex);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, w, h, GL_TRUE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, Color_tex, 0);
			glGenTextures(1, &Depth_tex);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, Depth_tex);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH_COMPONENT, w, h, GL_TRUE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, Depth_tex, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		Samples = samples;
		Width = w;
		Height = h;
	}
}

// выбор буфера FBO в качестве текущего для рендеринга
void CFBO::bind(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_Index);
}

// возврат к буферу кадра по умолчанию (для вывода на экран)
void CFBO::unbind(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// копирование данного буфера FBO в другой
void CFBO::resolveToFbo(CFBO& fbo)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_Index);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.FBO_Index);
	glBlitFramebuffer(0, 0, Width, Height, 0, 0, fbo.Width, fbo.Height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBlitFramebuffer(0, 0, Width, Height, 0, 0, fbo.Width, fbo.Height, GL_DEPTH_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

// привязать текстуру цвета к нужному текстурному блоку
void CFBO::bindColorTexture(GLenum TexUnit)
{
	glActiveTexture(TexUnit);
	glBindTexture(GL_TEXTURE_2D, Color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// привязать текстуру глубины к нужному текстурному блоку
void CFBO::bindDepthTexture(GLenum TexUnit)
{
	glActiveTexture(TexUnit);
	glBindTexture(GL_TEXTURE_2D, Depth_tex);
}