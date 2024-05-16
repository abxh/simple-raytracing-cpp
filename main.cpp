
#include "color.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#include <fstream>

// revelant parts:
// https://raytracing.github.io/books/RayTracingInOneWeekend.html#outputanimage
// https://raytracing.github.io/books/RayTracingInOneWeekend.html#rays,asimplecamera,andbackground

double hit_sphere(const point3& center, double radius, const ray& r) {

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

    vec3 oc = center - r.origin();

    // unoptimized:
    // auto a = dot(r.direction(), r.direction());
    // auto b = -2.0 * dot(r.direction(), oc);
    // auto c = dot(oc, oc) - radius * radius;
    //
    // auto discriminant = b * b - 4 * a * c;
    //
    // if (discriminant < 0) {
    //     return -1.;
    // } else {
    //     auto t = (-b - sqrt(discriminant)) / (2.0 * a);
    //     return t;
    // }

    // optimized version (from the book):
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = h * h - a * c;

    if (discriminant < 0) {
        return -1.0;
    } else {
        auto t = (h - sqrt(discriminant)) / a;
        return t;
    }
}

template <typename T>
T lerp(T start_value, T end_value, double t) {
    return (1. - t) * start_value + t * end_value;
}

color ray_color(const ray& r) {
    double t = hit_sphere(point3{0, 0, -1}, 0.5, r);
    if (t >= 0.0) {
        color red_mask = color{1., 0., 0.};
        color green_mask = color{0., 1., 0.};
        color blue_mask = color{0., 0., 1.};

        vec3 sphere_normal = unit_vector(r.at(t) - vec3(0, 0, -1));
        return static_cast<color>((sphere_normal + vec3{1, 1, 1}) / 2.) * red_mask;
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto a = (unit_direction.y() + 1) / 2;
    return lerp<color>(color{1.0, 1.0, 1.0}, color{0.5, 0.7, 1.0}, a);
}

int main() {
    // image properties:
    // - ideal_aspect_ratio : constant (width / height)
    // - image_width        : constant
    // - image_height       : max(1, image_width / ideal_aspect_ratio)

    double aspect_ratio{16.0 / 9.0};
    int image_width{400};
    int image_height{std::max(1, static_cast<int>(image_width / aspect_ratio))};

    // camera properties:
    // - approx_aspect_ratio : image_width / max(1, image_width / ideal_aspect_ratio)
    // - viewport_height     : constant
    // - viewport_width      : view_height * approx_aspect_ratio
    //
    // - camera_center       : constant
    // - focal_length        : constant dist_z(viewport, camera_center)

    double viewport_height{2.0};
    double viewport_width{viewport_height * (image_width / static_cast<double>(image_height))};

    point3 camera_center{0, 0, 0};
    double focal_length{1.0};

    // viewport properties:
    // (0,0)
    //   .-----------> viewport_u
    //   | . . . . . .
    //   | . . . . . .
    //   | . . . . . .
    //   | . . . . . .
    //   | . . . . . .
    //  \/
    // viewport_v

    vec3 viewport_u{viewport_width, 0, 0};
    vec3 viewport_v{0, -viewport_height, 0}; // - because y-axis points downwards

    // pixel properties:
    // - pixel_delta_u : (viewport_width / image_width, 0, 0)
    // - pixel_delta_v : (0, -viewport_height / image_height, 0)
    //
    // - viewport_upper_left : camera_center - (0, 0, focal_length) - viewport_u / 2 - viewport_v / 2
    // - pixel00_loc : center(pixel00)

    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    vec3 viewport_upper_left = camera_center - vec3{0, 0, focal_length} - viewport_u / 2 - viewport_v / 2;
    vec3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    std::ofstream out("image.ppm");

    out << "P3" << "\n";
    out << image_width << " " << image_height << "\n";
    out << "255" << "\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\r" << "Scanlines left: " << (image_height - j) << " " << std::flush;

        for (int i = 0; i < image_width; i++) {
            vec3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            vec3 ray_direction = pixel_center - camera_center; // camera_center (tail) to pixel_center (head)

            //           viewport
            //             |
            //  cam  ray   |
            //   []------> |
            //             |
            //             |
            //    <------->
            //   focal_length

            // ray:
            //
            //    orig   (t * dir)
            // ----.=============>----
            //

            ray r(camera_center, ray_direction);
            color pixel_color = ray_color(r);
            write_color(out, pixel_color);
        }
    }

    std::clog << "\r" << "Done." << std::string(sizeof("Scanlines left: 100"), ' ') << "\n";

    return 0;
}
