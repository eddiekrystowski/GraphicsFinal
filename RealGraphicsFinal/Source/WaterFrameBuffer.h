#pragma once

#include <glad/glad.h>
#include "Texture.h"
#include "Window.h"

class WaterFrameBuffer {
private:
	static const int reflectionWidth, reflectionHeight;
	static const int refractionWidth, refractionHeight;

	GLuint reflectionFrameBuffer, reflectionDepthBuffer;
	GLuint refractionFrameBuffer, refractionDepthTexture;
	unsigned int reflectionTexture, refractionTexture;

	GLuint CreateFrameBuffer();
	GLuint CreateDepthTextureAttachment(int width, int height);
	GLuint CreateDepthBufferAttachment(int width, int height);
	void BindFrameBuffer(GLuint buffer, int width, int height);

public:
	WaterFrameBuffer();
	~WaterFrameBuffer();

	void Clear();
	void BindReflectionBuffer();
	void BindRefractionBuffer();
	void UnbindBuffer();
	unsigned int GetReflectionTexture();
	unsigned int GetRefractionTexture();
	unsigned int GetRefractionDepthTexture();
};