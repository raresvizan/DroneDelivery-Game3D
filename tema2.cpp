#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <random>

#include "lab_m1/tema2/transform3D.h"
#include "lab_m1/tema2/object3D.h"
#include "lab_m1/tema2/camera.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
    dronePosition = glm::vec3(50, 10, 50); 
    droneRotationOY = 0.0f;                
    propellerRotationSpeed = 25.0f;        
    propellerAngle = 0.0f;
	droneRadius = 1.0f;
    randomSizes = {1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 
                0.75f, 0.85f, 0.95f, 1.15f, 1.35f, 1.95f, 1.75f, 2.25f, 1.85f };
	collision = false;
    packetAttached = false;
    packetCount = 0;
	printedMsg = 0;
}


Tema2::~Tema2()
{
}

float RandomFloat(float min, float max) {
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(min, max); // range [min, max]
    return dis(e);
}

glm::vec3 Tema2::GenerateUniquePosition(std::unordered_set<std::string>& uniquePositions, float terrainWidth, float terrainHeight) {
    glm::vec3 position;
    bool isTooClose;

    do {
        isTooClose = false;

        // Generate a random position within bounds
        position = glm::vec3(RandomFloat(-terrainWidth / 2 + 10.0f, terrainWidth / 2 - 10.0f), 0,
            RandomFloat(-terrainHeight / 2 + 10.0f, terrainHeight / 2 - 10.0f)) +
            glm::vec3(terrainWidth / 2, 0, terrainHeight / 2);


        // Check the new position against all existing positions
        for (const auto& key : uniquePositions) {
            size_t underscorePos = key.find('_');
            float x = std::stof(key.substr(0, underscorePos));
            float z = std::stof(key.substr(underscorePos + 1));

            glm::vec3 existingPosition = glm::vec3(x, 0, z);
            float distance = glm::distance(existingPosition, position);
            if (distance < 20) {
                isTooClose = true;
                break;
            }
        }
    } while (isTooClose);

    std::string key = std::to_string(position.x) + "_" + std::to_string(position.z);
    uniquePositions.insert(key);

    return position;
}

void Tema2::PlaceRandomObstacles() {
    std::unordered_set<std::string> uniquePositions;

    int numTrees = 30;
    int numBuildings = 25;
    int numPackets = 3;
	int numCheckpoints = 3;

    // Generate unique positions for trees
    for (int i = 0; i < numTrees; ++i) {
        glm::vec3 position = GenerateUniquePosition(uniquePositions, 200, 200);
        treePositions.push_back(position);
    }

    // Generate unique positions for buildings
    for (int i = 0; i < numBuildings; ++i) {
        glm::vec3 position = GenerateUniquePosition(uniquePositions, 200, 200);
        buildingPositions.push_back(position);
    }

	// Generate unique positions for packets
	for (int i = 0; i < numPackets; ++i) {
		glm::vec3 position = GenerateUniquePosition(uniquePositions, 200, 200);
        position.y = 1.0f;
		packetPositions.push_back(position);
	}

	// Generate unique positions for checkpoints
	for (int i = 0; i < numCheckpoints; ++i) {
		glm::vec3 position = GenerateUniquePosition(uniquePositions, 200, 200);
		position.y = 1.0f;
		checkpointPositions.push_back(position);
	}
}

