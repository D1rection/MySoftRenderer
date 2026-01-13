#include <cmath>
#include "tga/tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

/**
 * @brief DDA法
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
    float y = ay;
    for(int x = ax; x <= bx; x ++) {
        isSteep ? framebuffer.set(y, x, color) : framebuffer.set(x, y, color);
        y += (by - ay) / static_cast<float>(bx - ax);   // 每次x进，y就增斜率量
    }
}

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    line(ax, ay, bx, by, framebuffer, blue);
    line(cx, cy, bx, by, framebuffer, green);
    line(cx, cy, ax, ay, framebuffer, yellow);
    line(ax, ay, cx, cy, framebuffer, red);

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("daa.tga");
    return 0;
}
