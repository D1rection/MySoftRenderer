#include <cmath>

#include "tga/tgaimage.h"
#include "model/model.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

constexpr int width  = 800;
constexpr int height = 800;

/**
 * @brief 鞋带公式求三角形面积
 * @note 逆时针求解面积，一般为正
 * 
 * @param ax 
 * @param ay 
 * @param bx 
 * @param by 
 * @param cx 
 * @param cy 
 * @return double 三角形有向面积
 */
double get_signed_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

/**
 * @brief 现代栅格化算法
 * @note 便于并行计算
 * 
 * @param ax 
 * @param ay 
 * @param bx 
 * @param by 
 * @param cx 
 * @param cy 
 * @param framebuffer 
 * @param color 
 */
void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &zbuffer, TGAImage &framebuffer, TGAColor color) {
    /* 左下角坐标 */
    int bbminx = std::min(ax, std::min(bx, cx));
    int bbminy = std::min(ay, std::min(by, cy));
    /* 右上角坐标 */
    int bbmaxx = std::max(ax, std::max(bx, cx));
    int bbmaxy = std::max(ay, std::max(by, cy));
    double tot_area = get_signed_area(ax, ay, bx, by, cx, cy);

    /* 背面剔除的临时方法 */
    if(tot_area < 1) return;

    for(int x = bbminx; x <= bbmaxx; x ++ ) {
        for(int y = bbminy; y <= bbmaxy; y ++) {
            double alpha = get_signed_area(x, y, bx, by, cx, cy) / tot_area;
            double beta = get_signed_area(x, y, cx, cy, ax, ay) / tot_area;
            double gamma = get_signed_area(x, y, ax, ay, bx, by) / tot_area;

            if(alpha < 0 || beta < 0 || gamma < 0) continue;
            
            // 灰度图按深度插值
            unsigned char z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            if(z < zbuffer.get(x, y)[0]) continue;  // 处理隐藏面 - Per Pixel Painter Algo
            zbuffer.set(x, y, {z});

            framebuffer.set(x, y, color);
        }
    }
}

/**
 * @brief 视口转换
 * 
 * @param v 
 * @return vec2 
 */
vec3 fit(const vec3& v) {
    vec3 nv;
    nv.x = std::round((v.x + 1.) * width / 2.);
    nv.y = std::round((v.y + 1.) * height / 2.);
    nv.z = std::round((v.z + 1.) * 255. / 2.);   // 深度用于灰度图
    return nv;
}

int main() {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    Model model("../../../resources/diabio3_pose/diablo3_pose.obj");

    for(int i = 0; i < model.nfaces(); i ++) {
        vec3 a = fit(model.vert(i, 0));
        vec3 b = fit(model.vert(i, 1));
        vec3 c = fit(model.vert(i, 2));
        TGAColor rand_color;
        for(int c=0; c<3; c++) rand_color[c] = std::rand()%255;
        triangle(a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z, zbuffer, framebuffer, rand_color);
    }

    for(int i = 0; i < model.nverts(); i ++) {
        vec3 p = fit(model.vert(i));
        framebuffer.set(p.x, p.y, white);
    }

    framebuffer.write_tga_file("diablo_rgb_face.tga");
    zbuffer.write_tga_file("dioblo_gray_face.tga");

    return 0;
}