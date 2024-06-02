#pragma once

#include <GL\glew.h>
#include <string>

class Mesh
{
public:
	Mesh();

	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
	void ClearMesh();

    void SetName(const std::string& name) {
        this->name = name;
    }

    std::string GetName() const {
        return this->name;
    }

	~Mesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;

    std::string name;
};
