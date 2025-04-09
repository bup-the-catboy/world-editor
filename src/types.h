#ifndef TYPES_H
#define TYPES_H

#include "block.h"

#include <math.h>

#define WORLD_SIZE 32
#define TILEMAP_WIDTH  80
#define TILEMAP_HEIGHT 64

template<typename T> struct TVec2 {
    T x, y;
    TVec2(): x(0), y(0) {}
    TVec2(T x, T y): x(x), y(y) {}
    TVec2 operator +(const TVec2& other) const {
        return TVec2(x + other.x, y + other.y);
    }
    TVec2 operator -(const TVec2& other) const {
        return TVec2(x - other.x, y - other.y);
    }
    TVec2 operator -() const {
        return TVec2(-x, -y);
    }
    TVec2 operator *(const T& num) const {
        return TVec2(x * num, y * num);
    }
    TVec2 operator /(const T& num) const {
        return TVec2(x / num, y / num);
    }
    TVec2 operator >>(const TVec2& other) const {
        return (*this - other).length();
    }
    TVec2 operator <<(const TVec2& other) const { // alias for >>
        return *this >> other;
    }
    bool operator ==(const TVec2& other) const {
        return x == other.x && y == other.y;
    }
    bool operator !=(const TVec2& other) const {
        return !(*this != other);
    }
    T dot(const TVec2& other) const {
        return x * other.x + y * other.y;
    }
    TVec2 normalized() const {
        if (length() == 0) return TVec2(0, 0);
        return *this / length();
    }
    double length() const {
        return sqrt(x * x + y * y);
    }
    static TVec2 lerp(const TVec2& from, const TVec2& to, float t) {
        return TVec2(
            (to.x - from.x) * t + from.x,
            (to.y - from.y) * t + from.y
        );
    }

    static TVec2 zero()  { return TVec2( 0,  0); }
    static TVec2 one()   { return TVec2( 1,  1); }
    static TVec2 pos_x() { return TVec2(+1,  0); }
    static TVec2 neg_x() { return TVec2(-1,  0); }
    static TVec2 pos_y() { return TVec2( 0, +1); }
    static TVec2 neg_y() { return TVec2( 0, -1); }
};

