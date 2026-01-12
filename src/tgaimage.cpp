#include <iostream>
#include <cstring>
#include "tga/tgaimage.h"

/**
 * @brief 初始化图像并填充背景色.
 * 
 * 分配内存并逐像素填充指定的颜色。
 * 
 * @param w   图像宽度 (Pixels).
 * @param h   图像高度 (Pixels).
 * @param bpp 每个像素的字节数 (Bytes Per Pixel). 
 *            通常 1=灰度, 3=RGB, 4=RGBA.
 * @param c   背景填充颜色 (默认为黑色/透明).
 */
TGAImage::TGAImage(const int w, const int h, const int bpp, TGAColor c) : w(w), h(h), bpp(bpp), data(w*h*bpp) {
    for(int j = 0; j < h; j ++)
        for(int i = 0; i < w; i ++)
            set(i, j ,c);
}


/**
 * @brief 从磁盘读取一张 TGA 图片并加载到当前对象中.
 *
 * 支持两类常见的 TGA 数据类型:
 * 
 * - 未压缩: datatypecode = 2 (true-color) / 3 (grayscale)
 * - RLE 压缩: datatypecode = 10 (true-color) / 11 (grayscale)
 * 
 * @param filename TGA 文件路径.
 * @return 读取情况
 */
bool TGAImage::read_tga_file(const std::string filename) {
    std::ifstream in;
    in.open(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        return false;
    }
    /* 读取TGA头 */
    TGAHeader header;
    in.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (!in.good()) {
        std::cerr << "an error occured while reading the header\n";
        return false;
    }
    w = header.width;
    h = header.height;
    bpp = header.bitsperpixel >> 3;
    if (w<=0 || h<=0 || (bpp!=GRAYSCALE && bpp!=RGB && bpp!=RGBA)) {
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }
    size_t nbytes = w*h*bpp;
    data = std::vector<std::uint8_t>(nbytes, 0);
    
    if (3==header.datatypecode || 2==header.datatypecode) {
        /* 未压缩的RGB/RGBA/灰度图 */
        in.read(reinterpret_cast<char *>(data.data()), nbytes);
        if (!in.good()) {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    } else if (10==header.datatypecode||11==header.datatypecode) {
        /* RLE压缩 */
        if (!load_rle_data(in)) {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    } else {
        std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
        return false;
    }

    /* 调整为原点在左上角TL，水平上LTR */
    if (!(header.imagedescriptor & 0x20))
        flip_vertically();
    if (header.imagedescriptor & 0x10)
        flip_horizontally();
    std::cerr << w << "x" << h << "/" << bpp*8 << "\n";
    return true;
}

/**
 * @brief 解码 TGA 的 RLE 像素数据到 `data`.
 * @param in 已打开的二进制输入流（位于像素数据起始处）。
 * @return 成功返回 true；失败返回 false。
 */
bool TGAImage::load_rle_data(std::ifstream &in) {
    size_t pixelcount = w*h;
    size_t currentpixel = 0;
    size_t currentbyte  = 0;
    TGAColor colorbuffer;
    do {
        std::uint8_t chunkheader = 0;
        chunkheader = in.get();
        if (!in.good()) {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
        if (chunkheader<128) {
            chunkheader++;
            for (int i=0; i<chunkheader; i++) {
                in.read(reinterpret_cast<char *>(colorbuffer.bgra), bpp);
                if (!in.good()) {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }
                for (int t=0; t<bpp; t++)
                    data[currentbyte++] = colorbuffer.bgra[t];
                currentpixel++;
                if (currentpixel>pixelcount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        } else {
            chunkheader -= 127;
            in.read(reinterpret_cast<char *>(colorbuffer.bgra), bpp);
            if (!in.good()) {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }
            for (int i=0; i<chunkheader; i++) {
                for (int t=0; t<bpp; t++)
                    data[currentbyte++] = colorbuffer.bgra[t];
                currentpixel++;
                if (currentpixel>pixelcount) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    } while (currentpixel < pixelcount);
    return true;
}

/**
 * @brief 将当前图像保存为 TGA 文件.
 * 
 * 该函数根据当前图像的属性构建 TGA 头文件，并支持可选的 RLE 压缩。
 * 它自动写入 TGA 2.0 标准要求的页脚 (Footer) 和签名，
 * 确保生成的文件能被现代图像软件正确识别。
 * 
 * @param filename 输出文件的完整路径或文件名 
 * @param vflip    垂直翻转控制标志 (Vertical Flip).
 *                 - false: 图像原点设为 左上角 (Top-Left, 0x20)
 *                 - true:  图像原点设为 左下角 (Bottom-Left, 0x00).
 * @param rle      是否启用 RLE (Run-Length Encoding) 压缩.
 *                 - true:  输出 Type 10 (RGB) 或 Type 11 (灰度) 的压缩格式. 文件较小，读取较慢.
 *                 - false: 输出 Type 2 (RGB) 或 Type 3 (灰度) 的未压缩格式. 文件较大，读取极快.
 * 
 * @return true  文件写入成功.
 * @return false 文件打开失败、写入过程中出错或磁盘已满.
 * 
 * @note 此函数会覆盖同名文件，且不进行确认。
 */
bool TGAImage::write_tga_file(const std::string filename, const bool vflip, const bool rle) const {
    constexpr std::uint8_t developer_area_ref[4] = {0, 0, 0, 0};
    constexpr std::uint8_t extension_area_ref[4] = {0, 0, 0, 0};
    constexpr std::uint8_t footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
    std::ofstream out;
    out.open(filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        return false;
    }
    TGAHeader header = {};
    header.bitsperpixel = bpp << 3;
    header.width  = w;
    header.height = h;
    header.datatypecode = (bpp==GRAYSCALE ? (rle?11:3) : (rle?10:2));
    header.imagedescriptor = vflip ? 0x00 : 0x20; // top-left or bottom-left origin
    out.write(reinterpret_cast<const char *>(&header), sizeof(header));
    if (!out.good()) goto err;
    if (!rle) {
        out.write(reinterpret_cast<const char *>(data.data()), w*h*bpp);
        if (!out.good()) goto err;
    } else if (!unload_rle_data(out)) goto err;
    out.write(reinterpret_cast<const char *>(developer_area_ref), sizeof(developer_area_ref));
    if (!out.good()) goto err;
    out.write(reinterpret_cast<const char *>(extension_area_ref), sizeof(extension_area_ref));
    if (!out.good()) goto err;
    out.write(reinterpret_cast<const char *>(footer), sizeof(footer));
    if (!out.good()) goto err;
    return true;

err:
    std::cerr << "can't dump the tga file\n";
    return false;
}

/**
 * @brief 将像素数据按 TGA RLE 规则压缩并写入输出流.
 * @param out 已打开的二进制输出流（位于像素数据起始处）。
 * @return 成功返回 true；失败返回 false。
 */
bool TGAImage::unload_rle_data(std::ofstream &out) const {
    const std::uint8_t max_chunk_length = 128;
    size_t npixels = w*h;
    size_t curpix = 0;
    while (curpix<npixels) {
        size_t chunkstart = curpix*bpp;
        size_t curbyte = curpix*bpp;
        std::uint8_t run_length = 1;
        bool raw = true;
        while (curpix+run_length<npixels && run_length<max_chunk_length) {
            bool succ_eq = true;
            for (int t=0; succ_eq && t<bpp; t++)
                succ_eq = (data[curbyte+t]==data[curbyte+t+bpp]);
            curbyte += bpp;
            if (1==run_length)
                raw = !succ_eq;
            if (raw && succ_eq) {
                run_length--;
                break;
            }
            if (!raw && !succ_eq)
                break;
            run_length++;
        }
        curpix += run_length;
        out.put(raw ? run_length-1 : run_length+127);
        if (!out.good()) return false;
        out.write(reinterpret_cast<const char *>(data.data()+chunkstart), (raw?run_length*bpp:bpp));
        if (!out.good()) return false;
    }
    return true;
}

/**
 * @brief 获取指定坐标处的像素颜色.
 * @param x 像素 x 坐标（0-based）。
 * @param y 像素 y 坐标（0-based）。
 * @return 返回该像素颜色；若越界或图像为空则返回默认颜色（全 0）。
 */
TGAColor TGAImage::get(const int x, const int y) const {
    if (!data.size() || x<0 || y<0 || x>=w || y>=h) return {};
    TGAColor ret = {0, 0, 0, 0, bpp};
    const std::uint8_t *p = data.data()+(x+y*w)*bpp;
    for (int i=bpp; i--; ret.bgra[i] = p[i]);
    return ret;
}

/**
 * @brief 设置指定坐标处的像素颜色.
 * @param x 像素 x 坐标（0-based）。
 * @param y 像素 y 坐标（0-based）。
 * @param c 要写入的颜色。
 * @note 越界或图像为空时不做任何操作。
 */
void TGAImage::set(int x, int y, const TGAColor &c) {
    if (!data.size() || x<0 || y<0 || x>=w || y>=h) return;
    memcpy(data.data()+(x+y*w)*bpp, c.bgra, bpp);
}

/**
 * @brief 水平翻转图像（左右镜像）。
 */
void TGAImage::flip_horizontally() {
    for (int i=0; i<w/2; i++)
        for (int j=0; j<h; j++)
            for (int b=0; b<bpp; b++)
                std::swap(data[(i+j*w)*bpp+b], data[(w-1-i+j*w)*bpp+b]);
}

/**
 * @brief 垂直翻转图像（上下镜像）。
 */
void TGAImage::flip_vertically() {
    for (int i=0; i<w; i++)
        for (int j=0; j<h/2; j++)
            for (int b=0; b<bpp; b++)
                std::swap(data[(i+j*w)*bpp+b], data[(i+(h-1-j)*w)*bpp+b]);
}

/**
 * @brief 获取图像宽度（像素）。
 */
int TGAImage::width() const {
    return w;
}

/**
 * @brief 获取图像高度（像素）。
 */
int TGAImage::height() const {
    return h;
}