bool Tema2::CheckCollision(const glm::vec3& position, float radius) {
    // Collision with building
    int scaleIndex = 0; 
    for (size_t i = 0; i < buildingPositions.size(); ++i) {
        glm::vec3 building = buildingPositions[i];
        float scale = randomSizes[scaleIndex] * 5.0f; 
        scaleIndex = (scaleIndex + 1) % randomSizes.size();   
		
		float scaledWidth = 2.0f * scale;
		float scaledHeight = 2.0f * scale;
		float scaledDepth = 2.0f * scale;

        if (position.x >= building.x - scaledWidth / 2 - radius && position.x <= building.x + scaledWidth / 2 + radius &&
            position.z >= building.z - scaledDepth / 2 - radius && position.z <= building.z + scaledDepth / 2 + radius &&
            position.y >= building.y && position.y <= building.y + scaledHeight) {
            return true;
        }

        // Check collision with the pyramid roof
        scaledWidth = scale;
        scaledHeight = 2.5f * scale;
        scaledDepth =  scale;
        if (position.x >= building.x - scaledWidth / 2 - radius && position.x <= building.x + scaledWidth / 2 + radius &&
            position.z >= building.z - scaledDepth / 2 - radius && position.z <= building.z + scaledDepth / 2 + radius &&
            position.y >= building.y && position.y <= building.y + scaledHeight) {
            return true;
        }
    }

	// Collision with trees
    scaleIndex = 0;
    for (size_t i = 0; i < treePositions.size(); ++i){
		glm::vec3 tree = treePositions[i];
        float scale = randomSizes[scaleIndex] * 2;
        scaleIndex = (scaleIndex + 1) % randomSizes.size(); 
  
        float scaledTrunkRadius = 0.3f * scale; 
        float scaledTrunkHeight = 1.5f * scale; 
        float distanceXZ = glm::length(glm::vec2(position.x - tree.x, position.z - tree.z));
        if (distanceXZ < scaledTrunkRadius + radius &&
            position.y >= tree.y && position.y <= tree.y + scaledTrunkHeight) {
            return true; 
        }

        // Leaves
        glm::vec3 leaves1Center = tree + glm::vec3(0.0f, scaledTrunkHeight + 2.15f*scale, 0.0f);
        glm::vec3 leaves2Center = tree + glm::vec3(0.0f, scaledTrunkHeight + 3.7f*scale, 0.0f);
        float leavesRadius = 2.5f * scale; 

        float distanceToLeaves1 = glm::length(position - leaves1Center);
        float distanceToLeaves2 = glm::length(position - leaves2Center);

        if (distanceToLeaves1 < radius + leavesRadius || distanceToLeaves2 < radius + leavesRadius) {
            return true;
        }
    }

    //Collision with terrain
	if (dronePosition.y < 2.0f || dronePosition.y > 100.0f) {
		return true;
	}

    return false;
}

