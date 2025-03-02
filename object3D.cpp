#include "object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


#include "object3D.h"
#include <vector>

Mesh* object3D::CreateParallelepiped(std::string name, glm::vec3 dimensions, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 halfDim = dimensions * 0.5f;
    vertices.push_back(VertexFormat(glm::vec3(-halfDim.x, -halfDim.y,  halfDim.z), color *1.2f));
    vertices.push_back(VertexFormat(glm::vec3( halfDim.x, -halfDim.y,  halfDim.z), color *0.8f));
    vertices.push_back(VertexFormat(glm::vec3(-halfDim.x,  halfDim.y,  halfDim.z), color ));
    vertices.push_back(VertexFormat(glm::vec3( halfDim.x,  halfDim.y,  halfDim.z), color));
    vertices.push_back(VertexFormat(glm::vec3(-halfDim.x, -halfDim.y, -halfDim.z), color ));
    vertices.push_back(VertexFormat(glm::vec3( halfDim.x, -halfDim.y, -halfDim.z), color ));
    vertices.push_back(VertexFormat(glm::vec3(-halfDim.x,  halfDim.y, -halfDim.z), color * 1.2f));
    vertices.push_back(VertexFormat(glm::vec3( halfDim.x,  halfDim.y, -halfDim.z), color * 0.8f));

    indices = {
        0, 1, 2, 1, 3, 2, // Front face
        4, 5, 6, 5, 7, 6, // Back face
        0, 1, 4, 1, 5, 4, // Bottom face
        2, 3, 6, 3, 7, 6, // Top face
        0, 2, 4, 2, 6, 4, // Left face
        1, 3, 5, 3, 7, 5  // Right face
    };

    Mesh* mesh = new Mesh(name);
    mesh->InitFromData(vertices, indices);
    return mesh;
}

Mesh* object3D::CreateCylinder(const std::string& name, glm::vec3 size,glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float radius = size.x;
    float height = size.y;
    int slices = (int)size.z;

    // Vertices for the cylinder
    for (int i = 0; i < slices; ++i) {
        float angle = glm::two_pi<float>() * i / slices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.emplace_back(glm::vec3(x, 0, z), color);       // Bottom ring
        vertices.emplace_back(glm::vec3(x, height, z), color); // Top ring
    }

    // Add indices for the side triangles
    for (int i = 0; i < slices; ++i) {
        int next = (i + 1) % slices;

        // Side triangles
        indices.push_back(i * 2);
        indices.push_back(next * 2);
        indices.push_back(i * 2 + 1);

        indices.push_back(next * 2);
        indices.push_back(next * 2 + 1);
        indices.push_back(i * 2 + 1);
    }

    // Bottom and top caps
    int bottomCenter = (int)vertices.size();
    vertices.emplace_back(glm::vec3(0, 0, 0), color);

    int topCenter = (int)vertices.size();
    vertices.emplace_back(glm::vec3(0, height, 0), color);

    for (int i = 0; i < slices; ++i) {
        int next = (i + 1) % slices;

        // Bottom cap
        indices.push_back(bottomCenter);
        indices.push_back(next * 2);
        indices.push_back(i * 2);

        // Top cap
        indices.push_back(topCenter);
        indices.push_back(i * 2 + 1);
        indices.push_back(next * 2 + 1);
    }

    Mesh* cylinder = new Mesh(name);
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}

Mesh* object3D::CreateCone(const std::string& name,glm::vec3 size,glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float radius = size.x;
    float height = size.y;
    int slices = (int)size.z;

    // Vertices for the cone base
    for (int i = 0; i < slices; ++i) {
        float angle = glm::two_pi<float>() * i / slices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.emplace_back(glm::vec3(x, 0, z), color - glm::vec3(0.05f, 0.2f, 0.05f));
    }

    // Add the tip of the cone
    int tipIndex = (int)vertices.size();
    vertices.emplace_back(glm::vec3(0, height, 0), color + glm::vec3(0.2f,0.2f,0.1f));

    // Add the center of the base
    int baseCenterIndex = (int)vertices.size();
    vertices.emplace_back(glm::vec3(0, 0, 0), color);

    // Indices for the sides of the cone
    for (int i = 0; i < slices; ++i) {
        int next = (i + 1) % slices;

        indices.push_back(tipIndex);
        indices.push_back(i);
        indices.push_back(next);
    }

    // Indices for the base
    for (int i = 0; i < slices; ++i) {
        int next = (i + 1) % slices;

        indices.push_back(baseCenterIndex);
        indices.push_back(next);
        indices.push_back(i);
    }

    Mesh* cone = new Mesh(name);
    cone->InitFromData(vertices, indices);
    return cone;
}

