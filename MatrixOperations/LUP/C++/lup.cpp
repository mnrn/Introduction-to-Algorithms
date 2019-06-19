/**
 * @bfief LUP分解について扱う
 * @date  2016/03/09
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <cmath>
#include <cassert>
#include "lup.hpp"



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  与えられたP, L, Uおよびbに対して、手続きLUP-SOLVEは前進代入と後進代入を組み合わせて解xを得る
 * @note   ただし、置換行列Pは配列πで表現される
 *
 * @note   手続きの実行時間はΘ(n^2)である
 *
 * @param  const mat_t& L       単位下三角行列
 * @param  const mat_t& U       上三角行列
 * @param  const indices_t& pi  置換行列Pの配列表現(i=0,1,..n-1に対してpi[i]はP(i,pi[i])=1で、j!=pi[i]ならばPij=0)
 * @param  const vec_t& b       入力ベクトルb
 * @return vec_t x              LUx = Pbを満たすようなx
 */
vec_t lup_solve(const mat_t& L, const mat_t& U, const indices_t& pi, const vec_t& b)
{
    std::int32_t n = L.size();   // n = L.rows

    vec_t x(n, 0.0), y(n, 0.0);  // x, yを長さnの新しいベクトルとする

    for (index_t i = 0; i < n; i++) {       // 前進代入(forward substitution)を行ってyを計算する
        elem_t sigma = 0.0;
        for (index_t j = 0; j < i; j++) { sigma += L[i][j] * y[j]; }  // Σ[j=0, i-1](lij * yj)を計算
        y[i] = b[pi[i]] - sigma;            // y0,y1,...,y(i-1)をi番目の式に"前の方から順々に"代入することでyiの値を得る
    }

    for (index_t i = n - 1; i >= 0; i--) {  // 後進代入(back subsititusion)を行ってxを得る
        elem_t sigma = 0.0;
        for (index_t j = i; j < n; j++) { sigma += U[i][j] * x[j]; }  // Σ[j=i, n-1](uij * xj)を計算
        x[i] = (y[i] - sigma) / U[i][i];    // この方法は前進代入法と似ているが、対角成分で要素を割る必要がある
    }

    return x;
}


/**
 * @brief  A = LUとなるLとUを求めること、すなわち、AのLU分解(LU decomposition)を計算する
 *
 * @note   LU分解を生成するためにガウスの消去法(Gaussian elimination)と呼ばれる手続きを用いる
 *         最初に、1番目の方程式を何倍かしたものを2番目以降の各方程式から引き、1番目の変数を消去する
 *         つぎに、2番目の方程式を何倍かしたものを3番目以降の各式から引き、1番目と2番目の変数を消去する
 *         この手続きを残された系が上三角形になるまで繰り返す. こうして生成されるのが行列Uである
 *         行列Lは変数を消去するために各方程式にかけた値から構成される
 *
 * @note   a00 = 0のとき、この方法は0による除算を含み、うまく働かない
 *         Schur補行列A'-vw^T/a00の左上隅の要素が0のときも、0による除算が現れるので、同様にうまく働かない
 *         LU分解の過程で除算として使用する要素をピボット(pivot)と呼ぶ. これらの数は行列Uの対角線上に現れる
 *         LUP分解で置換行列Pを使うのは0による除算を避けるためである
 *         置換行列を使って0による除算(あるいは、数値的不安定性の要因となり得る小さい数による除算)を避けることを、
 *         ピボット選択(pivoting)と呼ぶ
 *
 * @note   LU-DECOMPOSITIONはΘ(n^3)時間で走る
 *
 * @param[inout] mat_t& A 正則行列A
 * @return std::pair<mat_t, mat_t> A= LUとなるLとU
 */
