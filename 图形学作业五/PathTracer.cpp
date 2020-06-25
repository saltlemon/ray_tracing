#include "PathTracer.h"
#include "ray.h"
#include"sphere.h"
#include"triangle.h"
#include"hitable_list.h"
#include"camera.h"
#include"random_num.h"
#include"material.h"
#include <time.h>
#include <iostream>
using namespace std;
vec3 what_color(const ray& r, hitable *world, int depth){
	hit_record rec;
	/* if (depth <= 0)
		return vec3(0, 0, 0);
	if (world->hit(r, 0.001, FLT_MAX, rec)){
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.point);
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			return emitted + attenuation * what_color(scattered, world, depth - 1);
		}
		else{
			return emitted;
		}
	}
	else{
		//vec3 unit_dir = unit_vector(r.direction());
		//float temp = 0.5*(unit_dir.y() + 1.0);
		//return(1.0 - temp)*vec3(1.0, 1.0, 1.0) + temp*vec3(0.5, 0.7, 1.0);
		return vec3(0, 0, 0);
	}
	*/
	if (world->hit(r, 0.001, FLT_MAX, rec)){
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.point);

		if (depth >= 50) return vec3(0.1, 0.1, 0.1);

		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			return truncation(emitted + attenuation * what_color(scattered, world, depth + 1));
		}
		else{
			return emitted;
		}
	}
	else{
		vec3 unit_dir = unit_vector(r.direction());
		float temp = 0.5*(unit_dir.y() + 1.0);
		return(1.0 - temp)*vec3(1.0, 1.0, 1.0) + temp*vec3(0.5, 0.7, 1.0);
		//return vec3(0.1, 0.1, 0.1);
	}
}

hitable *random_scene(){
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(
		make_shared<checker_texture>(
		make_shared<solid_color>(0.2, 0.3, 0.1),
		make_shared<solid_color>(0.9, 0.9, 0.9)
		)));
	int i = 1;
	for (int a = -6; a < 6; a++){
		for (int b = -6; b < 6; b++){
			float choose_mat = rand_num();
			vec3 center(a + 0.9*rand_num(), 0.2, b + 0.9*rand_num());
			if ((center - vec3(-4, 0.2, 0)).length()>0.9){
				if (choose_mat < 0.8){
					list[i++] = new sphere(center, 0.2, new lambertian(
						std::make_shared<solid_color>(rand_num()*rand_num(), rand_num()*rand_num(), rand_num()*rand_num())));
				}
				else if (choose_mat < 0.95){
					list[i++] = new sphere(center, 0.2, new metal(
						vec3(0.5*(1 + rand_num()), 0.5*(1 + rand_num()), 0.5*(1 + rand_num())), 0.5*rand_num()));
				}
				else{
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}
	
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new diffuse_light(std::make_shared<solid_color>(4, 4, 4)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

PathTracer::PathTracer()
	: m_channel(4), m_width(800), m_height(600), m_image(nullptr) {}

PathTracer::~PathTracer()
{
	if (m_image != nullptr)
		m_image;
	m_image = nullptr;
}

void PathTracer::initialize(int width, int height)
{
	m_width = width;
	m_height = height;
	if (m_image != nullptr)
		delete m_image;

	// allocate pixel buffer, RGBA format.
	m_image = new unsigned char[width * height * m_channel];
}

unsigned char * PathTracer::render(double & timeConsuming)
{
	if (m_image == nullptr)
	{
		std::cout << "Must call initialize() before rendering.\n";
		return nullptr;
	}

	// record start time.
	double startFrame = clock();
	vec3 lookfrom(5,1,3);
	vec3 lookat(0,0.6,0);
	float dist_to_focus = 5;
	float aperture = 0.0;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), 45, m_width / m_height,aperture,dist_to_focus);
	
	//hitable *world = random_scene();
	hitable *list[2];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(
		make_shared<checker_texture>(
		make_shared<solid_color>(0.2, 0.3, 0.1),
		make_shared<solid_color>(0.9, 0.9, 0.9)
		)));
	list[1] = new triangle(vec3(1, 1, 1), vec3(0, 1, 1), vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 0, 1), vec3(0, 0, 1), new lambertian(std::make_shared<img_texture>("earth.jpg")));
	hitable *world = new hitable_list(list, 2);
	// render the image pixel by pixel.
	for (int row = m_height - 1; row >= 0; --row)
	{
		for (int col = 0; col < m_width; ++col)
		{
			// TODO: implement your ray tracing algorithm by yourself.
			vec3 color(0,0,0);
			for (int i = 0; i < 100; i++){
				float u = (static_cast<float>(col)+rand_num()) / static_cast<float>(m_width);
				float v = (static_cast<float>(row)+rand_num()) / static_cast<float>(m_height);
				ray r = cam.get_ray(u, v);
				color += what_color(r, world ,0);
			}
			color /= 100;
			color = vec3(sqrt(color[0]), sqrt(color[1]), sqrt(color[2]));
			drawPixel(col, row, color);
		}
		std::cout << row<<std::endl;
	}

	// record end time.
	double endFrame = clock();

	// calculate time consuming.
	timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;

	return m_image;
}

void PathTracer::drawPixel(unsigned int x, unsigned int y, const vec3 & color)
{
	// Check out 
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return;
	// x is column's index, y is row's index.
	unsigned int index = (y * m_width + x) * m_channel;
	// store the pixel.
	// red component.
	m_image[index + 0] = static_cast<unsigned char>(255 * color.x());
	// green component.
	m_image[index + 1] = static_cast<unsigned char>(255 * color.y());
	// blue component.
	m_image[index + 2] = static_cast<unsigned char>(255 * color.z());
	// alpha component.
	m_image[index + 3] = static_cast<unsigned char>(255);
}
