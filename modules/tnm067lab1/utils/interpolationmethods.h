#pragma once

#include <modules/tnm067lab1/tnm067lab1moduledefine.h>
#include <inviwo/core/util/glm.h>


namespace inviwo {

template <typename T>
struct float_type {
    using type = double;
};

template <>
struct float_type<float> {
    using type = float;
};
template <>
struct float_type<vec3> {
    using type = float;
};
template <>
struct float_type<vec2> {
    using type = float;
};
template <>
struct float_type<vec4> {
    using type = float;
};

namespace TNM067 {
namespace Interpolation {

#define ENABLE_LINEAR_UNITTEST 1
template <typename T, typename F = double>
T linear(const T& a, const T& b, F x) {
    if (x <= 0) return a;
    if (x >= 1) return b;

    return a + x * (b - a);
}

// clang-format off
    /*
     2------3
     |      |
    y|  •   |
     |      |
     0------1
        x
    */
    // clang format on
#define ENABLE_BILINEAR_UNITTEST 1
template<typename T, typename F = double> 
T bilinear(const std::array<T, 4> &v, F x, F y) {

    auto dx = x - floor(x);
    auto dy = y - floor(y);


    auto interpolatedVal1 = linear(v[0], v[1], dx);
    auto interpolatedVal2 = linear(v[2], v[3], dx);

    return linear(interpolatedVal1, interpolatedVal2, dy);
}


    // clang-format off
    /* 
    a--•----b------c
    0  x    1      2
    */
// clang-format on
#define ENABLE_QUADRATIC_UNITTEST 1
template <typename T, typename F = double>
T quadratic(const T& a, const T& b, const T& c, F x) {

    return (1 - x) * (1 - 2 * x) * a + 4 * x * (1 - x) * b + x * (2 * x - 1) * c;
}

// clang-format off
    /* 
    6-------7-------8
    |       |       |
    |       |       |
    |       |       |
    3-------4-------5
    |       |       |
   y|  •    |       |
    |       |       |
    0-------1-------2
    0  x    1       2
    */
// clang-format on
#define ENABLE_BIQUADRATIC_UNITTEST 1
template <typename T, typename F = double>
T biQuadratic(const std::array<T, 9>& v, F x, F y) {

    auto dx = x - floor(x);
    auto dy = y - floor(y);

    auto interPolatedVal1 = quadratic(v[0], v[1], v[2], dx);
    auto interPolatedVal2 = quadratic(v[3], v[4], v[5], dx);
    auto interPolatedVal3 = quadratic(v[6], v[7], v[8], dx);

    return quadratic(interPolatedVal1, interPolatedVal2, interPolatedVal3, dy);
}


// clang-format off
    /*
     2---------3
     |'-.      |
     |   -,    |
   y |  •  -,  |
     |       -,|
     0---------1
        x
    */
// clang-format on
#define ENABLE_BARYCENTRIC_UNITTEST 1
template <typename T, typename F = double>
T barycentric(const std::array<T, 4>& v, F x, F y) {

    
    F alpha, beta, gamma;
    auto fA = v[0];
    auto fB = v[1];
    auto fC = v[2];

    if (x + y < 1.0f) {
        alpha = 1.0f - x - y;
        beta = x;
        gamma = y;

        fA = v[0];
    } else {
        alpha = x + y - 1;
        beta = 1 - y;
        gamma = 1 - x; 

        fA = v[3];
    }
    

    return alpha * fA + beta * fB + gamma * fC;
}

}  // namespace Interpolation
}  // namespace TNM067
}  // namespace inviwo
