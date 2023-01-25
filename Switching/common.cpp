
#include "common.h"

bool isZero(cmplx c){
    return c.real() == 0 && c.imag() == 0;
}

std::vector<float> operator*(const float alfa, const std::vector<float>& v)
{
    std::vector<float> temp;
    int n = v.size();
    temp.resize(n);
    for (int i = 0; i < n; i++)
    {
        temp.at(i) = v.at(i) * alfa;
    }
    return temp;
}

std::vector<float> operator*(const std::vector<float>& v, float alfa)
{
    std::vector<float> temp;
    int n = v.size();
    temp.resize(n);
    for (int i = 0; i < n; i++)
    {
        temp.at(i) = v.at(i) * alfa;
    }
    return temp;
}

std::vector<float> operator + (const std::vector<float>& v1, const std::vector<float>& v2)
{
    std::vector<float> temp;
    int n = v1.size();
    temp.resize(n);
    for (int i = 0; i < n; i++)
    {
        temp.at(i) = v1.at(i) + v2.at(i);
    }
    return temp;
}

std::vector<float> operator-(const std::vector<float>& v1, const std::vector<float>& v2)
{
    std::vector<float> temp;
    int n = v1.size();
    temp.resize(n);
    for (int i = 0; i < n; i++)
    {
        temp.at(i) = v1.at(i) - v2.at(i);
    }
    return temp;
}