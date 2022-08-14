#include "WaterFrameBuffer.h"
#include <iostream>

const int WaterFrameBuffer::reflectionWidth = 1024;
const int WaterFrameBuffer::reflectionHeight = 768;
const int WaterFrameBuffer::refractionWidth = 1280;
const int WaterFrameBuffer::refractionHeight = 720;

GLenum glCheckError2_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError2() glCheckError2_(__FILE__, __LINE__) 


unsigned int CreateTextureAttachment(int width, int height) {
	unsigned int texture;


	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	return texture;
}


WaterFrameBuffer::WaterFrameBuffer() {
	reflectionFrameBuffer = CreateFrameBuffer();
	reflectionTexture = CreateTextureAttachment(reflectionWidth, reflectionHeight);
	reflectionDepthBuffer = CreateDepthBufferAttachment(reflectionWidth, reflectionHeight);
	UnbindBuffer();

	refractionFrameBuffer = CreateFrameBuffer();
	refractionTexture = CreateTextureAttachment(refractionWidth, refractionHeight);
	refractionDepthTexture = CreateDepthTextureAttachment(refractionWidth, refractionHeight);
	UnbindBuffer();
}

WaterFrameBuffer::~WaterFrameBuffer() {
	glDeleteFramebuffers(1, &reflectionFrameBuffer);
	glDeleteFramebuffers(1, &refractionFrameBuffer);
	glDeleteRenderbuffers(1, &reflectionDepthBuffer);
	glDeleteTextures(1, &refractionDepthTexture);

}

unsigned int WaterFrameBuffer::GetReflectionTexture() {
	return reflectionTexture;
}
unsigned int WaterFrameBuffer::GetRefractionTexture() {
	return refractionTexture;
}
unsigned int  WaterFrameBuffer::GetRefractionDepthTexture() {
	return refractionDepthTexture;
}
void WaterFrameBuffer::Clear() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
void WaterFrameBuffer::BindReflectionBuffer() {
	BindFrameBuffer(reflectionFrameBuffer, reflectionWidth, reflectionHeight);
}
void WaterFrameBuffer::BindRefractionBuffer() {
	BindFrameBuffer(refractionFrameBuffer, refractionWidth, refractionHeight);
}
void WaterFrameBuffer::UnbindBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::Width, Window::Height);
}

void WaterFrameBuffer::BindFrameBuffer(GLuint buffer, int width, int height) {
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);
	glViewport(0, 0, width, height);
}

unsigned int WaterFrameBuffer::CreateFrameBuffer() {
	unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	return frameBuffer;
}

GLuint WaterFrameBuffer::CreateDepthTextureAttachment(int width, int height) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

	return texture;
}
GLuint WaterFrameBuffer::CreateDepthBufferAttachment(int width, int height) {
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	return depthBuffer;
}

