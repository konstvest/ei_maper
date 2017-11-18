#ifndef EI_VECTORS_H
#define EI_VECTORS_H
#include "iostream"

namespace ei {

template <class T> class vector3{
public:
    vector3(){
        x = y = z = T(0);
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

    void assign(const T& value){
        x = value;
        y = value;
        z = value;
    }

    vector3 (const T* array) {
        if (array == nullptr)
            throw std::out_of_range("array is null");
        x = *(array);
        y = *(array+1);
        z = *(array+2);
    }

    static T getDistance(const vector3 &v1, const vector3 &v2){
        T dx = v2.x - v1.x;
        T dy = v2.y - v1.y;
        T dz = v2.z - v1.z;

        return sqrt(dx * dx + dy * dy + dz * dz);
    }
    //TODO (), toArray(), ==, !=, >=, <=, >, <

    // ------------ Overloaded operators ------------
    T& operator[] (const int ind){
        switch (ind){
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }
        throw std::out_of_range("-_-");
    }

    vector3 operator[](const T* array) const{
        if (array == nullptr)
            throw std::out_of_range("array is null");
        return vector3<T>( *(array[0]), *(array[1]), *(array[2]) );
    }
    //with same vector
    vector3 operator+(const vector3& vector) const{
        return vector3<T>(x + vector.x, y + vector.y, z + vector.z);
    }
    vector3 operator*(const vector3& vector) const{
        return vector3<T>(x * vector.x, y * vector.y, z * vector.z);
    }
    vector3 operator-(const vector3& vector) const{
        return vector3<T>(x - vector.x, y - vector.y, z - vector.z);
    }
    // operator '/' may throw err or warn such as (int)1/(int)2 != (int)
    vector3 operator/(const vector3& vector) const{
        return vector3<T>(x / vector.x, y / vector.y, z / vector.z);
    }
    bool operator==(const vector3& vector) const{
        return ((x == vector.x) && (y == vector.y) && (z == vector.z));
    }
    bool operator!=(const vector3& vector) const{
        return !((x == vector.x) && (y == vector.y) && (z == vector.z));
    }
    vector3& operator+=(const vector3& vector){
        x += vector.x;
        y += vector.y;
        z += vector.z;
        return *this;
    }
    vector3& operator*=(const vector3& vector){
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
        return *this;
    }
    vector3& operator-=(const vector3& vector){
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        return *this;
    }
    //may throw err or warn
    vector3& operator/=(const vector3& vector){
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
        return *this;
    }
    //with value
    vector3& operator+(const T& value) const{
        return vector3<T>(x += value, y += value, z += value);
    }
    vector3& operator*(const T& value) const{
        return vector3<T>(x * value, y * value, z * value);
    }
    vector3& operator-(const T& value) const{
        return vector3<T>(x -= value, y -= value, z -= value);
    }
    //may throw err or warn
    vector3& operator/(const T& value) const{
        return vector3<T>(x / value, y / value, z / value);
    }

    vector3& operator+=(const T& value){
        x += value;
        y += value;
        z += value;
        return *this;
    }
    vector3& operator*=(const T& value){
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }
    vector3& operator-=(const T& value){
        x -= value;
        y -= value;
        z -= value;
        return *this;
    }
    vector3& operator/=(const T& value){
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
