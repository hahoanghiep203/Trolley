#include "Light.h"

Light::Light()
{
	colour = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity)
{
	colour = glm::vec3(red, green, blue);
	ambientIntensity = aIntensity;
}

void Light::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation)
{
	glUniform3f(ambientColourLocation, colour.x * 1.0f, colour.y * 1.0f, colour.z * 1.0f);
	glUniform1f(ambientIntensityLocation, ambientIntensity * 1.0f);
}

Light::~Light()
{

}

DirectionalLight::DirectionalLight()
{
	colour = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.0f;
	diffuseIntensity = 1.0f;
	specularIntensity = 1.0f;
	localDirection = glm::vec3(0.0f, 1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity, GLfloat sIntensity)
{
	colour = glm::vec3(red, green, blue);
	ambientIntensity = aIntensity;
	diffuseIntensity = dIntensity;
	specularIntensity = sIntensity;
	localDirection = glm::vec3(0.0, 1.0f, 0.0f);
}

void DirectionalLight::UseDirLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation, GLfloat diffuseIntensityLocation, GLfloat specularIntensityLocation, GLfloat lightDirectionLocation)
{
	glUniform3f(ambientColourLocation, colour.x * 1.5f, colour.y * 1.5f, colour.z * 1.5f);
	glUniform1f(ambientIntensityLocation, ambientIntensity * 1.0f);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform3f(lightDirectionLocation, localDirection.x, localDirection.y, localDirection.y);
}