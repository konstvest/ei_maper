#ifndef EI_VECTORS_H
#define EI_VECTORS_H
#include "iostream"

namespace ei {

template <class T> class vector2
{
public:
    vector2()
    {
        x = y = T(0);
    }

    vector2(T value)
    {
        x = y = value;
    }

    vector2(T xValue, T yValue)
    {
        x = xValue;
        y = yValue;
    }

    void reset()
    {
        x = y = T(0);
    }

    void reset(const T& xValue, const T& yValue)
    {
        x = xValue;
        y = yValue;
    }

    void reset(const T& value)
    {
        x = value;
        y = value;
    }

    vector2 (const T* array)
    {
        if (array == nullptr)
            throw std::out_of_range("array is null");
        x = *(array);
        y = *(array+1);
    }

    static T getDistance(const vector2 &v1, const vector2 &v2)
    {
        T dx = v2.x - v1.x;
        T dy = v2.y - v1.y;

        return sqrt(dx * dx + dy * dy);
    }
    //TODO (), toArray(), >=, <=, >, <

    // ------------ Overloaded operators ------------
    T& operator[] (const int ind)
    {
        switch (ind){
        case 0: return x;
        case 1: return y;
        }
        throw std::out_of_range("-_-");
    }

    vector2 operator[](const T* array) const
    {
        if (array == nullptr)
            throw std::out_of_range("array is null");
        return vector2<T>(*(array[0]), *(array[1]));
    }
    //with same vector
    vector2 operator+(const vector2& vector) const
    {
        return vector2<T>(x + vector.x, y + vector.y);
    }
    vector2 operator*(const vector2& vector) const
    {
        return vector2<T>(x * vector.x, y * vector.y);
    }
    vector2 operator-(const vector2& vector) const
    {
        return vector2<T>(x - vector.x, y - vector.y);
    }
    vector2 operator/(const vector2& vector) const
    {
        return vector2<T>(x / vector.x, y / vector.y);
    }
    bool operator==(const vector2& vector) const
    {
        return ((x == vector.x) && (y == vector.y));
    }
    bool operator!=(const vector2& vector) const
    {
        return !((x == vector.x) && (y == vector.y));
    }
    vector2& operator+=(const vector2& vector)
    {
        x += vector.x;
        y += vector.y;
        return *this;
    }
    vector2& operator*=(const vector2& vector)
    {
        x *= vector.x;
        y *= vector.y;
        return *this;
    }
    vector2& operator-=(const vector2& vector)
    {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }
    //may throw err or warn
    vector2& operator/=(const vector2& vector)
    {
        x /= vector.x;
        y /= vector.y;
        return *this;
    }
    //with value
    vector2 operator+(const T& value) const
    {
        return vector2<T>(x + value, y + value);
    }
    vector2 operator*(const T& value) const
    {
        return vector2<T>(x * value, y * value);
    }
    vector2 operator-(const T& value) const
    {
        return vector2<T>(x - value, y - value);
    }
    //may throw err or warn
    vector2 operator/(const T& value) const
    {
        return vector2<T>(x / value, y / value);
    }

    vector2& operator+=(const T& value)
    {
        x += value;
        y += value;
        return *this;
    }
    vector2& operator*=(const T& value)
    {
        x *= value;
        y *= value;
        return *this;
    }
    vector2& operator-=(const T& value)
    {
        x -= value;
        y -= value;
        return *this;
    }
    vector2& operator/=(const T& value)
    {
        x /= value;
        y /= value;
        return *this;
    }
public:
    T x, y;
};


template <class T> class vector3
{
public:
    vector3()
    {
        x = y = z = T(0);
    }

    vector3(T value)
    {
        x = y = z = value;
    }

    vector3(T xValue, T yValue, T zValue)
    {
        x = xValue;
        y = yValue;
        z = zValue;
    }

    void reset()
    {
        x = y = z = T(0);
    }

    void reset(const T& xValue, const T& yValue, const T& zValue)
    {
        x = xValue;
        y = yValue;
        z = zValue;
    }

    void reset(const T& value)
    {
        x = value;
        y = value;
        z = value;
    }

    vector3 (const T* array)
    {
        if (array == nullptr)
            throw std::out_of_range("array is null");
        x = *(array);
        y = *(array+1);
        z = *(array+2);
    }

    static T getDistance(const vector3 &v1, const vector3 &v2)
    {
        T dx = v2.x - v1.x;
        T dy = v2.y - v1.y;
        T dz = v2.z - v1.z;

        return sqrt(dx * dx + dy * dy + dz * dz);
    }
    //TODO (), toArray(), >=, <=, >, <

    // ------------ Overloaded operators ------------
    T& operator[] (const int ind)
    {
        switch (ind){
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }
        throw std::out_of_range("-_-");
    }

    vector3 operator[](const T* array) const
    {
        if (array == nullptr)
            throw std::out_of_range("array is null");
        return vector3<T>( *(array[0]), *(array[1]), *(array[2]) );
    }
    //with same vector
    vector3 operator+(const vector3& vector) const
    {
        return vector3<T>(x + vector.x, y + vector.y, z + vector.z);
    }
    vector3 operator*(const vector3& vector) const
    {
        return vector3<T>(x * vector.x, y * vector.y, z * vector.z);
    }
    vector3 operator-(const vector3& vector) const
    {
        return vector3<T>(x - vector.x, y - vector.y, z - vector.z);
    }
    // operator '/' may throw err or warn such as (int)1/(int)2 != (int)
    vector3 operator/(const vector3& vector) const
    {
        return vector3<T>(x / vector.x, y / vector.y, z / vector.z);
    }
    bool operator==(const vector3& vector) const
    {
        return ((x == vector.x) && (y == vector.y) && (z == vector.z));
    }
    bool operator!=(const vector3& vector) const
    {
        return !((x == vector.x) && (y == vector.y) && (z == vector.z));
    }
    vector3& operator+=(const vector3& vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;
        return *this;
    }
    vector3& operator*=(const vector3& vector)
    {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
        return *this;
    }
    vector3& operator-=(const vector3& vector)
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        return *this;
    }
    //may throw err or warn
    vector3& operator/=(const vector3& vector)
    {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
        return *this;
    }
    //with value
    vector3 operator+(const T& value) const
    {
        return vector3<T>(x + value, y + value, z + value);
    }
    vector3 operator*(const T& value) const
    {
        return vector3<T>(x * value, y * value, z * value);
    }
    vector3 operator-(const T& value) const
    {
        return vector3<T>(x - value, y - value, z - value);
    }
    //may throw err or warn
    vector3 operator/(const T& value) const
    {
        return vector3<T>(x / value, y / value, z / value);
    }

