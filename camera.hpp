#pragma once

#include "common.hpp"

#include "hittable.hpp"

class camera {
public:
    camera(double aspect_ratio = 16.0 / 9.0, int image_width = 400, int samples_per_pixel = 100, int max_depth = 10)
        : aspect_ratio{aspect_ratio}
        , image_width{image_width}
        , samples_per_pixel{samples_per_pixel}
        , max_depth{max_depth} {
        initialize();
    }

    void render(const hittable& world, std::ofstream& out) {
        out << "P3" << "\n";
        out << image_width << " " << image_height << "\n";
        out << "255" << "\n";

        for (int j = 0; j < image_height; j++) {
            std::clog << "\r" << "Scanlines left: " << (image_height - j) << " " << std::flush;

            for (int i = 0; i < image_width; i++) {
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

                // make pixel color a mix of surrounding colors
                color pixel_color{0, 0, 0};
                for (int sample_count = 0; sample_count < samples_per_pixel; sample_count++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                write_color(out, pixel_samples_scale * pixel_color); // scale down pixel color
            }
        }
        std::clog << "\r" << "Done." << std::string(sizeof("Scanlines left: 100"), ' ') << "\n";
    }

private:
    int image_width;            // Rendered image width
    int image_height;           // Rendered image height
    int samples_per_pixel;      // Count of random samples for each pixel
    int max_depth;              // Maximum number of ray bounces into scene
    double aspect_ratio;        // Ratio of image width over height
    double pixel_samples_scale; // Color scale factor for a sum of pixel samples
    point3 camera_center;       // Camera center
    point3 pixel00_loc;         // Location of pixel 0, 0
    vec3 pixel_delta_u;         // Offset to pixel to the right
    vec3 pixel_delta_v;         // Offset to pixel below

    void initialize() {
        // image properties:
        // - image_height       : max(1, image_width / ideal_aspect_ratio)

        image_height = {std::max(1, static_cast<int>(image_width / aspect_ratio))};

        pixel_samples_scale = 1.0 / samples_per_pixel;

        // camera properties:
        // - approx_aspect_ratio : image_width / max(1, image_width / ideal_aspect_ratio)
        // - viewport_height     : constant
        // - viewport_width      : view_height * approx_aspect_ratio
        //
        // - camera_center       : constant
        // - focal_length        : constant dist_z(viewport, camera_center)

        double viewport_height{2.0};
        double viewport_width = viewport_height * (image_width / static_cast<double>(image_height));

        camera_center = {0, 0, 0};
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

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        vec3 viewport_upper_left = camera_center - vec3{0, 0, focal_length} - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

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

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + (i + offset.x()) * pixel_delta_u + (j + offset.y()) * pixel_delta_v;

        auto ray_origin = camera_center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray{ray_origin, ray_direction};
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-0.5,-0.5]-[+0.5,+0.5] unit square.
        return vec3{random_double() - 0.5, random_double() - 0.5, 0};
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0) {
            return color(0,0,0);
        }

        hit_record rec;

        if (world.hit(r, interval{0.001, infinity}, rec)) {
            // color red_mask = color{1., 0., 0.};
            // color green_mask = color{0., 1., 0.};
            // color blue_mask = color{0., 0., 1.};

            vec3 direction = rec.normal + random_on_hemisphere(rec.normal);

            return 0.5 * ray_color(ray{rec.point, direction}, depth - 1, world);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = (unit_direction.y() + 1) / 2;
        return lerp(color{1.0, 1.0, 1.0}, color{0.5, 0.7, 1.0}, a);
    }
};
