#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

float trainPosition = 0.0f;
float wheelRotation = 0.0f;
const float toRadians = 3.14159265f / 180.0f;
bool run_animation = false;
int animation_scene = 0; // 0: The trolley turn, 1: the trolley moves straight  

Window mainWindow;
std::vector<Mesh*> plane_mesh, trolley_mesh, rail_mesh[3], wheel_mesh[6], human_mesh[6], rope_mesh;
std::vector<Shader> shaderList;
Camera camera;

Texture dirt, trolley, rail, human, rope;

DirectionalLight dLight(1.0f, 1.0f, 1.0f, 0.5f, 0.8f, 1.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        run_animation = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        camera.Unlock();
    }
}


void LoadMesh(aiMesh* mesh, const aiScene* scene, std::vector<Mesh*>& meshList) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
        if (mesh->mTextureCoords[0]) {
            vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        }
        else {
            vertices.insert(vertices.end(), { 0.0f, 0.0f });
        }
        // Check if mNormals exists before trying to access it
        if (mesh->mNormals) {
            vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
        }
        else {
            vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f }); // Insert default normal values
        }
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    Mesh* newMesh = new Mesh();
    newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
    meshList.push_back(newMesh);
}

void LoadNode(aiNode* node, const aiScene* scene, std::vector<Mesh*>& meshList) {
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        LoadMesh(scene->mMeshes[node->mMeshes[i]], scene, meshList);
    }

    for (size_t i = 0; i < node->mNumChildren; i++) {
        LoadNode(node->mChildren[i], scene, meshList);
    }
}

void LoadModel(const std::string& filePath, std::vector<Mesh*>& meshList) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
    if (!scene) {
        printf("Model (%s) failed to load: %s", filePath, importer.GetErrorString());
        return;
    }

    LoadNode(scene->mRootNode, scene, meshList);
}

void CreateObjects() {
    unsigned int indices0[] = {
        0, 1, 2,
        0, 2, 3,
    };

    GLfloat vertices0[] = {
        // x      y      z      u     v     nx    ny    nz
        -100.0f,  0.0f, -100.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -100.0f,  0.0f, 100.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        100.0f,   0.0f, 100.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        100.0f,   0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices0, indices0, 32, 6);
    plane_mesh.push_back(obj1);
}

void CreateShaders() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}


