#ifndef MODELH
#define MODELH

#include"triangle.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<vector>
struct uv{
	float u, v;
	uv(float u, float v):u(u),v(v){}
};
class model :public hitable{
public:
	bool loadModel(const std::string& filePath){
		Assimp::Importer importer;
		// 加载模型 得到aiScene
		const aiScene* sceneObjPtr = importer.ReadFile(filePath,
			aiProcess_Triangulate | aiProcess_FlipUVs);
		// 递归处理结点
		return this->processNode(sceneObjPtr->mRootNode, sceneObjPtr);
	}
	bool processNode(const aiNode* node, const aiScene* sceneObjPtr){
		for (size_t i = 0; i < node->mNumMeshes; ++i) // 先处理自身结点
		{
			// 注意node中的mesh是对sceneObject中mesh的索引
			const aiMesh* meshPtr = sceneObjPtr->mMeshes[node->mMeshes[i]];
			this->processMesh(meshPtr, sceneObjPtr); // 处理Mesh
		}

		for (size_t i = 0; i < node->mNumChildren; ++i) // 再处理孩子结点
		{
			this->processNode(node->mChildren[i], sceneObjPtr);
		}
		return true;
	}
	bool processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr){
		vector<vec3> point_vector;
		vector<uv> uv_vector;
		vector<vec3> normal_vector;
		// 从Mesh得到顶点数据、法向量、纹理数据
		for (size_t i = 0; i < meshPtr->mNumVertices; ++i)
		{
			if (meshPtr->HasPositions())
			{
				vec3 temp_point(meshPtr->mVertices[i].x, meshPtr->mVertices[i].y, meshPtr->mVertices[i].z);
				point_vector.push_back(temp_point);
			}
			// 获取纹理数据 目前只处理0号纹理
			if (meshPtr->HasTextureCoords(0))
			{
				uv temp_uv(meshPtr->mTextureCoords[0][i].x, meshPtr->mTextureCoords[0][i].y);
				uv_vector.push_back(temp_uv);
			}
			else
			{
				uv temp_uv(0.0f, 0.0f);
				uv_vector.push_back(temp_uv);
			}
			// 获取法向量数据
			if (meshPtr->HasNormals())
			{
				vec3 temp_normal(meshPtr->mNormals[i].x, meshPtr->mNormals[i].y, meshPtr->mNormals[i].z);
				normal_vector.push_back(temp_normal);
			}
		}
		// 获取索引数据
		for (size_t i = 0; i < meshPtr->mNumFaces; ++i)
		{
			aiFace face = meshPtr->mFaces[i];
			if (face.mNumIndices != 3)
			{
				std::cerr << "Error:Model::processMesh, mesh not transformed to triangle mesh." << std::endl;
				return false;
			}
			int index0 = face.mIndices[0];
			int index1 = face.mIndices[1];
			int index2 = face.mIndices[2];
			vec3 p0 = point_vector.at(index0);
			vec3 n0 = normal_vector.at(index0);
			float u0 = uv_vector.at(index0).u;
			float v0 = uv_vector.at(index0).v;

			vec3 p1 = point_vector.at(index1);
			vec3 n1 = normal_vector.at(index1);
			float u1 = uv_vector.at(index1).u;
			float v1 = uv_vector.at(index1).v;

			vec3 p2 = point_vector.at(index2);
			vec3 n2 = normal_vector.at(index2);
			float u2 = uv_vector.at(index2).u;
			float v2 = uv_vector.at(index2).v;
			/*
			triangle(vec3 p0, vec3 p1, vec3 p2, vec3 n0, vec3 n1, vec3 n2, material* mat,
			float u0 = 0, float v0 = 0, float u1 = 1, float v1 = 0.5,
			float u2 = 0, float v2 = 1)
			*/
			triangle temp_tri(size_to*p0 + center, size_to*p1 + center, size_to*p2 + center,
				n0, n1, n2, mat_ptr, u0, v0, u1, v1, u2, v2);
			mod.push_back(temp_tri);
		}
		return true;
	}

	model(){}
	model(vec3 cen,const std::string &path, material* mat,float Size_to = 0.0f){
		mat_ptr = mat;
		center = cen;
		size_to = Size_to;
		loadModel(path);
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
	vec3 center;
	float size_to;
	vector<triangle> mod;
	material *mat_ptr;
};
#endif