void Tema2::Init()
{
    camera = new camera::Camera();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	Mesh* body = object3D::CreateParallelepiped("body", glm::vec3(1.5f, 0.1f, 0.1f), glm::vec3(0.45f, 0.5f, 0.57f));
	AddMeshToList(body);

    Mesh* bodyRed = object3D::CreateParallelepiped("bodyRed", glm::vec3(1.5f, 0.1f, 0.1f), glm::vec3(1,0,0));
    AddMeshToList(bodyRed);

    Mesh* cube = object3D::CreateParallelepiped("cube", glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.45f, 0.5f, 0.57f));
    AddMeshToList(cube);

	Mesh* propeller = object3D::CreateParallelepiped("propeller", glm::vec3(0.3f, 0.03f, 0.05f), glm::vec3(0, 0, 0));
	AddMeshToList(propeller);

    Mesh* treeTrunk = object3D::CreateCylinder("treeTrunk", glm::vec3(0.3f, 1.5f, 20), glm::vec3(0.6f, 0.3f, 0.2f));
    AddMeshToList(treeTrunk);

    Mesh* treeLeaves = object3D::CreateCone("treeLeaves", glm::vec3(2.5f, 4.5f, 20), glm::vec3(0.1f, 0.6f, 0.2f));
    AddMeshToList(treeLeaves);

    Mesh* building = object3D::CreateParallelepiped("building",glm::vec3(2, 3, 2), glm::vec3(0.8f, 0.7f, 0.4f));
    AddMeshToList(building);

    Mesh* roof = object3D::CreatePyramid("roof", glm::vec3(2, 1, 2), glm::vec3(0.8f, 0.2f, 0.2f));
    AddMeshToList(roof);

	Mesh* packet = object3D::CreateParallelepiped("packet", glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(0.65f, 0.45f, 0.25f));
	AddMeshToList(packet);

	Mesh* chain = object3D::CreateCylinder("chain", glm::vec3(0.02f, 1.0f, 20), glm::vec3(0.65f, 0.45f, 0.25f));
	AddMeshToList(chain);

    Mesh* checkpoint = object3D::CreateCylinder("checkpoint", glm::vec3(2.0f, 0.1f, 20), glm::vec3(0.73f, 0.0f, 1.0f));
	AddMeshToList(checkpoint);

	polygonMode = GL_FILL;
    cameraOffset = glm::vec3(0, 1.7f, 4.0f);
    cameraUp = glm::vec3(0, 1, 0);
    camera = new camera::Camera();
    camera->Set(glm::vec3(0, 0, 0.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(55), window->props.aspectRatio, 0.01f, 500.0f);
    
	Mesh* terrainMesh = object3D::CreateTerrainGrid("terrain", glm::vec2(200, 200), glm::vec3(0.65f, 0.5f, 0.39f));
	AddMeshToList(terrainMesh);
    shaders["Terrain"] = new Shader("Terrain");
    shaders["Terrain"]->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shaders["Terrain"]->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shaders["Terrain"]->CreateAndLink();

	PlaceRandomObstacles();

    Mesh* sky = object3D::CreateSphere("sky", 200, glm::vec3(0.5f, 0.7f, 1.0f));
    AddMeshToList(sky);
    Mesh* skyEnd = object3D::CreateSphere("skyEnd", 200, glm::vec3(0.0f, 0.0f, 0.545f)); 
    AddMeshToList(skyEnd);

    glm::ivec2 resolution = window->GetResolution();
    float xPosition = resolution.x - resolution.x / 5.f - 20.0f; 
    float yPosition = resolution.y - resolution.y / 5.f - 20.0f;
    miniViewportArea = ViewportArea(xPosition, yPosition, resolution.x / 5.f, resolution.y / 5.f);

    Mesh* arrowMesh = object3D::CreateArrow("arrow", 0.3f, 0.05f, 0.2f, 0.1f, glm::vec3(0.73f, 0.0f, 1.0f));
	AddMeshToList(arrowMesh);
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderMeshTerrain(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glUniform1f(glGetUniformLocation(shader->GetProgramID(), "noiseFrequency"), 0.7f);
    glUniform1f(glGetUniformLocation(shader->GetProgramID(), "amplitude"), 0.7f);
    glUniform3f(glGetUniformLocation(shader->GetProgramID(), "greenColor"), 0.08f, 0.327f, 0.08f); 
    glUniform3f(glGetUniformLocation(shader->GetProgramID(), "brownColor"), 0.545f, 0.271f, 0.074f); 

    mesh->Render();
}

void Tema2::RenderDrone() {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(dronePosition.x, dronePosition.y, dronePosition.z);

    if (packetAttached) {
        glm::mat4 chainMatrix = modelMatrix * transform3D::Translate(0, -1.0f, 0);
        RenderMesh(meshes["chain"], shaders["VertexColor"], chainMatrix);
    }
    
    modelMatrix *= transform3D::RotateOY(droneRotationOY);
    glm::mat4 bodyMatrix = modelMatrix;
	bodyMatrix *= transform3D::RotateOY(glm::radians(45.0f));
	if (collision) {
		RenderMesh(meshes["bodyRed"], shaders["VertexColor"], bodyMatrix);
	}
	else {
		RenderMesh(meshes["body"], shaders["VertexColor"], bodyMatrix);
	}
    bodyMatrix = modelMatrix;
    bodyMatrix *= transform3D::RotateOY(glm::radians(135.0f));
    if (collision) {
        RenderMesh(meshes["bodyRed"], shaders["VertexColor"], bodyMatrix);
    }
    else {
        RenderMesh(meshes["body"], shaders["VertexColor"], bodyMatrix);
    }

	glm::mat4 cubeMatrix = modelMatrix;
    cubeMatrix *= transform3D::RotateOY(glm::radians(45.0f));
	cubeMatrix *= transform3D::Translate(0.70f, 0.1f, 0.0f);
	RenderMesh(meshes["cube"], shaders["VertexColor"], cubeMatrix);
    cubeMatrix = modelMatrix;
    cubeMatrix *= transform3D::RotateOY(glm::radians(45.0f));
    cubeMatrix *= transform3D::Translate(-0.70f, 0.1f, 0.0f);
    RenderMesh(meshes["cube"], shaders["VertexColor"], cubeMatrix);
    cubeMatrix = modelMatrix;
    cubeMatrix *= transform3D::RotateOY(glm::radians(45.0f));
    cubeMatrix *= transform3D::Translate(0.0f, 0.1f, 0.7f);
    cubeMatrix *= transform3D::RotateOY(glm::radians(90.0f)); 
    RenderMesh(meshes["cube"], shaders["VertexColor"], cubeMatrix);
    cubeMatrix = modelMatrix;
    cubeMatrix *= transform3D::RotateOY(glm::radians(45.0f));
    cubeMatrix *= transform3D::Translate(0.0f, 0.1f, -0.7f);
    cubeMatrix *= transform3D::RotateOY(glm::radians(90.0f)); 
    RenderMesh(meshes["cube"], shaders["VertexColor"], cubeMatrix);

	glm::mat4 propellerMatrix = modelMatrix;
    propellerMatrix *= transform3D::RotateOY(glm::radians(45.0f));
	propellerMatrix *= transform3D::Translate(0.7f, 0.175f, 0.0f);
	propellerMatrix *= transform3D::RotateOY(propellerAngle);
    RenderMesh(meshes["propeller"], shaders["VertexColor"], propellerMatrix);
    propellerMatrix = modelMatrix;
    propellerMatrix *= transform3D::RotateOY(glm::radians(45.0f));
    propellerMatrix *= transform3D::Translate(-0.7f, 0.175f, 0.0f);
    propellerMatrix *= transform3D::RotateOY(propellerAngle);
    RenderMesh(meshes["propeller"], shaders["VertexColor"], propellerMatrix);
    propellerMatrix = modelMatrix;
    propellerMatrix *= transform3D::RotateOY(glm::radians(45.0f));
    propellerMatrix *= transform3D::Translate(0.0f, 0.175f, 0.7f);
    propellerMatrix *= transform3D::RotateOY(propellerAngle);
    RenderMesh(meshes["propeller"], shaders["VertexColor"], propellerMatrix);
    propellerMatrix = modelMatrix;
    propellerMatrix *= transform3D::RotateOY(glm::radians(45.0f));
    propellerMatrix *= transform3D::Translate(0.0f, 0.175f, -0.7f);
    propellerMatrix *= transform3D::RotateOY(propellerAngle);
    RenderMesh(meshes["propeller"], shaders["VertexColor"], propellerMatrix);
}

void Tema2::RenderTerrainAndObstacles() {
    RenderMeshTerrain(meshes["terrain"], shaders["Terrain"], glm::mat4(1));
    // Render trees
    int scaleIndex = 0; 
    for (size_t i = 0; i < treePositions.size(); ++i) {
        glm::vec3 position = treePositions[i];
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position);
        float scale = randomSizes[scaleIndex] * 2;
        scaleIndex = (scaleIndex + 1) % randomSizes.size(); 
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
        RenderMesh(meshes["treeTrunk"], shaders["VertexColor"], modelMatrix);
        glm::mat4 leavesMatrix = modelMatrix;
        leavesMatrix = glm::translate(leavesMatrix, glm::vec3(0, 1.5f, 0));
        RenderMesh(meshes["treeLeaves"], shaders["VertexColor"], leavesMatrix);
        leavesMatrix = glm::translate(leavesMatrix, glm::vec3(0, 2.5f, 0));
        RenderMesh(meshes["treeLeaves"], shaders["VertexColor"], leavesMatrix);

    }

    // Render buildings
    scaleIndex = 0; 
    for (size_t i = 0; i < buildingPositions.size(); ++i) {
        glm::vec3 position = buildingPositions[i];
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position);
        float scale = randomSizes[scaleIndex] * 5;
        scaleIndex = (scaleIndex + 1) % randomSizes.size(); 
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
        RenderMesh(meshes["building"], shaders["VertexColor"], modelMatrix);
        glm::mat4 roofMatrix = modelMatrix;
        roofMatrix = glm::translate(roofMatrix, glm::vec3(0, 1.5f, 0));
        RenderMesh(meshes["roof"], shaders["VertexColor"], roofMatrix);
    }
}

