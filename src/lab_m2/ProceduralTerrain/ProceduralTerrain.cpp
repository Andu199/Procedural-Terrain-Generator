#include "lab_m2/ProceduralTerrain/ProceduralTerrain.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m2;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


ProceduralTerrain::ProceduralTerrain()
{
}


ProceduralTerrain::~ProceduralTerrain()
{
}


void ProceduralTerrain::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 5, 4), glm::quat(glm::vec3(-30 * TO_RADIANS, 0, 0)));
    camera->Update();

    // Load a mesh from file into GPU memory
    {
        Mesh* mesh = new Mesh("bamboo");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation", "bamboo"), "bamboo.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for rendering to texture
    {
        Shader *shader = new Shader("Instances");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}


void ProceduralTerrain::FrameStart()
{
}


void ProceduralTerrain::Update(float deltaTimeSeconds)
{
    ClearScreen();

    {
        auto shader = shaders["Instances"];

        shader->Use();

        RenderMesh(meshes["bamboo"], shaders["Instances"], glm::vec3(-3.3f, 0.5f, 0), glm::vec3(0.1f));
    }
}


void ProceduralTerrain::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void ProceduralTerrain::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input with window->KeyHold()
}


void ProceduralTerrain::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void ProceduralTerrain::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void ProceduralTerrain::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void ProceduralTerrain::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void ProceduralTerrain::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void ProceduralTerrain::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void ProceduralTerrain::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
