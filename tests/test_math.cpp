/**
 * @file tests/test_math.cpp
 * @brief tiny-renderer 的数学自测
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "math/geometry.h"

namespace {

/**
 * @brief 浮点近似比较
 *
 * @param a
 * @param b
 * @param eps
 * @return true
 */
inline bool nearly_equal(double a, double b, double eps) {
    // 绝对误差兜底，避免 b ~ 0 时相对误差不稳定。
    const double diff = std::fabs(a - b);
    const double scale = 1.0 + std::max(std::fabs(a), std::fabs(b));
    return diff <= eps * scale;
}

/// @brief 测试失败计数
int g_failures = 0;

/**
 * @brief 失败时记录并输出（不中断后续测试）
 *
 * @param ok
 * @param expr
 * @param file
 * @param line
 * @param msg
 */
inline void check(bool ok, const char* expr, const char* file, int line, const std::string& msg = {}) {
    if (ok) return;
    ++g_failures;
    std::cerr << file << ":" << line << ": FAIL: " << expr;
    if (!msg.empty()) std::cerr << " | " << msg;
    std::cerr << "\n";
}

/**
 * @brief CHECK 使用可变参数宏，避免逗号导致宏参数拆分
 */
#define CHECK(...) ::check((__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__)

#define CHECK_NEAR(A, B, EPS)                                                                    \
    do {                                                                                         \
        const double _a = static_cast<double>(A);                                                 \
        const double _b = static_cast<double>(B);                                                 \
        const double _eps = static_cast<double>(EPS);                                             \
        ::check(::nearly_equal(_a, _b, _eps), #A " ~= " #B, __FILE__, __LINE__,                   \
                std::string("a=") + std::to_string(_a) + ", b=" + std::to_string(_b) +           \
                    ", eps=" + std::to_string(_eps));                                             \
    } while (0)

/**
 * @brief vec 逐分量近似比较
 *
 * @tparam n
 * @param a
 * @param b
 * @param eps
 * @return true
 */
template<int n>
inline bool vec_nearly_equal(const vec<n>& a, const vec<n>& b, double eps) {
    for (int i = 0; i < n; ++i) {
        if (!nearly_equal(a[i], b[i], eps)) return false;
    }
    return true;
}

/**
 * @brief mat 逐元素近似比较
 *
 * @tparam r
 * @tparam c
 * @param a
 * @param b
 * @param eps
 * @return true
 */
template<int r, int c>
inline bool mat_nearly_equal(const mat<r, c>& a, const mat<r, c>& b, double eps) {
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            if (!nearly_equal(a[i][j], b[i][j], eps)) return false;
        }
    }
    return true;
}

/**
 * @brief 构造 n 维单位矩阵
 *
 * @tparam n
 * @return mat<n,n>
 */
template<int n>
inline mat<n, n> identity() {
    mat<n, n> I;
    for (int i = 0; i < n; ++i) I[i][i] = 1.0;
    return I;
}

/**
 * @brief 测试 vec：加/减/点乘/标量乘除
 */
void test_vec_basic_ops() {
    constexpr double eps = 1e-12;

    const vec3 a = {1, 2, 3};
    const vec3 b = {-2, 4, 0.5};

    CHECK(vec_nearly_equal<3>(a + b, vec3{-1, 6, 3.5}, eps));
    CHECK(vec_nearly_equal<3>(a - b, vec3{3, -2, 2.5}, eps));

    // 点乘：a·b = 1*(-2) + 2*4 + 3*0.5 = 7.5
    CHECK_NEAR(a * b, 7.5, eps);

    CHECK(vec_nearly_equal<3>(a * 2.0, vec3{2, 4, 6}, eps));
    CHECK(vec_nearly_equal<3>(2.0 * a, vec3{2, 4, 6}, eps));
    CHECK(vec_nearly_equal<3>(a / 2.0, vec3{0.5, 1, 1.5}, eps));
}

/**
 * @brief 测试 norm/normalized
 */
void test_norm_and_normalized() {
    constexpr double eps = 1e-12;

    const vec3 v = {3, 4, 0};
    CHECK_NEAR(norm(v), 5.0, eps);

    const vec3 u = normalized(v);
    CHECK_NEAR(norm(u), 1.0, 1e-10);

    // 方向一致：归一化后与原向量夹角为 0（点乘等于模长乘积）
    CHECK_NEAR(v * u, norm(v) * norm(u), 1e-10);
}

