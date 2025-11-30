#pragma once

#include <cmath>
#include <cstdint>
namespace Cocoa::Math {
    struct Matrix3x3 {
        float m[3][3];

        float& operator()(int row, int col) {
            return m[row][col];
        }
        
        Matrix3x3() {
            m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
            m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
            m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
        }

        float Determinant() const {
            // Compute determinant via Rules of Sarrus
            return   (m[0][0] * m[1][1] * m[2][2]) 
                   + (m[0][1] * m[1][2] * m[2][0])
                   + (m[0][2] * m[1][0] * m[2][1])
                   - (m[0][2] * m[1][1] * m[2][0])
                   - (m[0][1] * m[1][0] * m[2][2])
                   - (m[0][0] * m[1][2] * m[2][1]);
        }

        Matrix3x3 Inverse() const {
            float determinant = Determinant();
            
            if (fabs(determinant) < 1e-6f) {
                return Matrix3x3();
            }

            float inverseDeterminant = 1.0f / determinant;
            Matrix3x3 result;

            result(0, 0) = (m[1][1]*m[2][2] - m[1][2]*m[2][1]) * inverseDeterminant;
            result(0, 1) = (m[0][2]*m[2][1] - m[0][1]*m[2][2]) * inverseDeterminant;
            result(0, 2) = (m[0][1]*m[1][2] - m[0][2]*m[1][1]) * inverseDeterminant;

            result(1, 0) = (m[1][2]*m[2][0] - m[1][0]*m[2][2]) * inverseDeterminant;
            result(1, 1) = (m[0][0]*m[2][2] - m[0][2]*m[2][0]) * inverseDeterminant;
            result(1, 2) = (m[0][2]*m[1][0] - m[0][0]*m[1][2]) * inverseDeterminant;

            result(2, 0) = (m[1][0]*m[2][1] - m[1][1]*m[2][0]) * inverseDeterminant;
            result(2, 1) = (m[0][1]*m[2][0] - m[0][0]*m[2][1]) * inverseDeterminant;
            result(2, 2) = (m[0][0]*m[1][1] - m[0][1]*m[1][0]) * inverseDeterminant;

            return result;
        }

        
        Matrix3x3& operator+=(const Matrix3x3& other) {
            for (uint32_t r = 0; r < 3; r++) {
                for (uint32_t c = 0; c < 3; c++) {
                    m[r][c] += m[r][c];
                }
            }
            return *this;
        }
        
        Matrix3x3& operator-=(const Matrix3x3& other) {
            for (uint32_t r = 0; r < 3; r++) {
                for (uint32_t c = 0; c < 3; c++) {
                    m[r][c] -= m[r][c];
                }
            }
            return *this;
        }

        Matrix3x3& operator*=(const Matrix3x3& other) {
            Matrix3x3 result;
            for (uint32_t r = 0; r < 3; r++) {
                for (uint32_t c = 0; c < 3; c++) {
                    result(r, c) = 0.0f;
                    for (uint32_t s = 0; s < 3; s++) {
                        result(r, c) += m[r][s] * other.m[s][c];
                    }
                }
            }
            *this = result;
            return *this;
        }
    };
    
    inline Matrix3x3 operator+(const Matrix3x3& a, const Matrix3x3& b) {
        Matrix3x3 result = a;
        result += b;
        return result;
    }

    inline Matrix3x3 operator-(const Matrix3x3& a, const Matrix3x3& b) {
        Matrix3x3 result = a;
        result -= b;
        return result;
    }

    inline Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b) {
        Matrix3x3 result = a;
        result *= b;
        return result;
    }
}