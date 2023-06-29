#ifndef Vec3_H
#define Vec3_H

#include <cmath>
#include <iostream>
#include <vector>

using std::sqrt;

class Vec3 {
public:
    Vec3() : e{0, 0, 0} {}
    Vec3(float e0, float e1, float e2) : e{e0, e1, e2} {}

    inline float x() const {return e[0];}
    inline float y() const {return e[1];}
    inline float z() const {return e[2];}

    Vec3 operator-() const {return Vec3(-e[0], -e[1], -e[2]);}
    float operator[](int i) const {return e[i];}
    float& operator[](int i) { return e[i]; }

    Vec3& operator+=(const Vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3& operator*=(const float t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3& operator/=(const float t) {
        return *this *= 1/t;
    }

    float length() const {
        return sqrt(lengthSquared());
    }

    float lengthSquared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }
public:
    float e[3];

};

// Vec3 Utility Functions

inline std::ostream& operator<<(std::ostream &out, const Vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(float t, const Vec3 &v) {
    return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, float t) {
    return t * v;
}

inline Vec3 operator/(Vec3 v, float t) {
    return (1/t) * v;
}

inline float dot(const Vec3 &u, const Vec3 &v) {
    return u.e[0]*v.e[0]
         + u.e[1]*v.e[1]
         + u.e[2]*v.e[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[1]*v.e[2] - u.e[2]*v.e[1],
                u.e[2]*v.e[0] - u.e[0]*v.e[2],
                u.e[0]*v.e[1] - u.e[1]*v.e[0]);
}
inline Vec3 unitVector(Vec3 v) {
    return v / v.length();
}

inline Vec3 vectortoVec3(const float *v) {
    return Vec3(v[0], v[1], v[2]);
}

#endif //Vec3_H
