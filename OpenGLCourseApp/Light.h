#pragma once

#include <GL\glew.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation);
	float* getAmbientIntensity() { return &ambientIntensity; };
	~Light();

protected:
	glm::vec3 colour;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;
	GLfloat specularIntensity;
};

class DirectionalLight : public Light
{

public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity, GLfloat sIntensity);

	glm::vec3 localDirection;
	void UseDirLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation, GLfloat diffuseIntensityLocation, GLfloat specularIntensityLocation, GLfloat lightDirectionLocation);
	void print();
};