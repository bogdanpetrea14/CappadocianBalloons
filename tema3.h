#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"

namespace m1
{
    class Tema3 : public gfxc::SimpleScene
    {
    public:
        Tema3();
        ~Tema3();

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

        // Helper functions
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1);
        void UpdateBalloonPosition(glm::vec3& position, float radius, float speed, float height, float elapsedTime);
        void RenderBalloon(const glm::vec3& position, const std::string& sphereName, const std::string& boxName,
            const std::vector<std::string>& pilons, const std::string& textureName, float scale, float yTranslate);
        void RenderTerrain();
        void UpdateBalloonPhysics(float deltaTime, glm::vec3& position, glm::vec3& velocity, float windForce, float windChangeRate, bool applyAcceleration);

    protected:
        std::unordered_map<std::string, Texture2D*> mapTextures;
        GLboolean mixTextures;

        // Balloon positions
        glm::vec3 balloon1Position = glm::vec3(0, 10, 0);
        glm::vec3 balloon2Position = glm::vec3(0, 0, 0);
        glm::vec3 balloon3Position = glm::vec3(0, 0, 0);
        glm::vec3 balloon4Position = glm::vec3(0, 0, 0);
        glm::vec3 balloon5Position = glm::vec3(0, 0, 0);
        glm::vec3 balloonVelocity1 = glm::vec3(0, 0, 0);
        bool isKeyPressed = false;
    };
}
