/**
 * @brief  nxn型行列Aとnベクトルxの積を並列に計算する
 * @date   2016/03/05
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <chrono>
#include <omp.h>
#include "../../../../MiLib/Random/xorshift.hpp"



//****************************************
// 型シノニム
//****************************************

using elem_t = std::int32_t;
using vec_t = std::vector<elem_t>;
using mat_t = std::vector<vec_t>;



//****************************************
// 関数の宣言
//****************************************

void matvec_mainloop(const mat_t& A, const vec_t& x, vec_t& y, int n, int i, int i_);



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  nxn型行列A=(aij)にnベクトルx=(xj)を掛ける問題を考える
 * @note   結果であるnベクトルy=(yi)はi=0,1,...,n-1に対して
 *           yi = Σ[i = 0, n-1](aij)*(xj)
 *         と定義される
 * @param  const mat_t& A nxn型行列A
 * @param  const vec_t& x nベクトルx
 * @return vec_t y
 */
vec_t matvec(const mat_t& A, const vec_t& x)
{
    int i;
    
    std::size_t n = A.size();  // n = A.rows
    vec_t y(n);                // yを長さnの新たなベクトルとする
#pragma omp parallel for private(i)
    for (i = 0; i < n; i++) {  // 並列に初期化
        y[i] = 0;
    }

    // メインループ
#pragma omp parallel for private(i)
    for (i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            y[i] = y[i] + A[i][j] * x[j];
        }
    }
    return y;
}



int main()
{   
    const int N = 8;

    vec_t x(N);
    mat_t A(N, vec_t(N));
    XorShiftRNG rng;
    
    std::cout << "A = ["; 
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rng() & 0x01;
            std:: cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "]" << std::endl;

    std::cout << "x = [";
    for (int i = 0; i < N; i++) {
        x[i] = 1;
        std::cout << x[i] << " ";
    }
    std::cout << "]" << std::endl;


    omp_set_dynamic(0);
    omp_set_num_threads(4);


     vec_t y;
     y = matvec(A, x);


    std::cout << "y = [";
    for (int i = 0; i < N; i++) {
        std::cout << y[i] << " ";
    }
    std::cout << "]" << std::endl;
    

    return 0;
}


void matvec_mainloop(const mat_t& A, const vec_t& x, vec_t& y,
                     int n, int i, int i_)
{
    if (i == i_) {
        for (int j = 0; j < n; j++) {
            y[i] = y[i] + A[i][j] * x[j];
        }
    }
    else {
        int mid = (i + i_) / 2;
#pragma omp parallel firstprivate(n, i, mid)
        matvec_mainloop(A, x, y, n, i, mid);

        matvec_mainloop(A, x, y, n, mid+1, i_);  
    }
}