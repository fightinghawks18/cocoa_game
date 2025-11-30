#pragma once

#include <cmath>

namespace Cocoa::Math {
    struct Vector3 {
        float x, y, z;

        Vector3() : x(0), y(0), z(0) {}
        explicit Vector3(float scalar) : x(scalar), y(scalar), z(scalar) {}
        explicit Vector3(float x, float y) : x(x), y(y), z(0) {}
        explicit Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        float Dot(const Vector3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        Vector3 Cross(const Vector3& other) const {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        float Length() const {
            return sqrt(x * x + y * y + z * z);
        }
    
        Vector3 Normalize() const {
            float len = Length();
            if (len > 0.0f) {
                return Vector3(x / len, y / len, z / len);
            }
            return Vector3();
        }

        Vector3& operator+=(const Vector3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vector3& operator/=(const Vector3& other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        Vector3& operator*=(const Vector3& other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }
    };

    inline Vector3 operator+(const Vector3& a, const Vector3& b) {
        Vector3 result = a;
        result += b;
        return result;
    }

    inline Vector3 operator-(Vector3 a, const Vector3& b) {
        Vector3 result = a;
        result -= b;
        return result;
    }

    inline Vector3 operator/(Vector3 a, const Vector3& b) {
        Vector3 result = a;
        result /= b;
        return result;
    }

    inline Vector3 operator*(Vector3 a, const Vector3& b) {
        Vector3 result = a;
        result *= b;
        return result;
    }

    inline Vector3 operator*(const Vector3& v, float scalar) {
        return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
    }

    inline Vector3 operator*(float scalar, const Vector3& v) {
        return v * scalar;
    }

    inline Vector3 operator/(const Vector3& v, float scalar) {
        return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
    }

    inline bool operator==(const Vector3& a, const Vector3& b) {
        return a.x == b.x 
            && a.y == b.y 
            && a.z == b.z;
    }

    inline bool operator!=(const Vector3& a, const Vector3& b) {
        return !(a == b);
    }
}