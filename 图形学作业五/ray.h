#ifndef RAYH
#define RAYH
#include"vec3.h"
class ray
{
public:
	vec3 A;
	vec3 B;

	ray(){};
	ray(const vec3& a, const vec3& b)
	{
		A = a; B = b;
	}
	vec3 origin() const{ return A; }//光线原点
	vec3 direction() const{ return B; }//光线方向
	vec3 point_at_parameter(float t) const{ return A + t*B; }//光线碰撞点

};

#endif

