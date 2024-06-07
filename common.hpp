#pragma once

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

using std::make_shared;
using std::shared_ptr;
using std::sqrt;
using std::fabs;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

template <typename T>
T lerp(T start_value, T end_value, double t) {
    return (1. - t) * start_value + t * end_value;
}

#include "vec3.hpp"
#include "color.hpp"
#include "interval.hpp"
#include "ray.hpp"
