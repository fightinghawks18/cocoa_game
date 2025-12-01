#pragma once

#include "../math/common.h"
#include "transform.h"

namespace Cocoa::Objects {
    class Camera {
    public:
        Camera() = default;
        ~Camera() = default;

        void SetFieldOfView(float fov) { _fov = Math::Radians(fov); }
        void SetClipNearBounds(float near) { _near = near; }
        void SetClipFarBounds(float far) { _far = far; }
        void SetAspectRatio(float aspect) { _aspect = aspect; }

        [[nodiscard]] Transform& GetTransform() { return _transform; }
        [[nodiscard]] float GetFieldOfView() { return Math::Degrees(_fov); }
        [[nodiscard]] float GetClipNearBounds() { return Math::Degrees(_near); }
        [[nodiscard]] float GetClipFarBounds() { return Math::Degrees(_far); }
        [[nodiscard]] float GetAspectRatio() { return _aspect; }
        [[nodiscard]] Math::Matrix4x4 GetViewMatrix() {
            return Math::LookAt(_transform.GetPosition(), _transform.GetPosition() + _transform.GetForward(), Math::Vector3(0, 1, 0));
        }

        [[nodiscard]] Math::Matrix4x4 GetProjectionMatrix() const {
            return Math::CreatePerspectiveMatrix(_fov, _aspect, _near, _far);
        }
    private:
        float _fov, _near, _far, _aspect;
        Transform _transform;
    };
}