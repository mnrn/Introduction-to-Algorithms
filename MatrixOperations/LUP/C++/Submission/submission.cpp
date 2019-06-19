/**
 * @bfief LUP分解について扱う
 *
 * @note  関連URL : http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=0004
 *
 * @date  2016/04/03
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <functional>



//****************************************
// 型シノニム
//****************************************

using elem_t    = double;
using vec_t     = std::vector<elem_t>;
using mat_t     = std::vector<vec_t>;
using index_t   = std::int32_t;
using indices_t = std::vector<index_t>;



//****************************************
// 関数の定義
//****************************************

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
    index_t n = A.size();       // n = A.rows
    
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
    index_t n = A.size();                   // n = A.rows

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
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;
    const int n = 2;

    cout.setf(ios::fixed);
    cout.precision(3);
    
    int a, b, c, d, e, f;
    while (cin >> a >> b >> c >> d >> e >> f) {
        vec_t _b(n);
        _b[0] = c; _b[1] = f;

        mat_t A(n, vec_t(2));

        A[0][0] = a;
        A[0][1] = b;
        A[1][0] = d;
        A[1][1] = e;

        indices_t pi = lup_decomposition(A);

        vec_t x = lup_solve(A, pi, _b);

        if (fabs(x[0]) < 0.001) { cout << 0.000 << " ";  }
        else                    { cout << x[0]  << " ";  }

        if (fabs(x[1]) < 0.001) { cout << 0.000 << endl; }
        else                    { cout << x[1]  << endl; }
    }
    
    return 0;
}

