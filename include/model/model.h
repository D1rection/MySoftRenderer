#pragma once
#include <vector>
#include <string>

#include "math/geometry.h"

class Model {
private:
    std::vector<vec3> verts = {};       // 顶点
    std::vector<int> facet_vert = {};   // 面

public:
    Model(const std::string filename);
    int nverts() const;                                    
    int nfaces() const;                                    
    vec3 vert(const int i) const;                          
    vec3 vert(const int iface, const int nthvert) const;
};