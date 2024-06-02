#pragma once

#include <GL\glew.h>

#include "stb_image.h"
#include <string>

class Texture
{
public:
	Texture();
	Texture(std::string fileLoc);

	void LoadTexture();
	void UseTexture();
	void ClearTexture();

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	std::string fileLocation;
};

