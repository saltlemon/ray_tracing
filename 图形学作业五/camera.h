#ifndef CAMERRAH
#define CAMERRAH
#define M_PI 3.1415926535
#include"ray.h"
#include"random_num.h"

vec3 random_in_unit_disk(){
	vec3 p;
	p = unit_vector(2.0 * vec3(rand_num(), rand_num(), 0) - vec3(1, 1, 0)) * rand_num();
	//do{
	//p = 2.0*vec3(rand_num(), rand_num(), 0) - vec3(1, 1, 0);
	//} while (p.length() >= 1.0);
	return p;
}

class camera{
public:
	camera(vec3 lookfrom,vec3 lookat,vec3 vup,float vfov,float aspect,float aperture,float focus_dist){
		lens_radius = aperture / 2;
		float theta = vfov * M_PI / 180;
		float half_height = tan(theta / 2)*focus_dist;
		float half_width = aspect *half_height;
		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
		lower_left_corner = origin - half_width*u - half_height*v - focus_dist*w;
		horizontal = 2 * half_width*u;
		vertical = 2 * half_height*v;
	}
	ray get_ray(float s, float t)
	{
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}
	vec3 origin;
	vec3 vertical;
	vec3 horizontal;
	vec3 lower_left_corner;
	vec3 u, v, w;
	float lens_radius;
};

#endif