#ifndef PLANE_H
#define PLANE_H

#include "hittable.h"
#include "vec3.h"

class plane : public hittable {
    public:
        plane() {}
        plane(vec3 norm, point3 point, shared_ptr<material> m)
            : n(norm),p(point), mat_ptr(m) {};

        virtual bool hit(
            const ray& r, double tmin, double tmax, hit_record& rec) const override;

    public:
        vec3 n;
        point3 p;
        shared_ptr<material> mat_ptr;
};

bool plane::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {


    const float EPSILON = 0.0000001;
    auto den = dot(r.dir, n);
    if (den > -EPSILON && den < EPSILON) {
        return false;
    }
    auto t = dot((p - r.orig), n) / den;
    rec.t = t;
    if (t < t_min || t > t_max) {
        return false;
    }
    rec.p = r.at(rec.t);
    vec3 outward_normal = unit_vector(n);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif