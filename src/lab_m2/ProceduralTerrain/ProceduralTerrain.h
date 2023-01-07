#pragma once

#include "components/simple_scene.h"
#include "PerlinNoise.h"

namespace ptg
{
    class ProceduralTerrain : public gfxc::SimpleScene
    {
     public:
         ProceduralTerrain();
        ~ProceduralTerrain();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void GenText();
        void UpdateTexture2D();
        void RenderWater(float height, glm::vec4 color);
        void RenderText();
        void RenderTerrain();

     private:
         std::unique_ptr<PerlinNoise> myPerlinNoise;

         int textureWidth, textureHeight, mapWidth, mapHeight;
         int minHeight, maxHeight;

         double *heightmap;
         Texture2D *myTexture{nullptr};

         Texture2D* grassText, * cobbleText, * snowText;

         glm::ivec2 textOnScreenSize {glm::ivec2(300, 300)};

         float myZ{ 0.8 };

         glm::vec3 sun_position;
    };
}   // namespace m2
