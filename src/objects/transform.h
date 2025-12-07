#pragma once

#include "../math/common.h"

namespace Cocoa::Objects {
    class Transform
    {
      public:
        Transform() = default;
        ~Transform() = default;

        void SetPosition(Math::Vector3 position) { _position = position; }

        void Translate(Math::Vector3 position) { _position += position; }

        void Scale(Math::Vector3 scale) { _scale = scale; }

        void RotateX(float angle)
        {
            _x += Math::Radians(angle);
            UpdateRotation();
        }

        void RotateY(float angle)
        {
            _y += Math::Radians(angle);
            UpdateRotation();
        }

        void RotateZ(float angle)
        {
            _z += Math::Radians(angle);
            UpdateRotation();
        }

        [[nodiscard]] Math::Vector3& GetPosition() { return _position; }
        [[nodiscard]] Math::Quaternion& GetRotation() { return _rotation; }
        [[nodiscard]] Math::Vector3& GetScale() { return _scale; }
        [[nodiscard]] Math::Vector3 GetForward() const { return _rotation * Math::Vector3(0, 0, -1); }

        [[nodiscard]] Math::Vector3 GetUp() const { return _rotation * Math::Vector3(0, 1, 0); }

        [[nodiscard]] Math::Vector3 GetRight() const { return _rotation * Math::Vector3(1, 0, 0); }

        [[nodiscard]] Math::Matrix4x4 GetModelMatrix() const
        {
            return Math::CreateModelMatrix(_position, _rotation, _scale);
        }

      private:
        float _x = 0, _y = 0, _z = 0;

        Math::Vector3 _position;
        Math::Quaternion _rotation;
        Math::Vector3 _scale = Math::Vector3(1);

        void UpdateRotation()
        {
            auto yaw = Math::FromAxisAngle(Math::Vector3(0, 1, 0), _y);
            auto pitch = Math::FromAxisAngle(Math::Vector3(1, 0, 0), _x);
            auto roll = Math::FromAxisAngle(Math::Vector3(0, 0, 1), _z);
            _rotation = yaw * pitch * roll;
            _rotation.Normalize();
        }
    };
} // namespace Cocoa::Objects