std::pair<mat_t, mat_t> lu_decomposition(mat_t& A)
{
    std::int32_t n = A.size();                          // n = A.rows

    mat_t L(n, vec_t(n, 0.0)), U(n, vec_t(n, 0.0));     // LとUを新しいn x n型行列とする

    for (index_t i = 0; i < n; i++) { L[i][i] = 1.0; }  // Lの対角線上を1に初期化する

    for (index_t k = 0; k < n; k++) {
        U[k][k] = A[k][k];                              // ピボットをukk = akkに決める
        for (index_t i = k + 1; i < n; i++) {           // k = n - 1のときには実行されない
            // ベクトルvとw^Tを用いてLとUを更新する
            L[i][k] = A[i][k] / U[k][k];                // lijはviの値を持つ
            U[k][i] = A[k][i];                          // ukiはwi^Tの値を持つ
        }

        // Schur捕行列の各要素を計算して行列Aに書き戻す
        for (index_t i = k + 1; i < n; i++) {
            for (index_t j = k + 1; j < n; j++) {
                A[i][j] = A[i][j] - L[i][k] * U[k][j];  // lij計算時にakkですでに割っていることに注意
            }
        }
    }
    return std::make_pair(L, U);
}


/**
 * @brief  正則行列AのLUP分解を行う
 *
 * @note   置換行列Pは配列πとして動的に管理する. ここでπ[i] = jはPの第i行は第j列に1を持つことを意味する
 *         さらに、A上でLとUを「その場で」計算する. したがって、手続きが終了したときは
 *           aij = { lij i >  jのとき
 *                   uij i <= jのとき }
 *         が成立する
 *
 * @note   LUP-DECOMPOSITIONの実行時間はΘ(n^3)である
 *
 * @param[inout] mat_t& A  nxn型の正則行列A
 * @return indices_t pi    置換行列Pの配列表現
 */
