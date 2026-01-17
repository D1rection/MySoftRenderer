#include <fstream>
#include <sstream>

#include "model/model.h"

/**
 * @brief Construct a new Model:: Model object
 * 
 * @param filename 
 */
Model::Model(const std::string filename) {
    std::ifstream in;
    in.open(filename, std::ios::in);
    if(in.fail()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    std::string line;
    while(!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if(!line.compare(0, 2, "v ")) {
            iss >> trash;
            vec3 v;
            for(int i = 0; i < 3; i ++) iss >> v[i];
            verts.push_back(v);
        }
        else if(!line.compare(0, 2, "f ")) {
            int f, t, n, cnt = 0;
            iss >> trash;
            while(iss >> f >> trash >> t >> trash >> n) {
                facet_vert.push_back(-- f);
                cnt ++;
            }
            if (3!=cnt) {
                std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                return;
            }
        }
    }
}

/**
 * @brief 顶点个数
 * 
 * @return int 
 */
int Model::nverts() const { return verts.size(); }

/**
 * @brief 面数
 * 
 * @return int 
 */
int Model::nfaces() const { return facet_vert.size() / 3; }

/**
 * @brief 获取顶点
 * 
 * @param i 索引
 * @return vec3 
 */
vec3 Model::vert(const int i) const {
    return verts[i];
}

/**
 * @brief 获取第 iface 个面的第 nthvert 个顶点
 * 
 * @param iface 
 * @param nthvert 
 * @return vec3 
 */
vec3 Model::vert(const int iface, const int nthvert) const {
    int idx = iface * 3 + nthvert;
    int global_idx = facet_vert[idx];
    return verts[global_idx];
}