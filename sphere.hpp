#pragma once

#include "common.hpp"
#include "hittable.hpp"

class sphere : public hittable {
private:
    point3 center;
    double radius;

public:
    sphere(const point3& center, double radius)
        : center{center}
        , radius{fmax(0, radius)} {}

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        // note:
        // the solution to this comes from:
        // solving for (x,y,z) in:
        // (x(t) - x_0)**2 + (y(t) - y_0)**2 + (z(t) - z_0)**2 = radius**2
        //
        // or in vector form:
        // (point(t) - center) . (point(t) - center) = radius**2
        //
        // and plugging in the equation for the ray (line):
        // point(t) = camera_center + t * ray_direction
        //
        // and solving for t, involving a quadratic equation.

        // the constants are tucked away, and optimized with the code as per the book.

        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;
        auto discriminant = h * h - a * c;

        if (discriminant < 0) {
            return false;
        }

        // Find the nearest root that lies in the acceptable range.
        auto root1 = (h - sqrt(discriminant)) / a;
        auto root2 = (h + sqrt(discriminant)) / a;

        double t{};
        if (ray_tmin < root1 && root1 < ray_tmax) {
            t = root1;
        } else if (ray_tmin < root2 && root2 < ray_tmax) {
            t = root2;
        } else {
            return false;
        }

        rec.t = t;
        rec.point = r.at(rec.t);
        rec.normal = (rec.point - center) / radius;


        //    ______
        //   /      \
        //  |   .    | -----> (normal)
        //  | sphere |
        //   \______/
        //

        vec3 outward_normal = (rec.point - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }
};
