#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

/**
 * @brief n维向量
 *
 * @tparam n 向量维度，需为正数（例如 2/3/4）
 */
template<int n> struct vec {
    double data[n] = {0};
    double& operator[](const int i) {
        assert(i >= 0 && i < n);
        return data[i];
    }
    double operator[](const int i) const {
        assert(i >= 0 && i < n);
        return data[i];
    }
};

/**
 * @brief 计算两个向量的点乘
 * 
 * @tparam n 
 * @param lhs 
 * @param rhs 
 * @return double 
 */
template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
    double ret = 0;
    for(int i = 0; i < n; i ++) ret += lhs[i] * rhs[i];
    return ret;
}

/**
 * @brief 计算两个向量的和
 * 
 * @tparam n 
 * @param lhs 
 * @param rhs 
 * @return vec<n> 
 */
template<int n> vec<n> operator+(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> ret = lhs;
    for(int i = 0; i < n; i ++) ret += rhs[i];
    return ret;
}

/**
 * @brief 计算两个向量的差
 * 
 * @tparam n 
 * @param lhs 
 * @param rhs 
 * @return vec<n> 
 */
template<int n> vec<n> operator-(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> ret = lhs;
    for(int i = 0; i < n; i ++) ret -= rhs[i];
    return ret;
}

/**
 * @brief 计算向量右数乘
 * 
 * @tparam n 
 * @param lhs 
 * @param rhs 
 * @return vec<n> 
 */
template<int n> vec<n> operator*(const vec<n>& lhs, const double& rhs) {
    vec<n> ret = lhs;
    for(int i = 0; i < n; i ++) ret[i] *= rhs;
    return ret;
}

/**
 * @brief 计算向量左数乘
 * 
 * @tparam n 
 * @param lhs 
 * @param rhs 
 * @return vec<n> 
 */
template<int n> vec<n> operator*(const double& lhs, const vec<n>& rhs) {
    return rhs * lhs;   // 直接复用右数乘
}

/**
 * @brief 计算向量除以数
 * 
 * @tparam n 
 * @param lhs 
 * @param rhs 
 * @return vec<n> 
 */
template<int n> vec<n> operator/(const vec<n>& lhs, const double& rhs) {
    vec<n> ret = lhs;
    for(int i = 0; i < n; i ++) ret[i] /= rhs;
    return ret;
}

/**
 * @brief 将向量按分量输出到流（以空格分隔）
 *
 * @tparam n
 * @param out
 * @param v
 * @return
 */
template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i=0; i<n; i++) out << v[i] << " ";
    return out;
}

/**
 * @brief 特化2维向量
 */
template<> struct vec<2> {
    double x = 0, y = 0;
    double& operator[](const int i)       { assert(i>=0 && i<2); return i ? y : x; }
    double  operator[](const int i) const { assert(i>=0 && i<2); return i ? y : x; }
};

/**
 * @brief 特化3维向量
 */
template<> struct vec<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i)       { assert(i>=0 && i<3); return i ? (1==i ? y : z) : x; }
    double  operator[](const int i) const { assert(i>=0 && i<3); return i ? (1==i ? y : z) : x; }
};

/**
 * @brief 特化4维向量
 */
template<> struct vec<4> {
    double x = 0, y = 0, z = 0, w = 0;
    double& operator[](const int i)       { assert(i>=0 && i<4); return i<2 ? (i ? y : x) : (2==i ? z : w); }
    double  operator[](const int i) const { assert(i>=0 && i<4); return i<2 ? (i ? y : x) : (2==i ? z : w); }
    vec<2> xy()  const { return {x, y};    }
    vec<3> xyz() const { return {x, y, z}; }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

/**
 * @brief 求向量的模长norm范数
 * 
 * @tparam n 
 * @param v 
 * @return double 
 */
template<int n> double norm(const vec<n>& v) {
    return std::sqrt(v * v);
}

/**
 * @brief 归一化向量
 * 
 * @tparam n 
 * @return vec<n> 
 */
template<int n> vec<n> normalized() {
    return v / norm(v);
}

/**
 * @brief 求3维向量的叉积
 * 
 * @param v1 
 * @param v2 
 * @return vec3 
 */
inline vec3 cross(const vec3 &v1, const vec3 &v2) {
    return { 
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x 
    };
}

/**
 * @brief 行列式
 * 
 * @tparam n 
 */
template<int n> struct dt;

/**
 * @brief 矩阵类
 * 
 * @tparam nrows 
 * @tparam ncols
 * @note 行主序
 */
template<int nrows, int ncols> struct mat {
    vec<ncols> rows[nrows] = {{}};
    vec<ncols>& operator[](const int idx) {
        assert(idx >=0 && idx < nrows);
        return rows[idx];
    }
    const vec<ncols>& operator[](const int idx) const {
        assert(idx >= 0 && idx < nrows);
        return rows[idx];
    }

    /**
     * @brief 求矩阵行列式的值
     * 
     * @return double 
     */
    double det() const {
        return dt<ncols>::det(*this);
    }

    /**
     * @brief 求矩阵的算术余子式
     * 
     * @param row 
     * @param col 
     * @return double 
     */
    double cofactor(const int row, const int col) const {
        mat<nrows - 1, ncols - 1> submatrix;
        for(int i = 0; i < nrows; i ++) {
            for(int j = 0; j < ncols; j ++) {
                submatrix[i][j] = rows[i + int(i >= row)][j + int(j >= col)];
            }
        }
        return submatrix.det() * ((row + col) % 2 ? 1 : -1);
    }
};

/**
 * @brief 矩阵左乘行向量
 * 
 * @tparam nrows 
 * @tparam ncols 
 * @param lhs 行向量
 * @param rhs 矩阵
 * @return vec<ncols> 
 */
template<int nrows, int ncols> vec<ncols> operator*(const vec<nrows>& lhs, const mat<nrows, ncols>& rhs) {
    return (mat<1, nrows>{{lhs}} * rhs)[0];
}