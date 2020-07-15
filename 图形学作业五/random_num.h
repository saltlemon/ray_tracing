#ifndef RANDOMNUMH
#define RANDOMNUMH
#include"vec3.h"
#include<random>

float rand_num() {
	static std::uniform_real_distribution<float> u_rand(0, 1);
	static std::default_random_engine rand_engine;
	rand_engine.seed(std::random_device()());
	return u_rand(rand_engine);
}

vec3 random_in_unit_sphere(){
	vec3 p;
	p = unit_vector(2.0 * vec3(rand_num(), rand_num(), rand_num()) - vec3(1, 1, 1)) * rand_num();
	//do{
		//p = 2.0*vec3(rand_num(), rand_num(), rand_num()) - vec3(1, 1, 1);
	//} while (p.length() >= 1.0);
	return p;
}

#endif