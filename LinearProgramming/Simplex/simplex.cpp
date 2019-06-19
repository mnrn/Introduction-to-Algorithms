/**
 * @brief 線形計画問題(linear-programming problem)における
 *        シンプレックスアルゴリズム(simplex algorithm)を扱います
 * @date  2016/03/14
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <cfloat>
#include <climits>
#include <cmath>
#include "simplex.hpp"




//****************************************
// 大域変数の定義
//****************************************

static const elem_t inf = std::numeric_limits<elem_t>::max() / 3;
static const elem_t eps = std::numeric_limits<elem_t>::epsilon();



//****************************************
// 関数の宣言
//****************************************

static void dbg_print(const indices_t& N, const indices_t& B, const mat_t& A,
                      const vec_t& b,const vec_t& c, elem_t v);



//****************************************
// 関数の定義
//****************************************

static inline bool approximately_equal(elem_t a, elem_t b, elem_t epsilon)
{
    using namespace std;
    return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

static inline bool essentially_equal(elem_t a, elem_t b, elem_t epsilon)
{
    using namespace std;
    return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}




/**
 * @brief  ピボット手続き
 *
 * @note   手続きPIVOTは6項組(N, B, A, b, c, v)で与えられているスラック形、
 *         基底から出る変数xlの添字l, 基底に入る変数xeの添字eを入力として取り、
 *         そして新しいスラック形を記述する6項組(N^, B^, A^, b^, c^, v^)を返す
 *
 * @note   m x n型行列AとA^の要素は、実際には、スラック形に表れる係数の反転であることに注意
 */
std::tuple<indices_t, indices_t, mat_t, vec_t, vec_t, elem_t>
pivot(const indices_t& N, const indices_t& B, const mat_t& A,
      const vec_t& b,const vec_t& c, elem_t v, index_t l, index_t e)
{
    std::size_t m = B.size();
    std::size_t n = N.size();

    vec_t b_(m+n, 0), c_(m+n, 0);
    
    
    // STEP1: 新しい基底変数xeに対する式の係数を計算する
    mat_t A_(m+n, vec_t(m+n, 0));
    
    b_[e] = b[l] / A[l][e];

    for (index_t j : N) {
        if (j == e) { continue; }
        A_[e][j] = A[l][j] / A[l][e];
    }
    A_[e][l] = 1 / A[l][e];
    

    // STEP2: 残りの制約の係数を計算する
    for (index_t i : B) {
        if (i == l) { continue; }
        b_[i] = b[i] - A[i][e] * b_[e];
        
        for (index_t j : N) {
            if (j == e) { continue; }
            A_[i][j] = A[i][j] - A[i][e] * A_[e][j];
        }

        A_[i][l] = -(A[i][e] * A_[e][l]);
    }
    

    // STEP3: 目的関数を計算する
    elem_t v_ = v + c[e] * b_[e];
    for (index_t j : N) {
        if (j == e) { continue; }
        c_[j] = c[j] - c[e] * A_[e][j];
    }
    c_[l] = -(c[e] * A_[e][l]);
    

    // STEP4: 基底変数と非基底変数の新しい集合を計算する
    indices_t N_(N);  for (index_t& j : N_) { if (j == e) { j = l; } }
    indices_t B_(B);  for (index_t& i : B_) { if (i == l) { i = e; } }

    
    // 新しいスラック形を返す
    return std::make_tuple(N_, B_, A_, b_, c_, v_);
}



