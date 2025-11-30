#pragma once

#include <cstdint>
#include "matrix3x3.h"
#include "vector3.h"

namespace Cocoa::Math {
    struct Matrix4x4 {
        float m[4][4];

        float& operator()(int row, int col) {
            return m[row][col];
        }

        const float& operator()(int row, int col) const {
            return m[row][col];
        }
        
        Matrix4x4() {
            m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
            m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
            m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
            m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
        }

        Matrix4x4 Inverse() const {
            Matrix3x3 r;
            r(0, 0) = m[0][0]; r(0, 1) = m[0][1]; r(0, 2) = m[0][2];
            r(1, 0) = m[1][0]; r(1, 1) = m[1][1]; r(1, 2) = m[1][2];
            r(2, 0) = m[2][0]; r(2, 1) = m[2][1]; r(2, 2) = m[2][2];

            float tx = m[0][3];
            float ty = m[1][3];
            float tz = m[2][3];

            Matrix3x3 rInverse = r.Inverse();

            float new_tx = -(rInverse(0,0)*tx + rInverse(0,1)*ty + rInverse(0,2)*tz);
            float new_ty = -(rInverse(1,0)*tx + rInverse(1,1)*ty + rInverse(1,2)*tz);
            float new_tz = -(rInverse(2,0)*tx + rInverse(2,1)*ty + rInverse(2,2)*tz);

            Matrix4x4 result;
            result(0,0) = rInverse(0,0); result(0,1) = rInverse(0,1); 
            result(0,2) = rInverse(0,2); result(0,3) = new_tx;
            result(1,0) = rInverse(1,0); result(1,1) = rInverse(1,1); 
            result(1,2) = rInverse(1,2); result(1,3) = new_ty;
            result(2,0) = rInverse(2,0); result(2,1) = rInverse(2,1); 
            result(2,2) = rInverse(2,2); result(2,3) = new_tz;
            result(3,0) = 0; result(3,1) = 0; 
            result(3,2) = 0; result(3,3) = 1;

            return result;
        }

        float Determinant() const {
            return    m[0][0] * Minor(0, 0)
                    - m[0][1] * Minor(0, 1)
                    + m[0][2] * Minor(0, 2)
                    - m[0][3] * Minor(0, 3);
        }

        Matrix4x4 Transpose() const {
            Matrix4x4 result;
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    result(r, c) = m[c][r];
                }
            }
            return result;
        }

        Matrix4x4& operator+=(const Matrix4x4& other) {
            for (uint32_t r = 0; r < 4; r++) {
                for (uint32_t c = 0; c < 4; c++) {
                    m[r][c] += other(r, c);
                }
            }
            return *this;
        }

        Matrix4x4& operator-=(const Matrix4x4& other) {
            for (uint32_t r = 0; r < 4; r++) {
                for (uint32_t c = 0; c < 4; c++) {
                    m[r][c] -= other(r, c);
                }
            }
            return *this;
        }

        Matrix4x4& operator*=(const Matrix4x4& other) {
            Matrix4x4 result;
            for (uint32_t r = 0; r < 4; r++) {
                for (uint32_t c = 0; c < 4; c++) {
                    result(r, c) = 0.0f;
                    for (uint32_t s = 0; s < 4; s++) {
                        result(r, c) += m[r][s] * other(s, c);
                    }
                }
            }
            *this = result;
            return *this;
        }

        Matrix4x4& operator*=(float scalar) {
            Matrix4x4 result;
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    result(r, c) = m[r][c] * scalar;
                }
            }
            *this = result;
            return *this;
        }

    private:
        float Cofactor(int row, int col) const {
            int sign = ((row + col) % 2 == 0) ? 1 : -1;
            return sign * Minor(row, col);
        }

        /// @brief Extracts a Matrix3x3 from this Matrix4x4 and gets the determinant
        /// @note All elements with the row are skipped, same applies to elements with the column
        /// @param row The specified row that should be removed
        /// @param col The specified column that should be removed
        /// @returns The determinant of the extracted Matrix3x3
        float Minor(int row, int col) const {
            Matrix3x3 sub;
            int sub_r = 0;

            for (int r = 0; r < 4; r++) {
                if (r == row) continue;

                int sub_c = 0;
                for (int c = 0; c < 4; c++) {
                    if (c == col) continue;

                    sub.m[sub_r][sub_c] = m[r][c];
                    sub_c++;
                }
                sub_r++;
            }

            return sub.Determinant();
        }
    };

    inline Matrix4x4 LookAt(Vector3 eye, Vector3 target, Vector3 up) {
        Vector3 forward = (target - eye).Normalize();
        Vector3 right = forward.Cross(up).Normalize();
        Vector3 newUp = right.Cross(forward);
        
        Matrix4x4 view;
        view(0,0) = right.x;    view(0,1) = right.y;    view(0,2) = right.z;
        view(1,0) = newUp.x;    view(1,1) = newUp.y;    view(1,2) = newUp.z;
        view(2,0) = -forward.x; view(2,1) = -forward.y; view(2,2) = -forward.z;
        
        view(0,3) = -right.Dot(eye);
        view(1,3) = -newUp.Dot(eye);
        view(2,3) = forward.Dot(eye);

        return view;
    }

    inline Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b) {
        Matrix4x4 result = a;
        result += b;
        return result;
    }

    inline Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b) {
        Matrix4x4 result = a;
        result -= b;
        return result;
    }

    inline Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b) {
        Matrix4x4 result = a;
        result *= b;
        return result;
    }

    inline Matrix4x4 operator*(const Matrix4x4& a, float scalar) {
        Matrix4x4 result = a;
        result *= scalar;
        return result;
    }
}