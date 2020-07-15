#ifndef BVHNODEH
#define BVHNODEH

#include"hitable.h"
#include"hitable_list.h"
#include<memory>
#include<vector>
#include<random>
#include<algorithm>
using namespace std;
inline bool box_compare(const hitable* a, const hitable* b, int axis) {
	aabb box_a;
	aabb box_b;

	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min()[axis] < box_b.min()[axis];
}


bool box_x_compare(const hitable* a, const hitable* b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const hitable* a, const hitable* b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const hitable* a, const hitable* b) {
	return box_compare(a, b, 2);
}
class bvh_node :public hitable{
public:
	bvh_node(){};
	bvh_node(hitable_list& list, double t0, double t1)
		:bvh_node(list.list, 0, list.list_size, t0, t1){};
	bvh_node(hitable** objects, int start, int end, double t0, double t1);
	
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const ;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const ;
public:
	hitable* left;
	hitable* right;
	aabb box;
};
bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const{
	if (!box.hit(r, t_min, t_max))
		return false;
	bool hit_left = left->hit(r, t_min, t_max, rec);
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
	return hit_left || hit_right;
}
bool bvh_node::bounding_box(double t0, double t1, aabb& output_box) const {
	output_box = box;
	return true;
}
bvh_node::bvh_node(hitable** objects, int start, int end, double t0, double t1){

	static std::default_random_engine e;
	e.seed(std::random_device()());
	static std::uniform_int_distribution<int> u(0, 2);
	int index = u(e);
	auto comparator = (index == 0) ? box_x_compare
		: (index == 1) ? box_y_compare
		: box_z_compare;
	size_t length = end - start;
	if (length == 1){
		left = right = objects[start];
		
	}
	else if (length == 2){
		if (comparator(objects[start], objects[start+1])){
			left = objects[start];
			right = objects[start + 1];
		}
		else{
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else{
		std::sort(objects + start, objects + end, comparator);
		auto mid = start + length / 2;

		left = new bvh_node(objects, start, mid, t0, t1);
		right = new bvh_node(objects, mid, end, t0, t1);
	}
	aabb box_left, box_right;

	if (!left->bounding_box(t0, t1, box_left)
		|| !right->bounding_box(t0, t1, box_right)
		)
		std::cerr << "No bounding box in bvh_node constructor.\n";

	box = surrounding_box(box_left, box_right);
}

#endif