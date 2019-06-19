/**
 * @brief 線形計画問題(linear-programming problem)における
 *        シンプレックスアルゴリズム(simplex algorithm)を扱います
 * @date  2016/03/16
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __CLASS_SIMPLEX_HPP__
#define __CLASS_SIMPLEX_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <tuple>
#include <vector>
#include <cstdint>
#include <cfloat>
#include <climits>
#include <cmath>



//****************************************
// 構造体の定義
//****************************************

struct simplex {
    
    using elem_t    = double;
    using vec_t     = std::vector<elem_t>;
    using mat_t     = std::vector<vec_t>;
    using index_t   = std::int32_t;
    using indices_t = std::vector<index_t>;

    const std::int32_t _m, _n;
    
    
    simplex(std::int32_t m, std::int32_t n) : _m(m), _n(n) { }
    
    

    std::tuple<indices_t, indices_t, mat_t, vec_t, vec_t, elem_t>
    pivot(const indices_t& N, const indices_t& B, const mat_t& A,
          const vec_t& b,const vec_t& c, elem_t v, index_t l, index_t e)
    {
        vec_t b_(_m + _n + 1, 0), c_(_m + _n + 1, 0);
    
    
        // STEP1: 新しい基底変数xeに対する式の係数を計算する
        mat_t A_(_m + _n + 1, vec_t(_m + _n + 1, -1));
    
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



    vec_t execute_no_init(indices_t& N, indices_t& B, mat_t& A, vec_t& b, vec_t& c, elem_t v)
    {
    
        auto get_positive_coefficient = [](const vec_t& c, const indices_t& N) -> index_t
            { for (index_t j : N) { if (c[j] > 0) { return j; } } return 0; };
   
        auto min_index = [](const vec_t& delta, const indices_t& B) -> index_t
            { index_t min_i = B[1]; for (index_t i : B) { if (delta[min_i] > delta[i]) { min_i = i; } } return min_i; };

        auto in_set = [](const indices_t& B, index_t i) -> bool
            { for (index_t j : B) { if (j == i) { return true; } } return false; };
    
        
        vec_t delta(_m + _n + 1);  // Δを長さがmの新しいベクトルとする(ただし、記憶領域はm+n+1だけ確保する)


    
        while (true) {
            index_t e = get_positive_coefficient(c, N);  // ce > 0を満たすe ∈ Nを選択する
            if (e <= 0) { break; }                       // 目的関数のすべての係数が負なら、while文は終了する
        
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
        
            if (approximately_equal(delta[l], inf)) {       // 基底に入る変数の値を制限なく増やせるならば、
                std::cerr << "infinity..." << std::endl;    // "有界ではない"と出力し、
                return { };                                 // 空集合を返す
            }                                               // そうでなければ、PIVOT(N, B, A, b, c, v, l, e)を呼び出し、
            else {                                          // 基底に入る変数と基底から出る変数の役割を交換する
                std::tie(N, B, A, b, c, v) = pivot(N, B, A, b, c, v, l, e);
            }
        }


        // 元の線形計画法の変数に対する解x1_, x1_, ..., xn_を計算する
        vec_t x_(_n + _m + 1);
        for (index_t i = 1; i <= _n + _m; i++) {
            if (in_set(B, i)) {
                x_[i] = b[i];  // 基底変数xi_をbiに
            }
            else {
                x_[i] = 0.0;   // 非基底変数を0に設定し、
            }
        }
        return x_;             // これらの値を返す
    }



private:

    const elem_t inf { std::numeric_limits<elem_t>::max() / 3 };
    const elem_t eps { std::numeric_limits<elem_t>::epsilon() };
    
    bool approximately_equal(elem_t a, elem_t b)
    {
        using namespace std;
        return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * eps);
    }

    bool essentially_equal(elem_t a, elem_t b)
    {
        using namespace std;
        return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * eps);
    }
};



#endif  // end of __CLASS_SIMPLEX_HPP__

