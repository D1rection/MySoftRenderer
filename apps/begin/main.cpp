#include "tga/tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;
    int dx = 0, dy = 0;
    int ex = 32, ey = 32;

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, red);
    framebuffer.set(cx, cy, yellow);
    framebuffer.set(dx, dy, green);
    framebuffer.set(ex, ey, blue);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
