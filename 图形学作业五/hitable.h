#ifndef HITABLEH
#define HITABLEH
#include"ray.h"
#include"aabb.h"
class material;

struct hit_record{
	float t;
	vec3 point;
	vec3 normal;
	material *mat_ptr;
	float u;
	float v;
};

class hitable{
public:
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;
};

#endif