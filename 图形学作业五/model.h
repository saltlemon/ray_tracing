#ifndef MODELH
#define MODELH

#include"triangle.h"
#include <fstream>
#include <sstream>
#include<iostream>
#include<string>
#include<vector>
#include<vector>
using namespace std;
struct uv{
	float u, v;
	uv(float u, float v) :u(u), v(v){}
};
class model :public hitable{
public:
	void loadModel(const std::string& filePath, vec3 &cen, float &size);
	model(){}
	model(vec3 cen, const std::string &path, material* mat, float Size = 1.0f){
		cout << Size;
		mat_ptr = mat;
		minxyz = vec3(1000000, 1000000, 1000000);
		maxxyz = vec3(-1000000, -1000000, -1000000);
		loadModel(path, cen, Size);
	}
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const{
		hit_record temp_rec;
		bool hit_anything = false;
		float closest_so_far = t_max;
		for (int i = 0; i < mod.size(); i++){
			if (mod.at(i).hit(r, t_min, closest_so_far, temp_rec)){
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}
		return hit_anything;
	}
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const{
		output_box = aabb(minxyz, maxxyz);
		return true;
	}
	vector<triangle> mod;
	material *mat_ptr;
	vec3 minxyz;
	vec3 maxxyz;
};
void model::loadModel(const string &filePath, vec3 &cen, float &size){
	ifstream in(filePath);
	if (in.fail())
	{
		std::cout << "Fail to load obj->" << filePath << endl;
	}
	string line;
	vector<vec3> points;
	vector<vec3> normals;
	vector<uv> uvs;
	while (!in.eof())
	{
		getline(in, line);
		char trash;
		float x, y, z, u, v;
		istringstream iss(line.c_str());
		if (!line.compare(0, 2, "v "))
		{
			iss >> trash;
			iss >> x >> y >> z;
			vec3 vertex(x, y, z);
			points.push_back(vertex);
			if (minxyz.x() > x)minxyz[0] = x;
			if (minxyz.y() > y)minxyz[1] = y;
			if (minxyz.z() > z)minxyz[2] = z;
			if (maxxyz.x() < x)maxxyz[0] = x;
			if (maxxyz.y() < y)maxxyz[1] = y;
			if (maxxyz.z() < z)maxxyz[2] = z;
		}
		else if (!line.compare(0, 3, "vn "))
		{
			iss >> trash >> trash;
			iss >> x >> y >> z;
			vec3 normal(x, y, z);
			normals.push_back(normal);
		}
		else if (!line.compare(0, 3, "vt "))
		{
			iss >> trash >> trash;
			iss >> u;
			iss >> v;
			uv texcoord(u, v);
			uvs.push_back(texcoord);
		}
		else if (!line.compare(0, 2, "f "))
		{
			iss >> trash;
			int num = count(line.begin(), line.end(), '/');
			int index_point0;
			int index_point1;
			int index_point2;
			int index_point3;

			int index_normal0;
			int index_normal1;
			int index_normal2;
			int index_normal3;

			int index_uv0, index_uv1, index_uv2, index_uv3;
			if (num == 6){
				iss >> index_point0 >> trash >> index_uv0 >> trash >> index_normal0 >>
					index_point1 >> trash >> index_uv1 >> trash >> index_normal1 >>
					index_point2 >> trash >> index_uv2 >> trash >> index_normal2;
				mod.push_back(triangle(points[index_point0 - 1] * size + cen, points[index_point1 - 1] * size + cen,
					points[index_point2 - 1] * size + cen,
					normals[index_normal0 - 1], normals[index_normal1 - 1], normals[index_normal2 - 1],
					mat_ptr, uvs[index_uv0 - 1].u, uvs[index_uv0 - 1].v, uvs[index_uv1 - 1].u,
					uvs[index_uv1 - 1].v, uvs[index_uv2 - 1].u, uvs[index_uv2 - 1].v));
			}
			else if (num == 8){
				iss >> index_point0 >> trash >> index_uv0 >> trash >> index_normal0 >>
					index_point1 >> trash >> index_uv1 >> trash >> index_normal1 >>
					index_point2 >> trash >> index_uv2 >> trash >> index_normal2 >>
					index_point3 >> trash >> index_uv3 >> trash >> index_normal3;
				mod.push_back(triangle(points[index_point0 - 1] * size + cen, points[index_point1 - 1] * size + cen, points[index_point2 - 1] * size + cen,
					normals[index_normal0 - 1], normals[index_normal1 - 1], normals[index_normal2 - 1],
					mat_ptr, uvs[index_uv0 - 1].u, uvs[index_uv0 - 1].v, uvs[index_uv1 - 1].u,
					uvs[index_uv1 - 1].v, uvs[index_uv2 - 1].u, uvs[index_uv2 - 1].v));
				mod.push_back(triangle(points[index_point3 - 1] * size + cen, points[index_point1 - 1] * size + cen, points[index_point2 - 1] * size + cen,
					normals[index_normal3 - 1], normals[index_normal1 - 1], normals[index_normal2 - 1],
					mat_ptr, uvs[index_uv3 - 1].u, uvs[index_uv3 - 1].v, uvs[index_uv1 - 1].u,
					uvs[index_uv1 - 1].v, uvs[index_uv2 - 1].u, uvs[index_uv2 - 1].v));
			}
		}
	}
	in.close();
}
#endif