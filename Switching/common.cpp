
#include "common.h"

std::vector<real> operator*(const real alfa, const std::vector<real>& v)
{
    std::vector<real> temp;
    int n = v.size();
    temp.resize(n);
    for (int i = 0; i < n; i++)
    {
        temp.at(i) = v.at(i) * alfa;
    }
    return temp;
}

std::vector<real> operator*(const std::vector<real>& v, real alfa)
{
    std::vector<real> temp;
    int n = v.size();
    temp.resize(n);
    for (int i = 0; i < n; i++)
    {
        temp.at(i) = v.at(i) * alfa;
    }
    return temp;
}

std::vector<real> operator + (const std::vector<real>& v1, const std::vector<real>& v2)
{
    std::vector<real> temp;
    int n = v1.size();
    temp.resize(n);
    for (int i = 0; i < n; i++)
    {
        temp.at(i) = v1.at(i) + v2.at(i);
    }
    return temp;
}

std::vector<real> operator-(const std::vector<real>& v1, const std::vector<real>& v2)
{
    std::vector<real> temp;
    int n = v1.size();
    temp.resize(n);
    for (int i = 0; i < n; i++)
    {
        temp.at(i) = v1.at(i) - v2.at(i);
    }
    return temp;
}