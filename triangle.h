#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"

class triangle : public hittable {
    public:
        triangle() {}
        triangle(point3 vertex0, point3 vertex1, point3 vertex2, shared_ptr<material> m)
            : v0(vertex0),v1(vertex1),v2(vertex2), mat_ptr(m) {};

        virtual bool hit(
            const ray& r, double tmin, double tmax, hit_record& rec) const override;

    public:
        point3 v0;
        point3 v1;
        point3 v2;
        shared_ptr<material> mat_ptr;
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    const float EPSILON = 0.0000001;
    vec3 e1, e2, h, s, q;
    float a,f,u,v;

    e1 = v1 - v0;
    e2 = v2 - v0;
    h = cross(r.direction(), e2);
    a = dot(e1,h);
    if (a > -EPSILON && a < EPSILON) {
        return false; //This ray is parallel to this triangle
    }
    f = 1.0 / a;
    s = r.origin() - v0;
    u = f * dot(s,h);

    if (u < 0.0 || u > 1.0) {
        return false;
    }
    q = cross(s,e1);
    v = f * dot(r.direction(), q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }// at this stage we can compute t to find out where the intersection point is on the line

    float t = f * dot(e2, q);
    if (t > EPSILON && (t < t_max && t > t_min)) {
        //assign stuff
        rec.t = t;
        rec.p = r.at(rec.t);
        vec3 outward_normal = unit_vector(cross(e1,e2));
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }
    else {
        return false;
    }
}

#endif