indices_t lup_decomposition(mat_t& A)
{
    std::int32_t n = A.size();  // n = A.rows
    
    indices_t pi(n);            // π[0..n-1]を新しい配列とする

    for (index_t i = 0; i < n; i++) {
        pi[i] = i;
    }

    for (index_t k = 0; k < n; k++) {
        // 0による除算および数値的不安定性を避けるため、できるだけ大きい数をピボットとして選択する
        elem_t p = 0.0; index_t k_ = -1;
        for (index_t i = k; i < n; i++) {
            if (std::abs(A[i][k]) > std::abs(p)) {
                p = A[i][k];
                k_ = i;
            }
        }
        if (k_ < 0) { assert(false); }     // p == 0.0のとき、Aは特異行列

        // 要素ak'kをピボットの位置(k, k)に移すために、
        std::swap(pi[k], pi[k_]);          // π[k]とπ[k_]を交換し、
        for (index_t i = 0; i < n; i++) {  // 行全体を入れ換えるために、
            std::swap(A[k][i], A[k_][i]);  // akiとak'iを交換していく
        }

        // Schurの補行列を計算する
        for (index_t i = k + 1; i < n; i++) {
            A[i][k] = A[i][k] / A[k][k];
            for (index_t j = k + 1; j < n; j++) {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
    }
    return pi;
}


/**
 * @brief  与えられたP, L, Uおよびbに対して、手続きLUP-SOLVEは前進代入と後進代入を組み合わせて解xを得る
 * @note   ただし、置換行列Pは配列πで表現される
 *
 * @note   LUP-SOLVEは引数にLおよびUの2つをとる必要があった. そこで、Aに直接LおよびUを計算した値を代入した行列Aのみを
 *         引数にとるように修正を施す
 *
 * @note   手続きの実行時間はΘ(n^2)である
 *
 * @param  const mat_t& A       aij = { lij (i > j)、 uij (i <= j) }を満たすnxn型行列A
 * @param  const indices_t& pi  置換行列Pの配列表現(i=0,1,..n-1に対してpi[i]はP(i,pi[i])=1で、j!=pi[i]ならばPij=0)
 * @param  const vec_t& b       入力ベクトルb
 * @return vec_t x              LUx = Pbを満たすようなx
 */
vec_t lup_solve(const mat_t& A, const indices_t& pi, const vec_t& b)
{
    std::int32_t n = A.size();              // n = A.rows

    vec_t x(n, 0.0), y(n, 0.0);             // x, yを長さnの新しいベクトルとする

    for (index_t i = 0; i < n; i++) {       // 前進代入(forward substitution)を行ってyを計算する
        elem_t sigma = 0.0;
        for (index_t j = 0; j < i; j++) { sigma += A[i][j] * y[j]; }  // Σ[j=0, i-1](lij * yj)を計算
        y[i] = b[pi[i]] - sigma;            // y0,y1,...,y(i-1)をi番目の式に"前の方から順々に"代入することでyiの値を得る
    }

    for (index_t i = n - 1; i >= 0; i--) {  // 後進代入(back subsititusion)を行ってxを得る
        elem_t sigma = 0.0;
        for (index_t j = i; j < n; j++) { sigma += A[i][j] * x[j]; }  // Σ[j=i, n-1](uij * xj)を計算
        x[i] = (y[i] - sigma) / A[i][i];    // この方法は前進代入法と似ているが、対角成分で要素を割る必要がある
    }
    
    return x;


    // y[n - 1] /= A[n - 1][n - 1];            // i = n - 1の場合をループの外で先に行う
    // for (index_t i = n - 2; i >= 0; i--) {  // 後進代入(back subsititusion)を行ってxを得る
    //     elem_t sigma = y[i];
    //     for (index_t j = i + 1; j < n; j++) { sigma -= A[i][j] * y[j]; }  // Σ[j=i+1, n-1](uij * xj)を計算
    //     y[i] = sigma / A[i][i];             // この方法は前進代入法と似ているが、対角成分で要素を割る必要がある
    // }
    // return y;
}


int main()
{
    const int N = 3;
    
    vec_t     b  = { 3.0, 7.0, 8.0, };
    indices_t pi = { 2, 0, 1, };

    mat_t L(N, vec_t(N)), U(N, vec_t(N));

    elem_t ltbl[N][N] = { { 1.0, 0.0, 0.0, },
                          { 0.2, 1.0, 0.0, },
                          { 0.6, 0.5, 1.0, }, };

    elem_t utbl[N][N] = { { 5.0, 6.0, 3.0,  },
                          { 0.0, 0.8, -0.6, },
                          { 0.0, 0.0, 2.5,  }, };

    std::cout << "L: " << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            L[i][j] = ltbl[i][j];
            std::cout << L[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "U: " << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            U[i][j] = utbl[i][j];
            std::cout << U[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    vec_t x = lup_solve(L, U, pi, b);

    std::cout << "LUP solved: " << std::endl;
    for (int i = 0; i < N; i++) {
        std::cout << x[i] << " ";
    }
    std::cout << std::endl;

    
    std::cout << std::endl;

    
    const int M = 4;

    mat_t A(M, vec_t(M));

    elem_t atbl[M][M] = {
        {  2.0,  0.0,  2.0,  0.6, },
        {  3.0,  3.0,  4.0, -2.0, },
        {  5.0,  5.0,  4.0,  2.0, },
        { -1.0, -2.0,  3.4, -1.0, },
    };

    std::cout << "とある行列A: " << std::endl;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            A[i][j] = atbl[i][j];
            std::cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    indices_t _pi = lup_decomposition(A);

    std::cout << "LUP分解後: " << std::endl;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            std::cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "pi: " << std::endl;
    for (int i = 0; i < M; i++) {
        std::cout << _pi[i] << " ";
    }
    std::cout << std::endl;


    std::cout << std::endl;
    

    elem_t atbl2[M][M] = {
        {  2.0,  3.0,  1.0,  5.0, },
        {  6.0, 13.0,  5.0, 19.0, },
        {  2.0, 19.0, 10.0, 23.0, },
        {  4.0, 10.0, 11.0, 31.0, },
    };

    std::cout << "とある行列A: " << std::endl;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            A[i][j] = atbl2[i][j];
            std::cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    
    auto LU = lu_decomposition(A);
    std::cout << "LU分解: " << std::endl;

    std::cout << "L: " << std::endl;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            std::cout << LU.first[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "U: " << std::endl;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            std::cout << LU.second[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;


    mat_t _A(N, vec_t(N));

    elem_t lutbl[N][N] = {
        {  5.0,  6.0,  3.0, },
        {  0.2,  0.8, -0.6, },
        {  0.6,  0.5,  2.5, },
    };

    std::cout << "LU: " << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            _A[i][j] = lutbl[i][j];
            std::cout << _A[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << std::endl;

    x = lup_solve(_A, pi, b);

    std::cout << "LUP solved: " << std::endl;
    for (int i = 0; i < N; i++) {
        std::cout << x[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}

