#ifndef MATERIALH
#define MATERIALH

#include"hitable.h"
#include"random_num.h"
#include"texture.h"
#include<memory>
class material {
public:
	//击中射线，撞击记录，衰减率（暂时可理解为颜色），下一步的采样方向（折射或反射或其他）
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
	virtual vec3 emitted(const float &u,const float &v, const vec3 &p) const
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}
};

class diffuse_light :public material{//光源材质，将碰撞到该材质的光线设为光源颜色，光源颜色的分量会大于1，如（4，4，4）
public:
	diffuse_light(std::shared_ptr<texture> a) : albedo(a) {};
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		return false;
	}
	virtual vec3 emitted(const float &u,const float &v, const vec3 &p) const
	{
		return  albedo->value(u, v, p);
	}
	std::shared_ptr<texture> albedo;
};
class lambertian : public material{
public:
	lambertian(std::shared_ptr<texture> a) : albedo(a) {};
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const{
		vec3 target = rec.point + rec.normal + random_in_unit_sphere();//新的采样目标
		scattered = ray(rec.point, target - rec.point);// 新的采样射线，兰伯特会随机打线
		attenuation = albedo->value(rec.u,rec.v,rec.point); //衰减 = 折射率
		return true;
	}
	std::shared_ptr<texture> albedo;
};

class metal : public material {
public:
	metal(const vec3& a,float f) : albedo(a) {
		if (f < 1)
			fuzz = f;
		else
			fuzz = 1;
	}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.point, reflected+fuzz*random_in_unit_sphere());//反射光线+随机的单位扰动用来模拟非光滑的镜面反射
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);//出射线与法向量的夹角在正负90间
	}
	vec3 albedo;
	float fuzz;
};

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	vec3 uv = unit_vector(v);
	float dt = dot(uv, unit_vector(n));//-cos(theta1)
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);//cos(theta2)*cos(theta2);
	if (discriminant > 0) {//出射角为0或不存在，既入射角为垂直射入或数值错误，才可能返回false
		refracted = ni_over_nt * (uv - unit_vector(n) * dt) - unit_vector(n) * sqrt(discriminant);
		//分别为出射向量的横向和纵向向量，加在一起组成向量
		return true;
	}
	else {
		return false;
	}
}
float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}
class dielectric : public material{//玻璃材质，可认为即反射光线，又折射光线
public:
	dielectric(float ri) :ref_idx(ri){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const{
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflect_prob;
		float cosine;
		if (dot(r_in.direction(), rec.normal) > 0){
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx*dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else{
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)){
			reflect_prob = schlick(cosine, ref_idx);
		}
		else{
			scattered = ray(rec.point, reflected);
			reflect_prob = 1.0;
		}
		if (rand_num() < reflect_prob){//通过大数多次采样来保证玻璃的折射反射占比
			scattered = ray(rec.point, reflected);
		}
		else{
			scattered = ray(rec.point, refracted);
		}
		return true;
	}
	float ref_idx;
};
#endif