int main() {
    mainWindow = Window(800, 600);
    mainWindow.Initialise();

    glfwSetInputMode(mainWindow.getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mainWindow.getGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glfwSetMouseButtonCallback(mainWindow.getGLFWWindow(), mouseButtonCallback);

    CreateObjects();
    CreateShaders();

    LoadModel("D:/Docs/Visual Studio/Trolley Problem/OpenGLCourseApp/Obj/trolley_body.obj", trolley_mesh);    
    for (int i = 0; i < 6; i++) {
        std::string filePath = "D:/Docs/Visual Studio/Trolley Problem/OpenGLCourseApp/Obj/wheel" + std::to_string(i + 1) + ".obj";
        LoadModel(filePath, wheel_mesh[i]);
    }
    for (int i = 0; i < 3; i++) {
        std::string filePath = "D:/Docs/Visual Studio/Trolley Problem/OpenGLCourseApp/Obj/rail" + std::to_string(i + 1) + ".obj";
        LoadModel(filePath, rail_mesh[i]);
    }
    LoadModel("D:/Docs/Visual Studio/Trolley Problem/OpenGLCourseApp/Obj/human1.obj", human_mesh[0]);
    LoadModel("D:/Docs/Visual Studio/Trolley Problem/OpenGLCourseApp/Obj/rope1.obj", rope_mesh);

    // camera = Camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 5.0f, 0.2f);
    camera = Camera(glm::vec3(-30.0f, 30.0f, 100.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, -30.0f, 5.0f, 0.2f);

    dirt = Texture("Textures/dirt.jpg");
    trolley = Texture("Textures/trolley.jpg");
    rail = Texture("Textures/rail.jpg");
    human = Texture("Textures/human.jpg");
    rope = Texture("Textures/rope.jpg");

    dirt.LoadTexture();
    trolley.LoadTexture();
    rail.LoadTexture();
    human.LoadTexture();
    rope.LoadTexture();

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformAmbientIntensity = 0,
        uniformAmbientColour = 0, uniformDiffuseIntensity = 0, uniformSpecularIntensity = 0, uniformLightDirection = 0;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    // Loop until window closed
    while (!mainWindow.getShouldClose()) {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        // Get + Handle User Input
        glfwPollEvents();

        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Clear the window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create ImGui window
        ImGui::Begin("Trolley Problem");
        // ImGui::Text("There's a runaway trolley headed towards five people tied up on one track.\nYou're standing next to a lever that can divert the trolley to another track,\nbut there's one person tied up there.\nYou must decide whether to pull the lever, sacrificing one person to save five,\nor do nothing and let the trolley continue, leading to the death of the five people:");

        if (ImGui::Button("Scene 0")) {
            animation_scene = 0;
            run_animation = true;
        }
        if (ImGui::Button("Scene 1")) {
            animation_scene = 1;
            run_animation = true;
        }
        if (ImGui::Button("Scene 2")) {
            animation_scene = 2;
            run_animation = true;
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
        uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
        uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();
        uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
        uniformLightDirection = shaderList[0].GetLightDirectionLocation();

        dLight.UseDirLight(uniformAmbientIntensity, uniformAmbientColour,
            uniformDiffuseIntensity, uniformSpecularIntensity, uniformLightDirection);

        glm::mat4 model(1.0f);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        // Animation
        if (run_animation) {
            wheelRotation += 300.0f * deltaTime; // Rotate the wheel at 100 degrees per second
            if (wheelRotation > 360.0f) {
                wheelRotation -= 360.0f; // Reset the angle to prevent overflow
            }

            trainPosition += 10.0f * deltaTime;
        }

        // Grass Plane
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dirt.UseTexture();
        plane_mesh[0]->RenderMesh();

        // Trolley
        for (size_t i = 0; i < trolley_mesh.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            switch (animation_scene) {
            case 0:
                // First movement scenario
                if (trainPosition < 60.0f) {
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, trainPosition));
                } else {
                    float diagonalPosition = trainPosition - 60.0f;
                    model = glm::translate(model, glm::vec3(diagonalPosition, 0.0f, 60.0f + diagonalPosition));
                    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
                break;
            case 1:
                // Second movement scenario
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, trainPosition));
                break;
            case 2:
                // Third movement scenario
                if (trainPosition < 25.0f) {
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, trainPosition));
                } else {
                    float upwardPosition = trainPosition - 25.0f;
                    model = glm::translate(model, glm::vec3(0.0f, upwardPosition * glm::tan(glm::radians(30.0f)), 25.0f + upwardPosition));
                    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                }
                break;
            }
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            trolley.UseTexture();
            trolley_mesh[i]->RenderMesh();
        }

        // Wheels, from blender x y z to opengl: x -> 0, -z -> y, y -> z
        glm::vec3 wheelCenters[] = {
            glm::vec3(0.0f, -1.98191f, 3.6229f),
            glm::vec3(0.0f, -1.78191f, 0.056396f),
            glm::vec3(0.0f, -1.78191f, -3.2106f),
            glm::vec3(0.0f, -1.78191f, -3.2106f),
            glm::vec3(0.0f, -1.78191f, 0.056396f),
            glm::vec3(0.0f, -1.98191f, 3.6229f),
        };

        // Wheels
        for (int j = 0; j < 6; j++) {
            for (size_t i = 0; i < wheel_mesh[j].size(); i++) {
                model = glm::mat4(1.0f);
                switch (animation_scene) {
                case 0: // First movement scenario
                    if (trainPosition < 60.0f) {  // Move straight initially
                        model = glm::translate(model, glm::vec3(0.0f, 0.0f, trainPosition));
                    }
                    else {  // Move diagonally for a distance
                        float diagonalPosition = trainPosition - 60.0f;
                        model = glm::translate(model, glm::vec3(diagonalPosition, 0.0f, 60.0f + diagonalPosition));
                        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    }
                    break;
                case 1: // Second movement scenario
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, trainPosition));
                    break;
                case 2:
                    if (trainPosition < 25.0f) {  // Move straight initially
                        model = glm::translate(model, glm::vec3(0.0f, 0.0f, trainPosition));
                    }
                    else {  // Move upwards at a 30-degree angle
                        float upwardPosition = trainPosition - 25.0f;
                        model = glm::translate(model, glm::vec3(0.0f, upwardPosition * glm::tan(glm::radians(30.0f)), 25.0f + upwardPosition));
                        model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Add this line
                    }
                    break;
                }
                model = glm::translate(model, -wheelCenters[j]);
                model = glm::rotate(model, glm::radians(wheelRotation), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::translate(model, wheelCenters[j]);
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                trolley.UseTexture();
                wheel_mesh[j][i]->RenderMesh();
            }
        }

        // Rail
        switch (animation_scene) {
        case 0:
        case 1:
            for (int j = 0; j < 3; j++) {
                for (size_t i = 0; i < rail_mesh[j].size(); i++) {
                    model = glm::mat4(1.0f);
                    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
                    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                    rail.UseTexture();
                    rail_mesh[j][i]->RenderMesh();
                }
            }
            break;
        case 2:
            for (int j = 0; j < 2; j++) {
                for (size_t i = 0; i < rail_mesh[j].size(); i++) {
                    model = glm::mat4(1.0f);
                    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
                    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                    rail.UseTexture();
                    rail_mesh[j][i]->RenderMesh();
                }
            }
            for (size_t i = 0; i < rail_mesh[2].size(); i++) {
                model = glm::mat4(1.0f);
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
                if (animation_scene == 2) {
                    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    model = glm::translate(model, glm::vec3(0.0f, -12.5f, -5.0f));
                }
                // ... (some code for positioning the model)
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                rail.UseTexture();
                rail_mesh[2][i]->RenderMesh();
            }
            break;
        }

        // Human
        for (size_t i = 0; i < human_mesh[0].size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            human.UseTexture();
            human_mesh[0][i]->RenderMesh();
        }

        //Rope
        for (size_t i = 0; i < rope_mesh.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            rope.UseTexture();
            rope_mesh[i]->RenderMesh();
        }

        glUseProgram(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        mainWindow.swapBuffers();
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}