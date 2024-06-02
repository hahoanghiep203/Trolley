#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <string>
#include <vector>

struct Vertex {
    float x, y, z;
};

struct Normal {
    float nx, ny, nz;
};

struct Face {
    int v1, v2, v3;
    int n1, n2, n3;
};

class OBJLoader {
public:
    OBJLoader();
    ~OBJLoader();

    bool LoadOBJ(const std::string& filename);

    const std::vector<Vertex>& GetVertices() const;
    const std::vector<Normal>& GetNormals() const;
    const std::vector<Face>& GetFaces() const;

private:
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<Face> faces;
};

#endif // OBJLOADER_H
