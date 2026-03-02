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
constexpr vec3    eye{-1,0,2}; // camera position
constexpr vec3 center{0,0,0};  // camera direction
constexpr vec3     up{0,1,0};  // camera up vector

mat<4, 4> ModelView, Viewport, Perspective;

/**
 * @brief 视口矩阵
 * 将 -1，1 坐标映射到视口
 * 
 * @param x 图形左上角x
 * @param y 图形左上角y
 * @param w 
 * @param h 
 */
void viewport(const int x, const int y, const int w, const int h) {
    Viewport = {{
        { w/2., 0, 0, x + w/2. },
        { 0, h/2., 0, y + h/2. },
        { 0, 0, 1, 0 },
        { 0, 0, 0 ,1 }
    }};
}

/**
 * @brief 透视矩阵
 * 
 * @param f 焦距，镜头距离
 */
void perspective(const double f) {
    Perspective = {{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, -1/f, 1 }
    }};
}

/**
 * @brief 相机矩阵
 * 
 * @param eye 相机位置
 * @param center 相机的瞄准方向
 * @param up 相机的向上方向
 */
void lookat(const vec3 eye, const vec3 center, const vec3 up) {
    vec3 n = normalized(eye - center);
    vec3 l = normalized(cross(up, n));
    vec3 m = normalized(cross(n, l));

    ModelView = mat<4, 4>{{
        { l.x, l.y, l.z, 0 },
        { m.x, m.y, m.z, 0 },
        { n.x, n.y, n.z, 0 },
        { 0, 0, 0, 1 }
    }} * mat<4,4>{{
        { 1, 0, 0, -center.x },
        { 0, 1, 0, -center.y },
        { 0, 0, 1, -center.z },
        { 0, 0, 0, 1 }
    }};
}

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
 * @brief 光栅化三角形
 * 
 * @param clip 一个面的三个顶点，
 * @param zbuffer 
 * @param framebuffer 
 * @param color 
 */
void rasterize(const vec4 clip[3], std::vector<double> &zbuffer, TGAImage &framebuffer, TGAColor color) {
    vec4 ndc[3] = { clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w };    // 标准设备坐标（处理透视）
    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() };  // 视口转换

    // 三角形矩阵
    mat<3, 3> ABC = {{
        {screen[0].x, screen[0].y, 1.},
        {screen[1].x, screen[1].y, 1.},
        {screen[2].x, screen[2].y, 1.},
    }};

    // 背面剔除 + 不足像素剔除
    if(ABC.det() < 1) return;

    auto [bbminx, bbmaxx] = std::minmax({screen[0].x, screen[1].x, screen[2].x});
    auto [bbminy, bbmaxy] = std::minmax({screen[0].y, screen[1].y, screen[2].y});

    for(int x = std::max<int>(bbminx, 0); x < std::min<int>(bbmaxx, framebuffer.width() - 1); x ++) {
        for(int y = std::max<int>(bbminy, 0); y < std::min<int>(bbmaxy, framebuffer.height() - 1); y ++) {
            // 求重心坐标
            vec3 bc = ABC.invert_transpose() * vec3{ static_cast<double>(x), static_cast<double>(y), 1.};

            if(bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

            double z = bc * vec3{ndc[0].z, ndc[1].z, ndc[2].z};
            if(z <= zbuffer[x + y * framebuffer.width()]) continue;
            zbuffer[x + y * framebuffer.width()] = z; 
            framebuffer.set(x, y, color);
        }
    }
}

int main() {
    // 矩阵处理
    lookat(eye, center, up);
    perspective(norm(eye - center));
    viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);

    TGAImage framebuffer(width, height, TGAImage::RGB);
    std::vector<double> zbuffer(width * height, -std::numeric_limits<double>::max());

    Model model("../../../resources/diabio3_pose/diablo3_pose.obj");

    for(int i = 0; i < model.nfaces(); i ++) {
        vec4 clip[3];
        for(int d : { 0, 1, 2 }) {
            vec3 v = model.vert(i, d);
            clip[d] = Perspective * ModelView * vec4{v.x, v.y, v.z, 1};
        }

        TGAColor rand_color;
        for(int i = 0; i < 3; i ++) rand_color[i] = std::rand() % 255;
        rasterize(clip, zbuffer, framebuffer, rand_color);
    }

    framebuffer.write_tga_file("diablo_camera.tga");

    return 0;
}