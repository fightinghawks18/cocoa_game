#pragma once

#include "../math/common.h"

namespace Cocoa::Objects {
    class Transform {
    public:
        Transform() = default;
        ~Transform() = default;

        void Translate(Math::Vector3 position) {
            _position = position;
        }

        void Scale(Math::Vector3 scale) {
            _scale = scale;
        }

        void RotateX(float angle) {
            _rotation *= Math::FromAxisAngle(Math::Vector3(1, 0, 0), angle);
            _rotation.Normalize();
        }

        void RotateY(float angle) {
            _rotation *= Math::FromAxisAngle(Math::Vector3(0, 1, 0), angle);
            _rotation.Normalize();
        }

        void RotateZ(float angle) {
            _rotation *= Math::FromAxisAngle(Math::Vector3(0, 0, 1), angle);
            _rotation.Normalize();
        }

        [[nodiscard]] Math::Vector3& GetPosition() { return _position; }
        [[nodiscard]] Math::Quaternion& GetRotation() { return _rotation; }
        [[nodiscard]] Math::Vector3& GetScale() { return _scale; }
        [[nodiscard]] Math::Vector3 GetForward() const {
            return _rotation * Math::Vector3(0, 0, -1);
        }

        [[nodiscard]] Math::Matrix4x4 GetModelMatrix() const {
            return Math::CreateModelMatrix(_position, _rotation, _scale);
        }
    private:
        Math::Vector3 _position;
        Math::Quaternion _rotation;
        Math::Vector3 _scale = Math::Vector3(1);
    };
}