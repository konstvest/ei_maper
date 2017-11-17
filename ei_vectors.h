#ifndef EI_VECTORS_H
#define EI_VECTORS_H

namespace ei {

template <class T> class vec3{
public:
    vec3(){
        x = y = z = 0;
    }

    vec3(T xValue, T yValue, T zValue)
    {
        x = xValue;
        y = yValue;
        z = zValue;
    }

    void reset()
    {
        x = y = z = 0;
    }

    static T getDistance(const vec3 &v1, const vec3 &v2)
    {
        T dx = v2.x - v1.x;
        T dy = v2.y - v1.y;
        T dz = v2.z - v1.z;

        return sqrt(dx * dx + dy * dy + dz * dz);
    }
    //TODO [], (), toArray(), fromArray(), ==, !=, >=, <=, >, <

    // ------------ Overloaded operators ------------
    //with same vector
    vec3 operator+(const vec3& vector) const
    {
        return vec3<T>(x + vector.x, y + vector.y, z + vector.z);
    }
    vec3 operator*(const vec3& vector) const
    {
        return vec3<T>(x * vector.x, y * vector.y, z * vector.z);
    }
    vec3 operator-(const vec3& vector) const
    {
        return vec3<T>(x - vector.x, y - vector.y, z - vector.z);
    }
    // operator '/' may throw err or warn such as (int)1/(int)2 != (int)
    vec3 operator/(const vec3& vector) const
    {
        return vec3<T>(x / vector.x, y / vector.y, z / vector.z);
    }
    vec3& operator+=(const vec3& vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;
        return *this;
    }
    vec3& operator*=(const vec3& vector)
    {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
        return *this;
    }
    vec3& operator-=(const vec3& vector)
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        return *this;
    }
    //may throw err or warn
    vec3& operator/=(const vec3& vector)
    {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
        return *this;
    }
    //with value
    vec3& operator+(const T& value) const
    {
        return vec3<T>(x += value, y += value, z += value);
    }
    vec3& operator*(const T& value) const
    {
        return vec3<T>(x * value, y * value, z * value);
    }
    vec3& operator-(const T& value) const
    {
        return vec3<T>(x -= value, y -= value, z -= value);
    }
    //may throw err or warn
    vec3& operator/(const T& value) const
    {
        return vec3<T>(x / value, y / value, z / value);
    }

    vec3& operator+=(const T& value)
    {
        x += value;
        y += value;
        z += value;
        return *this;
    }
    vec3& operator*=(const T& value)
    {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }
    vec3& operator-=(const T& value)
    {
        x -= value;
        y -= value;
        z -= value;
        return *this;
    }
    vec3& operator/=(const T& value)
    {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }
public:
    T x, y, z;
};

}

#endif // EI_VECTORS_H
