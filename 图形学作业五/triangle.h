#ifndef TRIANGLEH
#define TRIANGLEH

#include"hitable.h"
#include"material.h"

float min3(float a, float b, float c){
	float temp;
	if (a < b)
		temp = a;
	else
		temp = b;
	if (c < temp)
		temp = c;
	return temp;
}
float max3(float a, float b, float c){
	float temp;
	if (a > b)
		temp = a;
	else
		temp = b;
	if (c > temp)
		temp = c;
	return temp;
}
class triangle :public hitable{//三角形
public:
	triangle(){}
	triangle(vec3 p0, vec3 p1, vec3 p2, vec3 n0,vec3 n1, vec3 n2, material* mat,
		float u0 = 0, float v0 = 0, float u1 = 1, float v1 = 0.5,
		float u2 = 0, float v2 = 1) :point0(p0), point1(p1),
		point2(p2), normal0(n0), normal1(n1), normal2(n2),
		mat_ptr(mat),u0(u0),v0(v0),u1(u1),v1(v1),u2(u2),v2(v2){
		normal = unit_vector(cross(p1 - p0, p2 - p0));
		d =  -dot(normal, point0); 
	}
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
	//三个点的坐标
	vec3 point0;
	vec3 point1;
	vec3 point2;
	//三个点的法向量
	vec3 normal0;
	vec3 normal1;
	vec3 normal2;
	//三个点的uv值
	float u0, u1, u2, v0, v1, v2;
	vec3 normal;
	float d;
	material *mat_ptr;
};
bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const{
	if (dot(normal, r.direction()) == 0.0f)//光线与平面平行
		return false;
	float t = -(dot(normal, r.origin()) + d) / dot(normal, r.direction());
	if (t < t_min || t > t_max)
		return false;
	rec.t = t;
	rec.point = r.point_at_parameter(t);
	rec.mat_ptr = mat_ptr;

	//判断点是否在三角形内
	vec3 R = rec.point - point0;
	vec3 Q1 = point1 - point0;
	vec3 Q2 = point2 - point0;

	float dot_Q1_Q1 = dot(Q1, Q1);
	float dot_Q2_Q2 = dot(Q2, Q2);
	float dot_Q1_Q2 = dot(Q1, Q2);
	float dot_R_Q1 = dot(R, Q1);
	float dot_R_Q2 = dot(R, Q2);
	float determinant = 1.0f / (dot_Q1_Q1*dot_Q2_Q2 - dot_Q1_Q2*dot_Q1_Q2);
	//获取当前点的信息，由三个顶点和omega，1omega2得到
	float omega1 = determinant*(dot_Q2_Q2*dot_R_Q1 - dot_Q1_Q2*dot_R_Q2);
	float omega2 = determinant*(-dot_Q1_Q2*dot_R_Q1 + dot_Q1_Q1*dot_R_Q2);
	if (omega1 + omega2 > 1.0f || omega1 < 0.0f || omega2 < 0.0f)
		return false;
	rec.normal = normal0* (1.0f - omega1 - omega2) + normal1 * omega1 + normal2 * omega2;
	rec.u = u0* (1.0f - omega1 - omega2) + u1 * omega1 + u2 * omega2;
	rec.v = v0* (1.0f - omega1 - omega2) + v1 * omega1 + v2 * omega2;
	if (dot(rec.normal, r.direction()) > 0.0f)
		rec.normal = -rec.normal;
	return true;
}
bool triangle::bounding_box(double t0, double t1, aabb& output_box) const
{
	output_box = aabb(
		vec3(min3(point0.x(), point1.x(), point2.x()),
		min3(point0.y(), point1.y(), point2.y()),
		min3(point0.z(), point1.z(), point2.z())),
		vec3(max3(point0.x(), point1.x(), point2.x()),
		max3(point0.y(), point1.y(), point2.y()),
		max3(point0.z(), point1.z(), point2.z())));
	return true;
}
#endif