#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "triangle.h"
#include "plane.h"
#include "camera.h"
#include "material.h"
#include "vec3.h"

#include <iostream>

//test shadow function
bool in_shadow(point3 prev_hit, const hittable& world, point3 light) {
  hit_record shadow_rec;
  ray light_ray = ray(prev_hit, light - prev_hit);

    return world.hit(light_ray, 0.001, infinity, shadow_rec);
}

//Returns a color for a single input ray
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
          //check for shadows TODO: create light class
          if (in_shadow(rec.p, world, point3(-3,0.5,-5) )) {
            attenuation /= 2;
          }
          return attenuation * ray_color(scattered, world, depth-1);
        }
            
        return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
  std::srand ( unsigned ( std::time(0) ) );

  //Image
  const auto aspect_ratio = 16.0/ 9.0;
  const int image_width = 1000;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 100;
  const bool use_multi_jittered = false;
  const int max_depth = 50;

  // World
  // TODO: World/Scene class
  hittable_list world;
  auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
  auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.1);
  auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

  
  //world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
  world.add(make_shared<plane>(vec3( 0.0, 1.0, 0.0), point3(0.0, -0.5, -2.0), material_ground));
  world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
  world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
  //world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
  world.add(make_shared<triangle>(point3( 1.0, 0.5, -1.0), point3( 1.5, -0.5, -1.0), point3( 0.5, -0.5, -1.0), material_left));
  // world.add(make_shared<triangle>(point3( 1.0, 0.5, -1.0), point3( 0.5, -0.5, -1.5), 
  //            point3( 1.0, -0.5, -0.5), material_left));



  //Camera
  camera cam;

  //Render
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = image_height-1; j >= 0; --j) {
      std::cerr << "\rLines remaining: " << j << ' ' << std::flush;
      for (int i = 0; i < image_width; ++i) {
        color pixel_color(0,0,0);
        if (use_multi_jittered) {
          vector<pair<double,double>> coord = vec3::multi_jittered_samples(samples_per_pixel);
          for (int s = 0; s < samples_per_pixel; ++s) {
            auto u = (i + coord[s].first) / (image_width-1);
            auto v = (j + coord[s].second) / (image_height-1);
            ray r = cam.get_ray(u, v);
            pixel_color += ray_color(r, world, max_depth);
          }
        }
        else {
          for (int s = 0; s < samples_per_pixel; ++s) {
            auto u = (i + random_double()) / (image_width-1);
            auto v = (j + random_double()) / (image_height-1);
            ray r = cam.get_ray(u, v);
            pixel_color += ray_color(r, world, max_depth);
          }
        }
        write_color(std::cout, pixel_color, samples_per_pixel);
      }
  }

  std::cerr << "\nDone.\n";
}