    vector3& operator+=(const T& value)
    {
        x += value;
        y += value;
        z += value;
        return *this;
    }
    vector3& operator*=(const T& value)
    {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }
    vector3& operator-=(const T& value)
    {
        x -= value;
        y -= value;
        z -= value;
        return *this;
    }
    vector3& operator/=(const T& value)
    {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }
public:
    T x, y, z;
};

//=============================================
//vector of xyzw
template <class T> class vector4
{
public:
    vector4()
    {
        x = y = z = w = T(0);
    }

    vector4(T value)
    {
        x = y = z = w = value;
    }

    vector4(T xValue, T yValue, T zValue, T wValue)
    {
        x = xValue;
        y = yValue;
        z = zValue;
        w = wValue;
    }

    void reset()
    {
        x = y = z = w = T(0);
    }

    void reset(const T& xValue, const T& yValue, const T& zValue, const T& wValue)
    {
        x = xValue;
        y = yValue;
        z = zValue;
        w = wValue;
    }

    void reset(const T& value)
    {
        x = value;
        y = value;
        z = value;
        w = value;
    }

    vector4 (const T* array)
    {
        if (array == nullptr)
            throw std::out_of_range("array is null");
        x = *(array);
        y = *(array+1);
        z = *(array+2);
        w = *(array+3);
    }

    //TODO (), toArray(), ==, !=, >=, <=, >, <

    // ------------ Overloaded operators ------------
    T& operator[] (const int ind)
    {
        switch (ind)
        {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        }
        throw std::out_of_range("-_-");
    }

    vector4 operator[](const T* array) const
    {
        if (array == nullptr)
            throw std::out_of_range("array is null");
        return vector4<T>( *(array[0]), *(array[1]), *(array[2]), *(array[3]) );
    }
    //with same vector
    vector4 operator+(const vector4& vector) const
    {
        return vector4<T>(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
    }
    vector4 operator*(const vector4& vector) const
    {
        return vector4<T>(x * vector.x, y * vector.y, z * vector.z, w * vector.w);
    }
    vector4 operator-(const vector4& vector) const
    {
        return vector4<T>(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
    }
    vector4 operator/(const vector4& vector) const
    {
        return vector4<T>(x / vector.x, y / vector.y, z / vector.z, w / vector.w);
    }
    bool operator==(const vector4& vector) const
    {
        return ((x == vector.x) && (y == vector.y) && (z == vector.z) && (w = vector.w));
    }
    bool operator!=(const vector4& vector) const
    {
        return !((x == vector.x) && (y == vector.y) && (z == vector.z) && (w == vector.w));
    }
    vector4& operator+=(const vector4& vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;
        w += vector.w;
        return *this;
    }
    vector4& operator*=(const vector4& vector)
    {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
        w *= vector.w;
        return *this;
    }
    vector4& operator-=(const vector4& vector)
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        w -= vector.w;
        return *this;
    }
    vector4& operator/=(const vector4& vector)
    {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
        w /= vector.w;
        return *this;
    }
    //with value
    vector4 operator+(const T& value) const
    {
        return vector4<T>(x + value, y + value, z + value, w + value);
    }
    vector4 operator*(const T& value) const
    {
        return vector4<T>(x * value, y * value, z * value, w * value);
    }
    vector4 operator-(const T& value) const
    {
        return vector4<T>(x - value, y - value, z - value, w - value);
    }
    //may throw err or warn
    vector4 operator/(const T& value) const
    {
        return vector4<T>(x / value, y / value, z / value, w / value);
    }

    vector4& operator+=(const T& value)
    {
        x += value;
        y += value;
        z += value;
        w += value;
        return *this;
    }
    vector4& operator*=(const T& value)
    {
        x *= value;
        y *= value;
        z *= value;
        z *= value;
        return *this;
    }
    vector4& operator-=(const T& value)
    {
        x -= value;
        y -= value;
        z -= value;
        w -= value;
        return *this;
    }
    vector4& operator/=(const T& value)
    {
        x /= value;
        y /= value;
        z /= value;
        w /= value;
        return *this;
    }
public:
    T x, y, z, w;
};

}

#endif // EI_VECTORS_H