Mesh* object3D::CreatePyramid(const std::string& name, const glm::vec3& baseDimensions, const glm::vec3& color) {
    float baseX = baseDimensions.x / 2.0f; 
    float baseZ = baseDimensions.z / 2.0f; 
    float height = baseDimensions.y;     

    // Define the vertices of the pyramid
    std::vector<VertexFormat> vertices = {
        // Base vertices
        VertexFormat(glm::vec3(-baseX, 0, -baseZ), color), // Bottom-left
        VertexFormat(glm::vec3(baseX, 0, -baseZ), color - glm::vec3(0.2f, 0.1f, 0.1f)),  // Bottom-right
        VertexFormat(glm::vec3(baseX, 0, baseZ), color),   // Top-right
        VertexFormat(glm::vec3(-baseX, 0, baseZ), color + glm::vec3(0.1f, 0.1f, 0)),  // Top-left

        // Apex vertex
        VertexFormat(glm::vec3(0, height, 0), color) // Apex
    };

    // Define the indices for the base and the four triangular faces
    std::vector<unsigned int> indices = {
        // Base (two triangles)
        0, 1, 2,
        0, 2, 3,

        // Side faces
        0, 1, 4, // Bottom-left to bottom-right to apex
        1, 2, 4, // Bottom-right to top-right to apex
        2, 3, 4, // Top-right to top-left to apex
        3, 0, 4  // Top-left to bottom-left to apex
    };

    Mesh* pyramid = new Mesh(name);
    pyramid->InitFromData(vertices, indices);

    return pyramid;
}

Mesh* object3D::CreateTerrainGrid(const std::string& name, glm::vec2 size, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // grid
    for (int i = 0; i <= size.x; ++i) {
        for (int j = 0; j <= size.y; ++j) {
            vertices.emplace_back(glm::vec3(i, 0, j), color, glm::vec3(0, 0, 0));
        }
    }

	// dreptunghiuri din 2 triunghiuri
    for (int i = 0; i < size.x; ++i) {
        for (int j = 0; j < size.y; ++j) {
            int topLeft = i * (size.y + 1) + j;
            int bottomLeft = (i + 1) * (size.y + 1) + j;
            int topRight = i * (size.y + 1) + (j + 1);
            int bottomRight = (i + 1) * (size.y + 1) + (j + 1);

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh *grid = new Mesh(name);
    grid->InitFromData(vertices, indices);
    return grid;
}

Mesh* object3D::CreateSphere(const std::string& name, float radius, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    const unsigned int stackCount = 20; 
    const unsigned int sectorCount = 20;

    float x, y, z, xy;  
    float nx, ny, nz, lengthInv = 1.0f / radius;  
    float s, t; 

    // Generate vertices
    for (unsigned int i = 0; i <= stackCount; ++i) {
        float stackAngle = M_PI / 2 - i * M_PI / stackCount;  
        xy = radius * cos(stackAngle); 
        z = radius * sin(stackAngle);   

        for (unsigned int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * M_PI / sectorCount; 

            x = xy * cos(sectorAngle);  
            y = xy * sin(sectorAngle);  

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            s = (float)j / sectorCount;
            t = (float)i / stackCount;

            vertices.emplace_back(glm::vec3(x, y, z), color + glm::vec3(0.1f*(i%2+0.1)), glm::vec3(nx, ny, nz), glm::vec2(s, t));
        }
    }

    // Generate indices
    for (unsigned int i = 0; i < stackCount; ++i) {
        unsigned int k1 = i * (sectorCount + 1);  
        unsigned int k2 = k1 + sectorCount + 1;  

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    Mesh* sphere = new Mesh(name);
    sphere->InitFromData(vertices, indices);
    return sphere;
}

Mesh* object3D::CreateArrow(std::string name, float shaftLength, float shaftRadius, float tipLength, float tipRadius, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // cylinder
    int shaftSegments = 20; 
    for (int i = 0; i <= shaftSegments; i++) {
        float angle = (float)i / shaftSegments * 2.0f * glm::pi<float>();
        float x = cos(angle) * shaftRadius;
        float z = sin(angle) * shaftRadius;

        vertices.push_back(VertexFormat(glm::vec3(x, 0.0f, z), color));
        vertices.push_back(VertexFormat(glm::vec3(x, shaftLength, z), color * 0.8f));
    }

    for (int i = 0; i < shaftSegments; i++) {
        int bottom1 = i * 2;
        int bottom2 = ((i + 1) % shaftSegments) * 2;
        int top1 = bottom1 + 1;
        int top2 = bottom2 + 1;

        indices.push_back(bottom1);
        indices.push_back(bottom2);
        indices.push_back(top1);

        indices.push_back(bottom2);
        indices.push_back(top2);
        indices.push_back(top1);
    }

    // cone
    int tipOffset = vertices.size();
    glm::vec3 tipBaseCenter(0.0f, shaftLength, 0.0f);
    glm::vec3 tipTop(0.0f, shaftLength + tipLength, 0.0f);
    vertices.push_back(VertexFormat(tipTop, color * 1.2f)); 

    for (int i = 0; i <= shaftSegments; i++) {
        float angle = (float)i / shaftSegments * 2.0f * glm::pi<float>();
        float x = cos(angle) * tipRadius;
        float z = sin(angle) * tipRadius;
        vertices.push_back(VertexFormat(glm::vec3(x, shaftLength, z), color));
    }

    for (int i = 0; i < shaftSegments; i++) {
        int base1 = tipOffset + 1 + i;
        int base2 = tipOffset + 1 + (i + 1) % shaftSegments;
        indices.push_back(tipOffset);
        indices.push_back(base1);
        indices.push_back(base2);
    }

    Mesh* mesh = new Mesh(name);
    mesh->InitFromData(vertices, indices);
    return mesh;
}