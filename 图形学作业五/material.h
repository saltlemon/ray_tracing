#ifndef MATERIALH
#define MATERIALH

#include"hitable.h"
#include"random_num.h"
#include"texture.h"
#include<memory>
class material {
public:
	//�������ߣ�ײ����¼��˥���ʣ���ʱ�����Ϊ��ɫ������һ���Ĳ���������������������
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
	virtual vec3 emitted(const float &u,const float &v, const vec3 &p) const
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}
};

class diffuse_light :public material{//��Դ���ʣ�����ײ���ò��ʵĹ�����Ϊ��Դ��ɫ����Դ��ɫ�ķ��������1���磨4��4��4��
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
		vec3 target = rec.point + rec.normal + random_in_unit_sphere();//�µĲ���Ŀ��
		scattered = ray(rec.point, target - rec.point);// �µĲ������ߣ������ػ��������
		attenuation = albedo->value(rec.u,rec.v,rec.point); //˥�� = ������
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
		scattered = ray(rec.point, reflected+fuzz*random_in_unit_sphere());//�������+����ĵ�λ�Ŷ�����ģ��ǹ⻬�ľ��淴��
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);//�������뷨�����ļн�������90��
	}
	vec3 albedo;
	float fuzz;
};

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	vec3 uv = unit_vector(v);
	float dt = dot(uv, unit_vector(n));//-cos(theta1)
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);//cos(theta2)*cos(theta2);
	if (discriminant > 0) {//�����Ϊ0�򲻴��ڣ��������Ϊ��ֱ�������ֵ���󣬲ſ��ܷ���false
		refracted = ni_over_nt * (uv - unit_vector(n) * dt) - unit_vector(n) * sqrt(discriminant);
		//�ֱ�Ϊ���������ĺ������������������һ���������
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
class dielectric : public material{//�������ʣ�����Ϊ��������ߣ����������
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
		if (rand_num() < reflect_prob){//ͨ��������β�������֤���������䷴��ռ��
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