bool Tema2::reachedCheckpoint(const glm::vec3& packetPosition, const glm::vec3& checkpointPosition) {
    float dx = packetPosition.x - checkpointPosition.x;
    float dz = packetPosition.z - checkpointPosition.z;
    return (dx * dx + dz * dz) <= (2.0f * 2.0f);
}

glm::vec3 CalculateDirectionToTarget(glm::vec3 dronePosition, glm::vec3 targetPosition) {
    glm::vec3 direction = targetPosition - dronePosition; 
    direction.y = 0; 
    return glm::normalize(direction);
}

void Tema2::RenderArrow(glm::vec3 target) {
    glm::vec3 directionToTarget = CalculateDirectionToTarget(dronePosition, target);
    float yaw = atan2(directionToTarget.x, directionToTarget.z);

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= glm::translate(modelMatrix, dronePosition + glm::vec3(0, 0.3f, 0));
    modelMatrix *= transform3D::RotateOY(yaw);
    modelMatrix *= transform3D::RotateOX(glm::radians(90.0f));
    RenderMesh(meshes["arrow"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderScene() {
    // Render the sky
    glm::mat4 skyMatrix = glm::mat4(1);
    skyMatrix *= transform3D::Translate(100.0f, 0.0f, 100.0f);
	if (packetCount == 3)
		RenderMesh(meshes["skyEnd"], shaders["VertexColor"], skyMatrix);
	else
		RenderMesh(meshes["sky"], shaders["VertexColor"], skyMatrix);

    RenderDrone();
	RenderTerrainAndObstacles();
    
    if (packetAttached) {
        packetPosition = dronePosition - glm::vec3(0, 1.0f, 0);

        // Check if the packet has been delivered
        if (packetCount < 3 && reachedCheckpoint(dronePosition, checkpointPositions[packetCount])) {
            packetAttached = false;
            packetPosition.y = 1.0f;
            packetCount++;
        }
    }
    else {
		// pick up the packet if it's close
        float distance = glm::length(dronePosition - packetPosition);
        if (distance < 2.0f) {
            packetAttached = true;
        }
    }

    if (packetCount < 3) {
        if (!packetAttached) {
            packetPosition = packetPositions[packetCount];
            RenderArrow(packetPosition);
        }

        // Render the checkpoint
        if (packetAttached) {
            RenderMesh(meshes["checkpoint"], shaders["VertexColor"], glm::translate(glm::mat4(1), checkpointPositions[packetCount]));
            RenderArrow(checkpointPositions[packetCount]);
        }

        // Render the packet
        RenderMesh(meshes["packet"], shaders["VertexColor"], glm::translate(glm::mat4(1), packetPosition));
    }
}

void Tema2::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    glm::vec3 forward = glm::vec3(sin(droneRotationOY), 0, cos(droneRotationOY));
    cameraPosition = dronePosition - forward * cameraOffset.z + glm::vec3(0, cameraOffset.y, 0);
    cameraTarget = dronePosition + glm::vec3(0, 0.75f, 0);;
    camera->Set(cameraPosition, cameraTarget, cameraUp);
    projectionMatrix = glm::perspective(RADIANS(55), window->props.aspectRatio, 0.01f, 500.0f);

    RenderScene();

    glClear(GL_DEPTH_BUFFER_BIT);

    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);
    camera->Set(cameraPosition + glm::vec3(0,4,0), cameraTarget, glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 500.0f);
    RenderScene();

	if (packetCount == 1 && printedMsg == 0) {
		cout << endl<<"Packet 1 delivered to checkpoint" << endl;
		printedMsg = 1;
	}
	if (packetCount == 2 && printedMsg == 1) {
		cout << "Packet 2 delivered to checkpoint" << endl;
		printedMsg = 2;
	}
	if (packetCount == 3 && printedMsg == 2) {
		cout << "Packet 3 delivered to checkpoint" << endl;
        cout << "Congratulations! YOU FINISHED YOUR SHIFT" << endl<<endl;
		printedMsg = 3;
	}
}

void Tema2::FrameEnd()
{
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float moveSpeed = 7.5f; 
    float rotateSpeed = 1.0f; 
    glm::vec3 forward = glm::vec3(sin(droneRotationOY), 0, cos(droneRotationOY));
    glm::vec3 right = glm::vec3(-cos(droneRotationOY), 0, sin(droneRotationOY));
    glm::vec3 up = glm::vec3(0, 1, 0); 
    glm::vec3 nextPosition = dronePosition;

    if (window->KeyHold(GLFW_KEY_W)) {
        nextPosition += forward * moveSpeed * deltaTime;
        if (!CheckCollision(nextPosition, droneRadius)) {
            dronePosition = nextPosition;
			collision = false;
        }
		else collision = true;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        nextPosition -= forward * moveSpeed * deltaTime;
        if (!CheckCollision(nextPosition, droneRadius)) {
            dronePosition = nextPosition;
			collision = false;
        }
        else collision = true;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        nextPosition -= right * moveSpeed * deltaTime;
        if (!CheckCollision(nextPosition, droneRadius)) {
            dronePosition = nextPosition;
			collision = false;
        }
        else collision = true;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
		nextPosition += right * moveSpeed * deltaTime;
        if (!CheckCollision(nextPosition, droneRadius)) {
            dronePosition = nextPosition;
            collision = false;
        }
        else collision = true;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
		float nextDroneRotation = rotateSpeed * deltaTime;
        if (!CheckCollision(nextPosition, droneRadius)) {
			droneRotationOY += nextDroneRotation;
            collision = false;
        }
        else collision = true;
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        float nextDroneRotation = rotateSpeed * deltaTime;
        if (!CheckCollision(nextPosition, droneRadius)) {
			droneRotationOY -= nextDroneRotation;
            collision = false;
        }
        else collision = true;
    }
    if (window->KeyHold(GLFW_KEY_UP)) {
		nextPosition += up * moveSpeed * deltaTime;
        if (!CheckCollision(nextPosition, droneRadius)) {
            dronePosition = nextPosition;
            collision = false;
        }
        else {
            collision = true;
            if(dronePosition.y<2)
				dronePosition.y = 2;
			if (dronePosition.y > 100)
				dronePosition.y = 100;
        }
    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
		nextPosition -= up * moveSpeed * deltaTime;
        if (!CheckCollision(nextPosition, droneRadius)) {
            dronePosition = nextPosition;
            collision = false;
        }
        else collision = true;
    }

    dronePosition.x = glm::clamp(dronePosition.x, 0.0f, 200.0f);
    dronePosition.z = glm::clamp(dronePosition.z, 0.0f, 200.0f);
    
    propellerAngle += propellerRotationSpeed * deltaTime;
    propellerAngle = fmod(propellerAngle, glm::two_pi<float>());
}


void Tema2::OnKeyPress(int key, int mods)
{

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
    float viewportWidth = width / 5.f;  
    float viewportHeight = height / 5.f; 

    float xPosition = width - viewportWidth - 20.0f; 
    float yPosition = height - viewportHeight - 20.0f; 
    miniViewportArea = ViewportArea(xPosition, yPosition, viewportWidth, viewportHeight);
}
