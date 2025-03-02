#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/camera.h"
#include <unordered_set>


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        Tema2();
        ~Tema2();

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
        void RenderDrone();
        void RenderTerrainAndObstacles();
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderMeshTerrain(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderScene();
        void PlaceRandomObstacles();
        glm::vec3 GenerateUniquePosition(std::unordered_set<std::string>& uniquePositions, float terrainWidth, float terrainHeight);
        bool CheckCollision(const glm::vec3& position, float radius);
        bool reachedCheckpoint(const glm::vec3& packetPosition, const glm::vec3& checkpointPosition);
        void RenderArrow(glm::vec3 target);

    protected:
        glm::vec3 dronePosition; 
        float droneRotationOY;             
        float propellerRotationSpeed;      
        float propellerAngle;
        float droneRadius;
		bool collision;
        bool packetAttached;
		glm::vec3 packetPosition;
        int packetCount;
        int printedMsg;
        camera::Camera* camera;
        glm::mat4 projectionMatrix;
        glm::vec3 cameraOffset;
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraUp;
        std::vector<glm::vec3> treePositions;
        std::vector<glm::vec3> buildingPositions;
        std::vector<float> randomSizes; 
        std::vector<glm::vec3> packetPositions;
        std::vector<glm::vec3> checkpointPositions;
        GLenum polygonMode;
        ViewportArea miniViewportArea;
    };
}   // namespace m1

