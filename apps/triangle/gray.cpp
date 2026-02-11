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
void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer) {
    /* 左下角坐标 */
    int bbminx = std::min(ax, std::min(bx, cx));
    int bbminy = std::min(ay, std::min(by, cy));
    /* 右上角坐标 */
    int bbmaxx = std::max(ax, std::max(bx, cx));
    int bbmaxy = std::max(ay, std::max(by, cy));
    double tot_area = get_signed_area(ax, ay, bx, by, cx, cy);

    for(int x = bbminx; x <= bbmaxx; x ++ ) {
        for(int y = bbminy; y <= bbmaxy; y ++) {
            double alpha = get_signed_area(x, y, bx, by, cx, cy) / tot_area;
            double beta = get_signed_area(x, y, cx, cy, ax, ay) / tot_area;
            double gamma = get_signed_area(x, y, ax, ay, bx, by) / tot_area;

            if(alpha < 0 || beta < 0 || gamma < 0) continue;
            unsigned char z = static_cast<unsigned char>(alpha * 23 + beta * 59 + gamma * 255);
            framebuffer.set(x, y, {z});
        }
    }
}

int main() {
    TGAImage framebuffer(width, height, TGAImage::GRAYSCALE);
    triangle(  7, 45, 35, 100, 45,  60, framebuffer);
    triangle(120, 35, 90,   5, 45, 110, framebuffer);
    triangle(115, 83, 80,  90, 85, 120, framebuffer);
    framebuffer.write_tga_file("gray.tga");
    return 0;
}