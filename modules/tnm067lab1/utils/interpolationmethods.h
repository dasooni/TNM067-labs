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

    // interpolate between the colors a and b
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

    // interpolate in x direction
    auto interpolatedVal1 = linear(v[0], v[1], x);
    auto interpolatedVal2 = linear(v[2], v[3], x);
    // interpolate in y direction and return
    return linear(interpolatedVal1, interpolatedVal2, y);
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


    return (1 - x) * (1 - 2 * x) * a  // (1-x)(1-2x)a
        + 4 * x * (1 - x) * b 	   // 4x(1-x)b  
        + x * (2 * x - 1) * c; 	   // x(2x-1)c
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

    // interpolate in x direction
    auto interPolatedVal1 = quadratic(v[0], v[1], v[2], x);
    auto interPolatedVal2 = quadratic(v[3], v[4], v[5], x);
    auto interPolatedVal3 = quadratic(v[6], v[7], v[8], x);
    // interpolate in y direction and return
    return quadratic(interPolatedVal1, interPolatedVal2, interPolatedVal3, y);
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

    // check if point is in the upper or lower triangle
    if (x + y < 1.0f) {
        alpha = 1.0f - x - y;
        beta = x;
        gamma = y;
        // set the first point to the upper left corner
        fA = v[0];
    // point is in the lower triangle
    } else {
        alpha = x + y - 1;
        beta = 1 - y;
        gamma = 1 - x; 
        // set the first point to the lower left corner
        fA = v[3];
    }
    
    // interpolate between the colors a and b
    return alpha * fA + beta * fB + gamma * fC;
}

}  // namespace Interpolation
}  // namespace TNM067
}  // namespace inviwo
