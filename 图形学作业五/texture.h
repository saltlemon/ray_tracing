#ifndef TEXTUREH	
#define TEXTUREH
#define M_PI 3.1415926535
#include"vec3.h"
#include<memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
class texture{
public:
	virtual vec3 value(double u, double v, const vec3& point) const = 0;
};

class solid_color :public texture{//单色纹理
public:
	solid_color(){}
	solid_color(vec3 c) :color_value(c){}
	solid_color(double r, double g, double b){
		color_value[0] = r;
		color_value[1] = g;
		color_value[2] = b;
	}
	virtual vec3 value(double u, double v, const vec3& point) const {
		return color_value;
	}
	vec3 color_value;
};

class checker_texture : public texture {//棋盘纹理
public:
	checker_texture() {}
	checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1) : even(t0), odd(t1) {}

	virtual vec3 value(double u, double v, const vec3& p) const {
		auto sines = sin(10 * p.x())*sin(10 * p.y())*sin(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}

public:
	shared_ptr<texture> odd;
	shared_ptr<texture> even;
};

class img_texture :public texture{//图片纹理
private:
	unsigned char *m_data;
	int m_width, m_height, m_channel;

public:
	img_texture(){}
	img_texture(const std::string &path){
		m_data = stbi_load(path.c_str(), &m_width, &m_height, &m_channel, 0);
		if (m_data == nullptr)
			std::cout << "Failed to load the image->" << path.c_str() << std::endl;
	};
	virtual vec3 value(double u, double v, const vec3& p) const {
		int i = static_cast<int>(u * m_width);
		int j = static_cast<int>((1.0f - v)*m_height) - 0.001;
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		if (i > m_width - 1) i = m_width - 1;
		if (j > m_height - 1) j = m_height - 1;
		int index = (j * m_width + i) * m_channel;
		float r = static_cast<int>(m_data[index + 0]) / 255.0f;
		float g = static_cast<int>(m_data[index + 1]) / 255.0f;
		float b = static_cast<int>(m_data[index + 2]) / 255.0f;
		return vec3(r, g, b);
	}
};
#endif