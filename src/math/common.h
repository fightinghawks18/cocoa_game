#pragma once

#include <numbers>

#include "vector3.h"
#include "quaternion.h"
#include "matrix4x4.h"

namespace Cocoa::Math {
    inline float Radians(float deg) {
        return deg * (std::numbers::pi / 180.0);
    }

    inline float Degrees(float rad) {
        return rad * (180.0 / std::numbers::pi);
    }

    inline Matrix4x4 CreateModelMatrix(Vector3 position, Quaternion rotation, Vector3 scale) {
        Matrix4x4 scaleMatrix;
        scaleMatrix(0, 0) = scale.x;
        scaleMatrix(1, 1) = scale.y;
        scaleMatrix(2, 2) = scale.z;

        Matrix3x3 rotation3x3 = ToMatrix3x3(rotation);
        Matrix4x4 rotationMatrix;
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                rotationMatrix(r, c) = rotation3x3(r, c);
            }
        }

        Matrix4x4 translationMatrix;
        translationMatrix(0, 3) = position.x;
        translationMatrix(1, 3) = position.y;
        translationMatrix(2, 3) = position.z;

        return translationMatrix * rotationMatrix * scaleMatrix;
    }

    inline Matrix4x4 CreatePerspectiveMatrix(float fovY, float aspectRatio, float nearZ, float farZ) {
        float tanHalfFov = tan(fovY / 2.0f);

        Matrix4x4 projectionMatrix;
        projectionMatrix(0,0) = 1.0f / (aspectRatio * tanHalfFov);
        projectionMatrix(1,1) = -1.0f / tanHalfFov;
        projectionMatrix(2,2) = farZ / (nearZ - farZ);
        projectionMatrix(2,3) = -(farZ * nearZ) / (farZ - nearZ);
        projectionMatrix(3,2) = -1.0f;
        projectionMatrix(3,3) = 0.0f;

        return projectionMatrix;
    }

    inline Matrix4x4 CreateOrthographicMatrix(float left, float right, float bottom, float top, float nearZ, float farZ) {
        Matrix4x4 orthographicMatrix;
    
        orthographicMatrix(0,0) = 2.0f / (right - left);
        orthographicMatrix(1,1) = -2.0f / (top - bottom);
        orthographicMatrix(2,2) = 1.0f / (nearZ - farZ);
        
        orthographicMatrix(0,3) = -(right + left) / (right - left);
        orthographicMatrix(1,3) = -(top + bottom) / (top - bottom);
        orthographicMatrix(2,3) = nearZ / (nearZ - farZ);
        
        return orthographicMatrix;
    }
}