/**
 * @brief 测试叉乘：右手定则/正交性/反对称
 */
void test_cross_product() {
    constexpr double eps = 1e-12;

    const vec3 x = {1, 0, 0};
    const vec3 y = {0, 1, 0};
    const vec3 z = {0, 0, 1};

    const vec3 xy = cross(x, y);
    CHECK(vec_nearly_equal<3>(xy, z, eps));

    // 正交性：x·(x×y)=0, y·(x×y)=0
    CHECK_NEAR(x * xy, 0.0, eps);
    CHECK_NEAR(y * xy, 0.0, eps);

    // 反对称：x×y = -(y×x)
    const vec3 yx = cross(y, x);
    CHECK(vec_nearly_equal<3>(xy + yx, vec3{0, 0, 0}, eps));
}

/**
 * @brief 测试矩阵：乘法/转置
 */
void test_matrix_multiply_and_transpose() {
    constexpr double eps = 1e-12;

    const mat<2, 3> A = {{{1, 2, 3}, {4, 5, 6}}};
    const mat<3, 2> B = {{{7, 8}, {9, 10}, {11, 12}}};
    const mat<2, 2> C = A * B;

    // 手算：
    // [1 2 3] [ 7  8]   [ 58  64]
    // [4 5 6] [ 9 10] = [139 154]
    //         [11 12]
    const mat<2, 2> C_expect = {{{58, 64}, {139, 154}}};
    CHECK(mat_nearly_equal<2, 2>(C, C_expect, eps));

    const mat<3, 2> At = A.transpose();
    const mat<3, 2> At_expect = {{{1, 4}, {2, 5}, {3, 6}}};
    CHECK(mat_nearly_equal<3, 2>(At, At_expect, eps));
}

/**
 * @brief 测试行列式：2x2/3x3
 */
void test_determinant() {
    constexpr double eps = 1e-12;

    // det([[4,7],[2,6]]) = 4*6 - 7*2 = 10
    const mat<2, 2> M2 = {{{4, 7}, {2, 6}}};
    CHECK_NEAR(M2.det(), 10.0, eps);

    // 经典 3x3：det = 1
    const mat<3, 3> M3 = {{{1, 2, 3}, {0, 1, 4}, {5, 6, 0}}};
    CHECK_NEAR(M3.det(), 1.0, eps);
}

/**
 * @brief 测试逆矩阵：A * A^{-1} ≈ I
 */
void test_inverse() {
    constexpr double eps = 1e-9;

    const mat<2, 2> A2 = {{{4, 7}, {2, 6}}};
    const mat<2, 2> inv2 = A2.invert();
    CHECK(mat_nearly_equal<2, 2>(A2 * inv2, identity<2>(), eps));
    CHECK(mat_nearly_equal<2, 2>(inv2 * A2, identity<2>(), eps));

    // det=1 的 3x3，更适合直接对比期望值
    const mat<3, 3> A3 = {{{1, 2, 3}, {0, 1, 4}, {5, 6, 0}}};
    const mat<3, 3> inv3 = A3.invert();
    const mat<3, 3> inv3_expect = {
        {{-24, 18, 5}, {20, -15, -4}, {-5, 4, 1}},
    };
    CHECK(mat_nearly_equal<3, 3>(inv3, inv3_expect, 1e-8));
    CHECK(mat_nearly_equal<3, 3>(A3 * inv3, identity<3>(), eps));
    CHECK(mat_nearly_equal<3, 3>(inv3 * A3, identity<3>(), eps));
}

} // namespace

/**
 * @brief 自测入口
 *
 * @return 0 表示通过
 */
int main() {
    test_vec_basic_ops();
    test_norm_and_normalized();
    test_cross_product();
    test_matrix_multiply_and_transpose();
    test_determinant();
    test_inverse();

    if (g_failures == 0) {
        std::cout << "test_math: all tests passed\n";
        return 0;
    }

    std::cerr << "test_math: failed cases = " << g_failures << "\n";
    return 1;
}