template<typename T> struct TVec3 {
    T x, y, z;
    TVec3(): x(0), y(0), z(0) {}
    TVec3(T x, T y, T z): x(x), y(y), z(z) {}
    TVec3 operator +(const TVec3& other) const {
        return TVec3(x + other.x, y + other.y, z + other.z);
    }
    TVec3& operator +=(const TVec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }
    TVec3 operator -(const TVec3& other) const {
        return TVec3(x - other.x, y - other.y, z - other.z);
    }
    TVec3 operator -() const {
        return TVec3(-x, -y, -z);
    }
    TVec3& operator -=(const TVec3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
    TVec3 operator *(const T& num) const {
        return TVec3(x * num, y * num, z * num);
    }
    TVec3& operator *=(const T& num) {
        x *= num; y *= num; z *= num;
        return *this;
    }
    TVec3 operator /(const T& num) const {
        return TVec3(x / num, y / num, z / num);
    }
    TVec3& operator /=(const T& num) {
        x /= num; y /= num; z /= num;
        return *this;
    }
    TVec3 operator >>(const TVec3& other) const {
        return (*this - other).length();
    }
    TVec3 operator <<(const TVec3& other) const { // alias for >>
        return *this >> other;
    }
    bool operator ==(const TVec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator !=(const TVec3& other) const {
        return !(*this != other);
    }
    T dot(const TVec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    TVec3 cross(const TVec3& other) const {
        TVec3 out = TVec3();
        out.x = y * other.z - other.y * z;
        out.y = z * other.x - other.z * x;
        out.z = x * other.y - other.x * y;
        return out;
    }
    TVec3 normalized() const {
        if (length() == 0) return TVec3(0, 0, 0);
        return *this / length();
    }
    TVec3 to_euler(double* distance = NULL) const {
        if (distance) *distance = sqrt(x * x + y * y + z * z);
        return TVec3(
            atan2(sqrt(x * x + z * z), y),
            atan2(z, x),
            0
        );
    }
    TVec3 to_direction(double distance = 1.0) const {
        return TVec3(
            distance * cos(x) * sin(y),
            distance * sin(x),
            distance * cos(x) * cos(y)
        );
    }
    double length() const {
        return sqrt(x * x + y * y + z * z);
    }
    static TVec3 lerp(const TVec3& from, const TVec3& to, float t) {
        return TVec3(
            (to.x - from.x) * t + from.x,
            (to.y - from.y) * t + from.y,
            (to.z - from.z) * t + from.z
        );
    }

    static TVec3 zero()  { return TVec3( 0,  0,  0); }
    static TVec3 one ()  { return TVec3( 1,  1,  1); }
    static TVec3 pos_x() { return TVec3(+1,  0,  0); }
    static TVec3 neg_x() { return TVec3(-1,  0,  0); }
    static TVec3 pos_y() { return TVec3( 0, +1,  0); }
    static TVec3 neg_y() { return TVec3( 0, -1,  0); }
    static TVec3 pos_z() { return TVec3( 0,  0, +1); }
    static TVec3 neg_z() { return TVec3( 0,  0, -1); }
};

template<typename T> struct TVec4 {
    T x, y, z, w;
    TVec4(): x(0), y(0), z(0), w(0) {}
    TVec4(T x, T y, T z, T w): x(x), y(y), z(z), w(w) {}
    TVec4(const TVec3<T>& vec, T w): x(vec.x), y(vec.y), z(vec.z), w(w) {}
    TVec4(const TVec3<T>& vec):      x(vec.x), y(vec.y), z(vec.z), w(1) {}
    TVec3<T> vec3() const {
        return TVec3<T>(x, y, z);
    }
    TVec4<T> divide() const {
        if (w == 0) return *this;
        return TVec4<T>(x / w, y / w, z / w, 1);
    }
};

typedef TVec2<float> Vec2;
typedef TVec2<int> IVec2;
typedef TVec3<float> Vec3;
typedef TVec3<int> IVec3;
typedef TVec4<float> Vec4;
typedef TVec4<int> IVec4;

struct Mtx {
    struct MtxRow {
        float& operator[](int col) {
            return data[col * 4 + row];
        }
        const float operator[](int col) const {
            return data[col * 4 + row];
        }
        private: friend Mtx;
            MtxRow(float* data, int row): data(data), row(row) {}
            float* data;
            int row;
    };

    static Mtx identity() {
        Mtx mtx = Mtx();
        mtx[0][0] = mtx[1][1] = mtx[2][2] = mtx[3][3] = 1;
        return mtx;
    }
    static Mtx scale(float x, float y, float z) {
        Mtx mtx = identity();
        mtx[0][0] = x;
        mtx[1][1] = y;
        mtx[2][2] = z;
        return mtx;
    }
    static Mtx scale(float x) {
        return scale(x, x, x);
    }
    static Mtx translate(float x, float y, float z) {
        Mtx mtx = identity();
        mtx[0][3] = x;
        mtx[1][3] = y;
        mtx[2][3] = z;
        return mtx;
    }
    static Mtx quaternion(float x, float y, float z, float angle) {
        Mtx mtx = identity();

        float s = sin(angle / 2);
        float c = cos(angle / 2);
        Vec3 n = Vec3(x, y, z).normalized();

        float qw = c;
        float qx = n.x * s;
        float qy = n.y * s;
        float qz = n.z * s;

        float xx = qx * qx * 2;
        float yy = qy * qy * 2;
        float zz = qz * qz * 2;
        float xy = qx * qy * 2;
        float xz = qx * qz * 2;
        float yz = qy * qz * 2;
        float wx = qw * qx * 2;
        float wy = qw * qy * 2;
        float wz = qw * qz * 2;

        mtx[0][0] = 1 - (yy + zz);
        mtx[0][1] = (xy - wz);
        mtx[0][2] = (xz + wy);

        mtx[1][0] = (xy + wz);
        mtx[1][1] = 1 - (xx + zz);
        mtx[1][2] = (yz - wx);

        mtx[2][0] = (xz - wy);
        mtx[2][1] = (yz + wx);
        mtx[2][2] = 1.0f - (xx + yy);

        return mtx;
    }
    static Mtx pitch(float angle) {
        return quaternion(Vec3::pos_x(), angle);
    }
    static Mtx yaw(float angle) {
        return quaternion(Vec3::pos_y(), angle);
    }
    static Mtx roll(float angle) {
        return quaternion(Vec3::pos_z(), angle);
    }
    static Mtx perspective(float fov, float aspect, float near, float far) {
        Mtx mtx = identity();
        float f = 1 / tan(fov / 2);
        float range = near - far;
        mtx[0][0] = f / aspect;
        mtx[1][1] = f;
        mtx[2][2] = (far + near) / range;
        mtx[2][3] = (2 * far * near) / range;
        mtx[3][2] = -1;
        mtx[3][3] = 0;
        return mtx;
    }
    static Mtx orthographic(float left, float right, float bottom, float top, float near, float far) {
        Mtx mtx = identity();
        mtx[0][0] = 2 / (right - left);
        mtx[1][1] = 2 / (top - bottom);
        mtx[2][2] = -2 / (far - near);
        mtx[0][3] = -(right + left) / (right - left);
        mtx[1][3] = -(top + bottom) / (top - bottom);
        mtx[2][3] = -(far + near) / (far - near);
        return mtx;
    }
    template<typename T> static Mtx scale(const TVec3<T>& vec) {
        return scale(vec.x, vec.y, vec.z);
    }
    template<typename T> static Mtx translate(const TVec3<T>& vec) {
        return translate(vec.x, vec.y, vec.z);
    }
    template<typename T> static Mtx quaternion(const TVec3<T>& vec, float angle) {
        return quaternion(vec.x, vec.y, vec.z, angle);
    }
    Mtx operator *(const Mtx& other) const {
        Mtx mtx = Mtx();
        mtx[0][0] = (*this)[0][0] * other[0][0] + (*this)[0][1] * other[1][0] + (*this)[0][2] * other[2][0] + (*this)[0][3] * other[3][0];
        mtx[0][1] = (*this)[0][0] * other[0][1] + (*this)[0][1] * other[1][1] + (*this)[0][2] * other[2][1] + (*this)[0][3] * other[3][1];
        mtx[0][2] = (*this)[0][0] * other[0][2] + (*this)[0][1] * other[1][2] + (*this)[0][2] * other[2][2] + (*this)[0][3] * other[3][2];
        mtx[0][3] = (*this)[0][0] * other[0][3] + (*this)[0][1] * other[1][3] + (*this)[0][2] * other[2][3] + (*this)[0][3] * other[3][3];
        mtx[1][0] = (*this)[1][0] * other[0][0] + (*this)[1][1] * other[1][0] + (*this)[1][2] * other[2][0] + (*this)[1][3] * other[3][0];
        mtx[1][1] = (*this)[1][0] * other[0][1] + (*this)[1][1] * other[1][1] + (*this)[1][2] * other[2][1] + (*this)[1][3] * other[3][1];
        mtx[1][2] = (*this)[1][0] * other[0][2] + (*this)[1][1] * other[1][2] + (*this)[1][2] * other[2][2] + (*this)[1][3] * other[3][2];
        mtx[1][3] = (*this)[1][0] * other[0][3] + (*this)[1][1] * other[1][3] + (*this)[1][2] * other[2][3] + (*this)[1][3] * other[3][3];
        mtx[2][0] = (*this)[2][0] * other[0][0] + (*this)[2][1] * other[1][0] + (*this)[2][2] * other[2][0] + (*this)[2][3] * other[3][0];
        mtx[2][1] = (*this)[2][0] * other[0][1] + (*this)[2][1] * other[1][1] + (*this)[2][2] * other[2][1] + (*this)[2][3] * other[3][1];
        mtx[2][2] = (*this)[2][0] * other[0][2] + (*this)[2][1] * other[1][2] + (*this)[2][2] * other[2][2] + (*this)[2][3] * other[3][2];
        mtx[2][3] = (*this)[2][0] * other[0][3] + (*this)[2][1] * other[1][3] + (*this)[2][2] * other[2][3] + (*this)[2][3] * other[3][3];
        mtx[3][0] = (*this)[3][0] * other[0][0] + (*this)[3][1] * other[1][0] + (*this)[3][2] * other[2][0] + (*this)[3][3] * other[3][0];
        mtx[3][1] = (*this)[3][0] * other[0][1] + (*this)[3][1] * other[1][1] + (*this)[3][2] * other[2][1] + (*this)[3][3] * other[3][1];
        mtx[3][2] = (*this)[3][0] * other[0][2] + (*this)[3][1] * other[1][2] + (*this)[3][2] * other[2][2] + (*this)[3][3] * other[3][2];
        mtx[3][3] = (*this)[3][0] * other[0][3] + (*this)[3][1] * other[1][3] + (*this)[3][2] * other[2][3] + (*this)[3][3] * other[3][3];
        return mtx;
    }
    template<typename T> TVec4<T> operator *(const TVec4<T>& other) const {
        TVec4<T> vec = TVec4<T>();
        vec.x = other.x * (*this)[0][0] + other.y * (*this)[0][1] + other.z * (*this)[0][2] + other.w * (*this)[0][3];
        vec.y = other.x * (*this)[1][0] + other.y * (*this)[1][1] + other.z * (*this)[1][2] + other.w * (*this)[1][3];
        vec.z = other.x * (*this)[2][0] + other.y * (*this)[2][1] + other.z * (*this)[2][2] + other.w * (*this)[2][3];
        vec.w = other.x * (*this)[3][0] + other.y * (*this)[3][1] + other.z * (*this)[3][2] + other.w * (*this)[3][3];
        return vec;
    }
    template<typename T> TVec4<T> operator *(const TVec3<T>& other) const {
        return *this * TVec4<T>(other.x, other.y, other.z, 1);
    }
    MtxRow operator[](int row) {
        return MtxRow(m, row);
    }
    const MtxRow operator[](int row) const {
        return MtxRow(const_cast<float*>(m), row);
    }
    Mtx inv() const {
        Mtx i = Mtx();
        Mtx m = *this;
        i[0][0] =  m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[2][3] * m[3][2] - m[2][1] * m[1][2] * m[3][3] + m[2][1] * m[1][3] * m[3][2] + m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[1][3] * m[2][2];
        i[1][0] = -m[1][0] * m[2][2] * m[3][3] + m[1][0] * m[2][3] * m[3][2] + m[2][0] * m[1][2] * m[3][3] - m[2][0] * m[1][3] * m[3][2] - m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[1][3] * m[2][2];
        i[2][0] =  m[1][0] * m[2][1] * m[3][3] - m[1][0] * m[2][3] * m[3][1] - m[2][0] * m[1][1] * m[3][3] + m[2][0] * m[1][3] * m[3][1] + m[3][0] * m[1][1] * m[2][3] - m[3][0] * m[1][3] * m[2][1];
        i[3][0] = -m[1][0] * m[2][1] * m[3][2] + m[1][0] * m[2][2] * m[3][1] + m[2][0] * m[1][1] * m[3][2] - m[2][0] * m[1][2] * m[3][1] - m[3][0] * m[1][1] * m[2][2] + m[3][0] * m[1][2] * m[2][1];
        i[0][1] = -m[0][1] * m[2][2] * m[3][3] + m[0][1] * m[2][3] * m[3][2] + m[2][1] * m[0][2] * m[3][3] - m[2][1] * m[0][3] * m[3][2] - m[3][1] * m[0][2] * m[2][3] + m[3][1] * m[0][3] * m[2][2];
        i[1][1] =  m[0][0] * m[2][2] * m[3][3] - m[0][0] * m[2][3] * m[3][2] - m[2][0] * m[0][2] * m[3][3] + m[2][0] * m[0][3] * m[3][2] + m[3][0] * m[0][2] * m[2][3] - m[3][0] * m[0][3] * m[2][2];
        i[2][1] = -m[0][0] * m[2][1] * m[3][3] + m[0][0] * m[2][3] * m[3][1] + m[2][0] * m[0][1] * m[3][3] - m[2][0] * m[0][3] * m[3][1] - m[3][0] * m[0][1] * m[2][3] + m[3][0] * m[0][3] * m[2][1];
        i[3][1] =  m[0][0] * m[2][1] * m[3][2] - m[0][0] * m[2][2] * m[3][1] - m[2][0] * m[0][1] * m[3][2] + m[2][0] * m[0][2] * m[3][1] + m[3][0] * m[0][1] * m[2][2] - m[3][0] * m[0][2] * m[2][1];
        i[0][2] =  m[0][1] * m[1][2] * m[3][3] - m[0][1] * m[1][3] * m[3][2] - m[1][1] * m[0][2] * m[3][3] + m[1][1] * m[0][3] * m[3][2] + m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[0][3] * m[1][2];
        i[1][2] = -m[0][0] * m[1][2] * m[3][3] + m[0][0] * m[1][3] * m[3][2] + m[1][0] * m[0][2] * m[3][3] - m[1][0] * m[0][3] * m[3][2] - m[3][0] * m[0][2] * m[1][3] + m[3][0] * m[0][3] * m[1][2];
        i[2][2] =  m[0][0] * m[1][1] * m[3][3] - m[0][0] * m[1][3] * m[3][1] - m[1][0] * m[0][1] * m[3][3] + m[1][0] * m[0][3] * m[3][1] + m[3][0] * m[0][1] * m[1][3] - m[3][0] * m[0][3] * m[1][1];
        i[3][2] = -m[0][0] * m[1][1] * m[3][2] + m[0][0] * m[1][2] * m[3][1] + m[1][0] * m[0][1] * m[3][2] - m[1][0] * m[0][2] * m[3][1] - m[3][0] * m[0][1] * m[1][2] + m[3][0] * m[0][2] * m[1][1];
        i[0][3] = -m[0][1] * m[1][2] * m[2][3] + m[0][1] * m[1][3] * m[2][2] + m[1][1] * m[0][2] * m[2][3] - m[1][1] * m[0][3] * m[2][2] - m[2][1] * m[0][2] * m[1][3] + m[2][1] * m[0][3] * m[1][2];
        i[1][3] =  m[0][0] * m[1][2] * m[2][3] - m[0][0] * m[1][3] * m[2][2] - m[1][0] * m[0][2] * m[2][3] + m[1][0] * m[0][3] * m[2][2] + m[2][0] * m[0][2] * m[1][3] - m[2][0] * m[0][3] * m[1][2];
        i[2][3] = -m[0][0] * m[1][1] * m[2][3] + m[0][0] * m[1][3] * m[2][1] + m[1][0] * m[0][1] * m[2][3] - m[1][0] * m[0][3] * m[2][1] - m[2][0] * m[0][1] * m[1][3] + m[2][0] * m[0][3] * m[1][1];
        i[3][3] =  m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[1][2] * m[2][1] - m[1][0] * m[0][1] * m[2][2] + m[1][0] * m[0][2] * m[2][1] + m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[0][2] * m[1][1];
        float d =  m[0][0] * i[0][0] + m[0][1] * i[1][0] + m[0][2] * i[2][0] + m[0][3] * i[3][0];
        if (d == 0) return Mtx::identity();
        for (int p = 0; p < 16; p++) i.m[p] /= d;
        return i;
    }
    const float* data() const {
        return m;
    }
private:
    float m[16] {};
    Mtx() {}
};

struct Selection {
    IVec3 pos;
    IVec3 normal;
};

struct Angle {
    constexpr static float rad = M_PI / 180;
    constexpr static float deg = 180 / M_PI;
};

typedef BlockID World[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];

#endif