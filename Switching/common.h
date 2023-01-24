#pragma once

#include <vector>
#include <cuComplex.h>

#define OPEN -1
#define CLOSED 1
#define STEP_OBJECTIVE 10

typedef float real;
class Complex;
typedef Complex complex;

std::vector<real> operator*(const real alfa, const std::vector<real>& v);
std::vector<real> operator*(const std::vector<real>& v, real alfa);
std::vector<real> operator+(const std::vector<real>& v1, const std::vector<real>& v2);
std::vector<real> operator-(const std::vector<real>& v1, const std::vector<real>& v2);

class Complex : public cuFloatComplex {
public:

    __device__ __host__ inline
        Complex(real x = 0, real y = 0) {
        this->x = x;
        this->y = y;
    }

    __device__ __host__ inline
        Complex operator + (Complex const& obj) {
        Complex res;
        res.x = x + obj.x;
        res.y = y + obj.y;
        return res;
    }

    __device__ __host__ inline
        Complex& operator+=(const Complex& rhs) {

        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    __device__ __host__ inline
        Complex operator - (Complex const& obj) {
        Complex res;
        res.x = x - obj.x;
        res.y = y - obj.y;
        return res;
    }

    __device__ __host__ inline
        Complex& operator-=(const Complex& rhs) {

        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    __device__ __host__ inline
        Complex operator * (Complex const& obj) {
        Complex res;
        res.x = x * obj.x - y * obj.y;
        res.y = y * obj.x + obj.y * x;
        return res;
    }

    __device__ __host__ inline
        Complex operator * (real const& val) {
        return Complex(x * val, y * val);
    }

    __device__ __host__ inline
        Complex operator / (real const& val) {
        return Complex(x/val, y/val);
    }

    __device__ __host__ inline
        Complex operator / (Complex const& val) {
        cuFloatComplex temp = cuCdivf(*this, val);
        return Complex(temp.x, temp.y);
    }

    __device__ __host__ inline
        Complex scale(real const& val) {
        return (*this) * val;
    }

    __device__ __host__ inline
        real abs() {
        return sqrtf(x * x + y * y);
    }

    __device__ __host__ inline
        real const isZero() const {
        return x == 0.0f && y == 0.0f;
    }

    __device__ __host__ inline
        real ang() {
        if (x == 0.0f) {
            return 0;
        }
        else {
            return tanf(y / x);
        }
    }

    __device__ __host__ inline
        real lessThanManhattanMetric(float a) {
        return (x < a && x > -a) && (y < a && y > -a);
    }

    __device__ __host__ inline
        Complex conj() {
        return Complex(this->x, -(this->y));
    }
    __device__ __host__ inline
        real realVal() {
        return this->x;
    }

    __device__ __host__ inline
        real imagVal() {
        return this->y;
    }
};
