
#include "color.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#include <algorithm>
#include <fstream>

// revelant parts:
// https://raytracing.github.io/books/RayTracingInOneWeekend.html#outputanimage
// https://raytracing.github.io/books/RayTracingInOneWeekend.html#rays,asimplecamera,andbackground

template <typename T>
T lerp(T start_value, T end_value, double t) {
    return (1. - t) * start_value + t * end_value;
}

color ray_color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    auto t = (unit_direction.y() + 1) / 2;
    return lerp<color>(color{1.0, 1.0, 1.0}, color{0.5, 0.7, 1.0}, t);
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
    double viewport_width{viewport_height * image_width / static_cast<double>(image_height)};

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

    vec3 pixel_delta_u = viewport_u / viewport_width;
    vec3 pixel_delta_v = viewport_v / viewport_height;

    vec3 camera_upper_left = camera_center - viewport_u / 2 - viewport_v / 2;
    vec3 viewport_upper_left = camera_upper_left - vec3{0, 0, focal_length};

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

            ray r(camera_center, ray_direction);
            color pixel_color = ray_color(r);
            write_color(out, pixel_color);
        }
    }

    std::clog << "\r" << "Done." << std::string(sizeof("Scanlines left: 100"), ' ') << "\n";

    return 0;
}
