#include <cmath>
#include "tga/tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

constexpr int width = 200;
constexpr int height = 200;

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
 * @brief 扫描线算法
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
void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    if(ay > by) { std::swap(ax, bx); std::swap(ay, by); }
    if(ay > cy) { std::swap(ax, cx); std::swap(ay, cy); }
    if(by > cy) { std::swap(bx, cx); std::swap(by, cy); }
    int tot_h = cy - ay;

    // 下半部分
    if(ay != by) {
        int seg_h = by - ay;
        for(int y = ay; y <= by; y ++) {
            int ac_x = ax + (cx - ax) * (y - ay) / tot_h;
            int ab_x = ax + (bx - ax) * (y - ay) / seg_h;
            for(int x = std::min(ac_x, ab_x); x < std::max(ac_x, ab_x); x ++) {
                framebuffer.set(x, y, color);
            }
        }
    }

    // 上半部分
    if(by != cy) {
        int seg_h = cy - by;
        for(int y = by; y <= cy; y ++) {
            int ac_x = ax + (cx - ax) * (y - ay) / tot_h;
            int bc_x = bx + (cx - bx) * (y - by) / seg_h;
            for(int x = std::min(ac_x, bc_x); x < std::max(ac_x, bc_x); x ++) {
                framebuffer.set(x, y, color);
            }
        }
    }
}

int main() {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    framebuffer.write_tga_file("scanline.tga");
    return 0;
}