#include "lab_m2/ProceduralTerrain/ProceduralTerrain.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace ptg;


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

    grassText = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "grassM.jpg");
    cobbleText = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "cobble.png");
    snowText = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "snow.png");

    // Load a mesh from file into GPU memory
    {
        Mesh* mesh = new Mesh("bamboo");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation", "bamboo"), "bamboo.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        vector<VertexFormat> vertices {
            VertexFormat(glm::vec3(0))
        };

        vector<unsigned int> indices = {
            0
        };

        meshes["point"] = new Mesh("point");
        meshes["point"]->InitFromData(vertices, indices);
        meshes["point"]->SetDrawMode(GL_POINTS);
    }

    // Create a shader program for rendering to texture
    {
        Shader *shader = new Shader("Terrain");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("UI");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "UIVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "UIFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("Water");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "WaterVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "ProceduralTerrain", "shaders", "WaterFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    myPerlinNoise = make_unique<PerlinNoise>(112);

    textureWidth = 500;
    textureHeight = 500;

    mapWidth = 30;
    mapHeight = 30;

    minHeight = -10;
    maxHeight = 10;

    heightmap = (double*)calloc(textureWidth * textureHeight, sizeof(double));

    GenText();

    sun_position = glm::vec3(0, 10000, 0);
}

void ProceduralTerrain::GenText()
{
    for (int i = 0; i < textureHeight; i++)
    {
        for (int j = 0; j < textureWidth; j++)
        {
            double x = (double)j / ((double)textureWidth);
            double y = (double)i / ((double)textureWidth);

            double value = myPerlinNoise->noise(5 * x, 5 * y, myZ);

            heightmap[i * textureWidth + j] = value;
        }
    }

    UpdateTexture2D();
}

void ProceduralTerrain::UpdateTexture2D()
{
    if (myTexture != nullptr)
        delete myTexture;

    myTexture = new Texture2D;

    unsigned char* data = (unsigned char*)calloc(3 * textureWidth * textureHeight, sizeof(unsigned char));

    for (int i = 0; i < textureHeight; i++)
    {
        for (int j = 0; j < textureWidth; j++)
        {
            double value = heightmap[i * textureWidth + j];

            int offset = 3 * (i * textureWidth + j);
            unsigned char gray = (int)ceil(value * 255);

            memset(data + offset, gray, 3);

            /*
            if (i < textureHeight / 2)
                memset(data + offset, 0, 3);
            else
                memset(data + offset, 255, 3);
            */
        }
    }

    myTexture->Create(data, textureWidth, textureHeight, 3);
}

void ProceduralTerrain::FrameStart()
{
}


void ProceduralTerrain::Update(float deltaTimeSeconds)
{
    ClearScreen(glm::vec3(0.1, 0.2, 0.3));

    /*
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    */

    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    RenderText();
    RenderTerrain();
    RenderWater(-2, glm::vec4(0.37, 0.81, 0.97, 0.4));
}


void ProceduralTerrain::FrameEnd()
{
    glm::ivec2 res = window->GetResolution();

    glViewport(0, 0, res.x, res.y);
    //DrawCoordinateSystem();
}

void ProceduralTerrain::RenderWater(float height, glm::vec4 color)
{
    glm::ivec2 res = window->GetResolution();
    glViewport(0, 0, res.x, res.y);

    auto shader = shaders["Water"];
    shader->Use();

    glUniform4fv(shader->GetUniformLocation("color"), 1, glm::value_ptr(color));

    glm::mat4 model(1);

    model = glm::translate(model, glm::vec3(0, height, 0));
    model = glm::rotate(model, RADIANS(90), glm::vec3(1, 0, 0));
    model = glm::scale(model, glm::vec3(20));

    RenderMesh(meshes["quad"], shader, model);
}

void ProceduralTerrain::RenderText()
{
    glm::ivec2 res = window->GetResolution();
    glViewport(0, res.y - textOnScreenSize.y, textOnScreenSize.x, textOnScreenSize.y);

    auto shader = shaders["UI"];
    shader->Use();
    glUniform1i(shader->GetUniformLocation("texture_1"), 0);

    myTexture->BindToTextureUnit(GL_TEXTURE0);
    RenderMesh(meshes["quad"], shader, glm::mat4(1));
}

void ProceduralTerrain::RenderTerrain()
{
    glm::ivec2 res = window->GetResolution();
    glViewport(0, 0, res.x, res.y);

    auto shader = shaders["Terrain"];
    shader->Use();

    float textSize = 1.0f / mapWidth;
    glUniform1f(shader->GetUniformLocation("textSize"), textSize);

    myTexture->BindToTextureUnit(GL_TEXTURE1);
    glUniform1i(shader->GetUniformLocation("texture_1"), 1);

    glUniform1i(shader->GetUniformLocation("minHeight"), minHeight);
    glUniform1i(shader->GetUniformLocation("maxHeight"), maxHeight);

    glUniform3fv(shader->GetUniformLocation("sun_position"), 1, glm::value_ptr(sun_position));

    grassText->BindToTextureUnit(GL_TEXTURE2);
    glUniform1i(shader->GetUniformLocation("grass"), 2);

    cobbleText->BindToTextureUnit(GL_TEXTURE3);
    glUniform1i(shader->GetUniformLocation("cobble"), 3);

    snowText->BindToTextureUnit(GL_TEXTURE4);
    glUniform1i(shader->GetUniformLocation("snow"), 4);

    for (int x = -(mapWidth / 2); x < (mapWidth / 2); x++)
    {
        for (int y = -(mapHeight / 2); y < (mapHeight / 2); y++)
        {
            glm::mat4 model(1);
            model = glm::translate(model, glm::vec3(x, 0, y));

            glm::vec2 textPos((float)(x + (mapWidth / 2)) / mapWidth, (float)(y + (mapWidth / 2)) / mapWidth);

            glUniform2fv(shader->GetUniformLocation("textPos"), 1, glm::value_ptr(textPos));

            RenderMesh(meshes["point"], shader, model);
        }
    }
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
    if (key == GLFW_KEY_N)
    {
        myZ -= 0.1f;
        GenText();
    }

    if (key == GLFW_KEY_M)
    {
        myZ += 0.1f;
        GenText();
    }

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
