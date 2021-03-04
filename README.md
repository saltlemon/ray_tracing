# 光线追踪器完善



## 简介

对作业5的光线追踪器进行完善，实现了光源添加，纹理添加，三角形模型引入，bvh树实现，以及多线程加速



## 项目实现方法简述



### 添加纹理
``` c++
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


```
具体思路为在材料中添加一个指向纹理的指针，这样在计算材料表面颜色的时候，不是返回默认值，而是调用value函数，根据u，v值来获取相应的值。上述代码为图片纹理的value函数实现。

### 添加光源
``` c++
class diffuse_light :public material{//光源材质，将碰撞到该材质的光线设为光源颜色，光源颜色的分量可以大于1，如（4，4，4）
public:
        diffuse_light(std::shared_ptr<texture> a) : albedo(a) {};
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, 
ray& scattered) const {
               return false;
        }
        virtual vec3 emitted(const float &u,const float &v, const vec3 &p) const
        {
               return  albedo->value(u, v, p);
        }
        std::shared_ptr<texture> albedo;
};
```
这里，我们把光源当成一种特殊的材质，这种材质不反射光线，而是直接发射相应的颜色，强度的光线。

### 三角形渲染
``` c++
bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const{
        if (dot(normal, r.direction()) == 0.0f)//光线与平面平行
               return false;
        float t = -(dot(normal, r.origin()) + d) / dot(normal, r.direction());
        if (t < t_min || t > t_max)
               return false;
        rec.t = t;
        rec.point = r.point_at_parameter(t);
        rec.mat_ptr = mat_ptr;

        //判断点是否在三角形内
        vec3 R = rec.point - point0;
        vec3 Q1 = point1 - point0;
        vec3 Q2 = point2 - point0;

        float dot_Q1_Q1 = dot(Q1, Q1);
        float dot_Q2_Q2 = dot(Q2, Q2);
        float dot_Q1_Q2 = dot(Q1, Q2);
        float dot_R_Q1 = dot(R, Q1);
        float dot_R_Q2 = dot(R, Q2);
        float determinant = 1.0f / (dot_Q1_Q1*dot_Q2_Q2 - dot_Q1_Q2*dot_Q1_Q2);

        //获取当前点的信息，由三个顶点和omega，1omega2得到
        float omega1 = determinant*(dot_Q2_Q2*dot_R_Q1 - dot_Q1_Q2*dot_R_Q2);
        float omega2 = determinant*(-dot_Q1_Q2*dot_R_Q1 + dot_Q1_Q1*dot_R_Q2);
        if (omega1 + omega2 > 1.0f || omega1 < 0.0f || omega2 < 0.0f)
               return false;

        rec.normal = normal0* (1.0f - omega1 - omega2) + normal1 * omega1 + normal2 * 
omega2;
        rec.u = u0* (1.0f - omega1 - omega2) + u1 * omega1 + u2 * omega2;
        rec.v = v0* (1.0f - omega1 - omega2) + v1 * omega1 + v2 * omega2;
        if (dot(rec.normal, r.direction()) > 0.0f)
               rec.normal = -rec.normal;
        return true;
}
```
这里，我们把光源当成一种特殊的材质，这种材质不反射光线，而是直接发射相应的颜色，强度的光线。

### 加载网格模型
``` c++
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

        vector<triangle> mod;
```
一个模型拥有一个vector用来储存三角形，具体的处理思路在没有处理优化的时候其实和hitable_list差不多。在obj文件读取时，会碰到四边形面片处理，如（p0,p1,p2,p3).这里我们就要把它当成2个三角形（p0,p1,p2)和(p0,p3,p2)就能刚好组成一个四边形面片，即两个三角形的公共边需要是四边形中的对角线。

### 构建BVH树
``` c++
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
```

首先利用层次包围盒的概念，利用bvh树对求交进行简化，可以理解为B（左节点），C（右节点）两个小的包围盒组合成一个大的包围盒A（父节点），在判断求交的时候，先判断是否和A（父节点）相交，若相交，再判断子节点。
那么主要的难点就是如何构建bvh树，这里利用了递归的思想，每次将数组二分，直到只剩两个物体分别为左右节点，同时，在每次二分前，随机选择一个轴对物体进行排序。

### 多线程加速
``` c++
#pragma omp parallel for
```
通过在for循环前添加上述语句，然后在项目属性的语言，C++中，openMP支持改为是~，就可以自动将下面的for循环分成N份，(N为电脑CPU核数)，然后把每份指派给一个核去执行，而且多核之间为并行执行


