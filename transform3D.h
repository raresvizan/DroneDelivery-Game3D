#pragma once

#include "utils/glm_utils.h"


namespace transform3D
{
    // Translate matrix
    inline glm::mat4 Translate(float translateX, float translateY, float translateZ)
    {
        // TODO(student): Implement the translation matrix
        return glm::transpose(
            glm::mat4(1, 0, 0, translateX,
                0, 1, 0, translateY,
                0, 0, 1, translateZ,
                0, 0, 0, 1)
        );


    }

    // Scale matrix
    inline glm::mat4 Scale(float scaleX, float scaleY, float scaleZ)
    {
        // TODO(student): Implement the scaling matrix
        return glm::transpose(
            glm::mat4(scaleX, 0, 0, 0,
                0, scaleY, 0, 0,
                0, 0, scaleZ, 0,
                0, 0, 0, 1)
        );

    }

    // Rotate matrix relative to the OZ axis
    inline glm::mat4 RotateOZ(float radians)
    {
        // TODO(student): Implement the rotation matrix
        float cosAngle = cos(radians);
        float sinAngle = sin(radians);

        // Rotation matrix around the Z-axis
        return glm::transpose(
            glm::mat4(cosAngle, -sinAngle, 0, 0,
                sinAngle, cosAngle, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1)
        );

    }

    // Rotate matrix relative to the OY axis
    inline glm::mat4 RotateOY(float radians)
    {
        // TODO(student): Implement the rotation matrix
        float cosAngle = cos(radians);
        float sinAngle = sin(radians);

        // Rotation matrix around the Y-axis
        return glm::transpose(
            glm::mat4(cosAngle, 0, sinAngle, 0,
                0, 1, 0, 0,
                -sinAngle, 0, cosAngle, 0,
                0, 0, 0, 1)
        );

    }

    // Rotate matrix relative to the OX axis
    inline glm::mat4 RotateOX(float radians)
    {
        // TODO(student): Implement the rotation matrix
        float cosAngle = cos(radians);
        float sinAngle = sin(radians);

        // Rotation matrix around the X-axis
        return glm::transpose(
            glm::mat4(1, 0, 0, 0,
                0, cosAngle, -sinAngle, 0,
                0, sinAngle, cosAngle, 0,
                0, 0, 0, 1)
        );

    }
}   // namespace transform3D
