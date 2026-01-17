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
 * @brief Bresenham法
 * 
 * @param ax 
 * @param ay 
 * @param bx 
 * @param by 
 * @param framebuffer 
 * @param color 
 */
void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
    bool isSteep = std::abs(ax - bx) < std::abs(ay - by);
    if(isSteep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if(ax > bx) {   // ltr
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for(int x = ax; x <= bx; x ++) {
        isSteep ? framebuffer.set(y, x, color) : framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by - ay);
        if(ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx - ax);
        }
    }
}

/**
 * @brief 视口转换
 * 
 * @param v 
 * @return vec2 
 */
vec2 fit(const vec3& v) {
    vec2 nv;
    nv.x = std::round((v.x + 1.) * width / 2.);
    nv.y = std::round((v.y + 1.) * height / 2.);
    return nv;
}

int main() {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Model model("../../../resources/diabio3_pose/diablo3_pose.obj");

    for(int i = 0; i < model.nfaces(); i ++) {
        vec2 a = fit(model.vert(i, 0));
        vec2 b = fit(model.vert(i, 1));
        vec2 c = fit(model.vert(i, 2));
        line(a.x, a.y, b.x, b.y, framebuffer, yellow);
        line(b.x, b.y, c.x, c.y, framebuffer, yellow);
        line(c.x, c.y, a.x, a.y, framebuffer, yellow);
    }

    for(int i = 0; i < model.nverts(); i ++) {
        vec2 p = fit(model.vert(i));
        framebuffer.set(p.x, p.y, white);
    }

    framebuffer.write_tga_file("diablo.tga");

    return 0;
}