vec_t simplex_no_init(indices_t& N, indices_t& B, mat_t& A, vec_t& b, vec_t& c, elem_t v)
{
    std::int32_t m = B.size();
    std::int32_t n = N.size();

    
    auto get_positive_coefficient = [](const vec_t& c, const indices_t& N) -> index_t
        { for (index_t j : N) { if (c[j] > 0) { return j; } } return -1; };
   
    auto min_index = [](const vec_t& delta, const indices_t& B) -> index_t
        { index_t min_i = B[0]; for (index_t i : B) { if (delta[min_i] > delta[i]) { min_i = i; } } return min_i; };

    auto in_set = [](const indices_t& B, index_t i) -> bool
        { for (index_t j : B) { if (j == i) { return true; } } return false; };
    
        
    vec_t delta(m + n);  // Δを長さがmの新しいベクトルとする(ただし、記憶領域はm+nだけ確保する)


    
    while (true) {
        index_t e = get_positive_coefficient(c, N);  // ce > 0を満たすe ∈ Nを選択する
        if (e < 0) { break; }                        // 目的関数のすべての係数が負なら、while文は終了する
        
        // 各制約を調べ、どの非負制約にも違反することなくxeを増やせる量を、最も厳しく制限する制約を選ぶ
        for (index_t i : B) {       // for each index i ∈ B
            if (A[i][e] > 0) {
                delta[i] = b[i] / A[i][e];
            }
            else {
                delta[i] = inf;
            }
        }
        index_t l = min_index(delta, B);  // Δlを最小化する添字l ∈ Bを選択する(この制約に対する基底変数がxlである)
        
        if (approximately_equal(delta[l], inf, eps)) {  // 基底に入る変数の値を制限なく増やせるならば、
            std::cerr << "infinity..." << std::endl;    // "有界ではない"と出力し、
            return { };                                 // 空集合を返す
        }                                               // そうでなければ、PIVOT(N, B, A, b, c, v, l, e)を呼び出し、
        else {                                          // 基底に入る変数と基底から出る変数の役割を交換する
            std::tie(N, B, A, b, c, v) = pivot(N, B, A, b, c, v, l, e);
        }
    }


    // 元の線形計画法の変数に対する解x0_, x1_, ..., x(n-1)_を計算する
    vec_t x_(n+m);
    for (index_t i = 0; i < n+m; i++) {
        if (in_set(B, i)) {
            x_[i] = b[i];  // 基底変数xi_をbiに
        }
        else {
            x_[i] = 0.0;   // 非基底変数を0に設定し、
        }
    }
    return x_;             // これらの値を返す
}


int main()
{
    
    
    const int m = 3, n = 3;
    
    indices_t N = { 0, 1, 2, };
    indices_t B = { 3, 4, 5, };

    vec_t b = { 0.0, 0.0, 0.0, 30.0, 24.0, 36.0, };
    vec_t c = { 3.0, 1.0, 2.0, 0.0,  0.0,  0.0,  };

    index_t l = 5;
    index_t e = 0;

    elem_t v = 0.0;

    mat_t A(m+n, vec_t(n+n, 0.0));

    A[3][0] = 1.0, A[3][1] = 1.0, A[3][2] = 3.0;
    A[4][0] = 2.0, A[4][1] = 2.0, A[4][2] = 5.0;
    A[5][0] = 4.0, A[5][1] = 1.0, A[5][2] = 2.0;

    // tie(N, B, A, b, c, v) = pivot(N, B, A, b, c, v, l, e);

    // dbg_print(N, B, A, b, c, v);

    auto x_ = simplex_no_init(N, B, A, b, c, v);

    for (auto xi_ : x_) {
        std::cout << xi_ << std::endl;
    }
    
    return 0;
}



static void dbg_print(const indices_t& N, const indices_t& B, const mat_t& A,
                      const vec_t& b,const vec_t& c, elem_t v)
{
    using namespace std;
    
    cout << "N : ";
    for (auto j : N) { cout << j << " "; }
    cout << endl;
    
    cout << "B : ";
    for (auto i : B) { cout << i << " "; }
    cout << endl;

    cout << "b : ";
    for (auto i : b) { cout << i << " "; }
    cout << endl;

    cout << "c : ";
    for (auto j : c) { cout << j << " "; }
    cout << endl;
    
    cout << "v : ";
    cout << v << endl;

    for (auto i : B) {
        for (auto j : N) {
            cout << "A[" << i << "][" << j << "] = " << A[i][j] << " ";
        }
        cout << endl;
    }
}

