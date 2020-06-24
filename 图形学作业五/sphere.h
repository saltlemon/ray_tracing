#ifndef SPHEREH
#define SPHEREH
#define M_PI 3.1415926535
#include"hitable.h"
#include"material.h"

void get_sphere_uv(const vec3& p, float& u, float& v) {
	auto phi = atan2(p.z(), p.x());
	auto theta = asin(p.y());
	u = 1 - (phi + M_PI) / (2 * M_PI);
	v = (theta + M_PI / 2) / M_PI;
}

class sphere :public hitable{
public:
	sphere(){}
	sphere(vec3 cen, float r, material* mat) :center(cen), radius(r),mat_ptr(mat){};
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	vec3 center;
	float radius;
	material *mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	vec3 A_C = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0*dot(r.direction(), A_C);
	float c = dot(A_C, A_C) - radius*radius;
	float discriminant = b*b - 4 * a*c;
	if (discriminant > 0){
		float temp = (-b - sqrt(discriminant)) / (2.0 * a);
		if (temp<t_max&&temp>t_min){
			rec.t = temp;
			rec.point = r.point_at_parameter(temp);
			rec.normal = (rec.point - center) / radius;
			rec.mat_ptr = mat_ptr;
			get_sphere_uv((rec.point - center) / radius, rec.u, rec.v);
			return true;
		}
		temp = (-b + sqrt(discriminant)) / (2.0 * a);
		if (temp > t_min && temp < t_max)
		{
			rec.t = temp;
			rec.point = r.point_at_parameter(temp);
			rec.normal = (rec.point - center) / radius;
			rec.mat_ptr = mat_ptr;
			get_sphere_uv((rec.point - center) / radius, rec.u, rec.v);
			return true;
		}
	}
	return false;
}

#endif