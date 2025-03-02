#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object3D
{
	Mesh* CreateParallelepiped(std::string name, glm::vec3 dimensions, glm::vec3 color);
	Mesh* CreateCylinder(const std::string& name, glm::vec3 size, glm::vec3 color);
	Mesh* CreateCone(const std::string& name, glm::vec3 size, glm::vec3 color);
	Mesh* CreatePyramid(const std::string& name, const glm::vec3& baseDimensions, const glm::vec3& color);
	Mesh* CreateTerrainGrid(const std::string& name, glm::vec2 size, glm::vec3 color);
	Mesh* CreateSphere(const std::string& name, float radius, glm::vec3 color);
	Mesh* CreateArrow(std::string name, float shaftLength, float shaftRadius, float tipLength, float tipRadius, glm::vec3 color);
}