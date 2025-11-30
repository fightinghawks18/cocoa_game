#pragma once

#include "../math/common.h"

namespace Cocoa::Objects {
    class Camera {
    public:
        Camera() = default;
        ~Camera() = default;

        void RotatePitch(float angle) {
            _pitch += angle;
            UpdateRotation();
        }

        void RotateYaw(float angle) {
            _yaw += angle;
            UpdateRotation();
        }

        void SetPosition(const Math::Vector3& pos) { _position = pos; }
        void SetRotation(const Math::Quaternion& rot) { _rotation = rot; }
        void SetFieldOfView(float fov) { _fov = Math::Radians(fov); }
        void SetClipNearBounds(float near) { _near = near; }
        void SetClipFarBounds(float far) { _far = far; }
        void SetAspectRatio(float aspect) { _aspect = aspect; }

        [[nodiscard]] Math::Vector3& GetPosition() { return _position; }
        [[nodiscard]] Math::Quaternion& GetRotation() { return _rotation; }
        [[nodiscard]] Math::Vector3 GetForward() const {
            return _rotation * Math::Vector3(0, 0, -1);
        }

        [[nodiscard]] float GetFieldOfView() { return Math::Degrees(_fov); }
        [[nodiscard]] float GetClipNearBounds() { return Math::Degrees(_near); }
        [[nodiscard]] float GetClipFarBounds() { return Math::Degrees(_far); }
        [[nodiscard]] float GetAspectRatio() { return _aspect; }
        [[nodiscard]] Math::Matrix4x4 GetViewMatrix() const {
            return Math::LookAt(_position, _position + GetForward(), Math::Vector3(0, 1, 0));
        }

        [[nodiscard]] Math::Matrix4x4 GetProjectionMatrix() const {
            return Math::CreatePerspectiveMatrix(_fov, _aspect, _near, _far);
        }
    private:
        float _pitch = 0, _yaw = 0;
        float _fov, _near, _far, _aspect;
        Math::Vector3 _position;
        Math::Quaternion _rotation;

        void UpdateRotation() {
            Math::Quaternion yawQuat = Math::FromAxisAngle(Math::Vector3(0, 1, 0), _yaw);
            Math::Quaternion pitchQuat = Math::FromAxisAngle(Math::Vector3(1, 0, 0), _pitch);
            _rotation = yawQuat * pitchQuat;
            _rotation.Normalize();
        }
    };
}