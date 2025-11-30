#pragma once

#include <cmath>
#include "vector3.h"
#include "matrix3x3.h"

namespace Cocoa::Math {
    struct Quaternion {
        float x, y, z, w;

        Quaternion() : x(0), y(0), z(0), w(1) {}
        explicit Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        Quaternion& Normalize() {
            float len = sqrt(x * x + y * y + z * z + w * w);
            if (len > 0.0f) {
                x /= len;
                y /= len;
                z /= len;
                w /= len;
            }
            return *this;
        }
        
        Quaternion& operator*=(const Quaternion& other) {
            float ox = x;
            float oy = y;
            float oz = z;
            float ow = w;

            x = ow * other.x + ox * other.w + oy * other.z - oz * other.y;
            y = ow * other.y - ox * other.z + oy * other.w + oz * other.x;
            z = ow * other.z + ox * other.y - oy * other.x + oz * other.w;
            w = ow * other.w - ox * other.x - oy * other.y - oz * other.z;

            return *this;
        };
    };

    inline Matrix3x3 ToMatrix3x3(const Quaternion& quaternion) {
        Matrix3x3 result;

        float xx = quaternion.x * quaternion.x;
        float yy = quaternion.y * quaternion.y;
        float zz = quaternion.z * quaternion.z;
        float xy = quaternion.x * quaternion.y;
        float xz = quaternion.x * quaternion.z;
        float yz = quaternion.y * quaternion.z;
        float wx = quaternion.w * quaternion.x;
        float wy = quaternion.w * quaternion.y;
        float wz = quaternion.w * quaternion.z;
        
        result(0, 0) = 1.0f - 2.0f * (yy + zz);
        result(0, 1) = 2.0f * (xy - wz);
        result(0, 2) = 2.0f * (xz + wy);
        result(1, 0) = 2.0f * (xy + wz);
        result(1, 1) = 1.0f - 2.0f * (xx + zz);
        result(1, 2) = 2.0f * (yz - wx);
        result(2, 0) = 2.0f * (xz - wy);
        result(2, 1) = 2.0f * (yz + wx);
        result(2, 2) = 1.0f - 2.0f * (xx + yy);

        return result;
    }

    inline Quaternion operator*(const Quaternion& a, const Quaternion& b) {
        Quaternion result = a;
        result *= b;
        return result;
    }

    inline Vector3 operator*(const Quaternion& a, const Vector3& b) {
        Vector3 qvec(a.x, a.y, a.z);
        Vector3 uv = qvec.Cross(b);
        Vector3 uuv = qvec.Cross(uv);
        
        return b + ((uv * a.w) + uuv) * 2.0f;
    }

    inline Quaternion FromEuler(float pitch, float yaw, float roll) {
        float cy = cos(yaw * 0.5f);
        float sy = sin(yaw * 0.5f);
        float cp = cos(pitch * 0.5f);
        float sp = sin(pitch * 0.5f);
        float cr = cos(roll * 0.5f);
        float sr = sin(roll * 0.5f);

        return Quaternion(
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            cr * cp * cy + sr * sp * sy 
        );
    }

    inline Quaternion FromAxisAngle(Vector3 axis, float angle) {
        float halfAngle = angle * 0.5f;
        float s = sin(halfAngle);
        
        return Quaternion(
            axis.x * s,
            axis.y * s,
            axis.z * s,
            cos(halfAngle)
        );
    }
}