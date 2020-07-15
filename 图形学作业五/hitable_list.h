#ifndef HITABLELISTH
#define HITABLELISTH
#include"hitable.h"
#include <float.h>
#include<vector>
using namespace std;
class hitable_list :public hitable{
public:
	hitable_list(){}
	hitable_list(hitable** l, int n){ list = l; list_size = n; };
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const ;
	

	hitable** list;
	int list_size;
};
bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;
	for (int i = 0; i < list_size; i++){
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)){
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}
bool hitable_list::bounding_box(double t0, double t1, aabb& output_box) const{
	if (list_size == 0)return false;
	aabb temp_box;
	bool first_box = true;
	for (int i = 0; i < list_size; i++){
		if (list[i]->bounding_box(t0,t1,temp_box)){
			output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
			first_box = false;
		}
	}

	return true;
}
#endif