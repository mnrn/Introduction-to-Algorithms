/**
 * @brief 線形計画問題(linear-programming problem)における
 *        シンプレックスアルゴリズム(simplex algorithm)を扱います
 * @date  2016/03/16
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "class_simplex.hpp"



//****************************************
// 関数の宣言
//****************************************

static void dbg_print(const simplex::indices_t& N, const simplex::indices_t& B, const simplex::mat_t& A,
                      const simplex::vec_t& b,const simplex::vec_t& c, simplex::elem_t v);



//****************************************
// 関数の定義
//****************************************

int main()
{    
    const int m = 3, n = 3;
    
    simplex::indices_t N = { 1, 2, 3, };
    simplex::indices_t B = { 4, 5, 6, };

    simplex::vec_t b = { 0.0, 0.0, 0.0, 0.0, 30.0, 24.0, 36.0, };
    simplex::vec_t c = { 0.0, 3.0, 1.0, 2.0, 0.0,  0.0,  0.0,  };

    simplex::index_t l = 6;
    simplex::index_t e = 1;

    simplex::elem_t v = 0.0;

    simplex::mat_t A(m+n+1, simplex::vec_t(m+n+1, 0.0));

    
    A[4][0] = A[5][0] = A[6][0] = -1.0;
    A[4][1] = A[5][1] = A[6][1] = -1.0; 

    
    A[4][1] = 1.0, A[4][2] = 1.0, A[4][3] = 3.0;
    A[5][1] = 2.0, A[5][2] = 2.0, A[5][3] = 5.0;
    A[6][1] = 4.0, A[6][2] = 1.0, A[6][3] = 2.0;

    simplex simp(m, n);


    // std::tie(N, B, A, b, c, v) = simp.pivot(N, B, A, b, c, v, l, e);

    // dbg_print(N, B, A, b, c, v);

    auto x_ = simp.execute_no_init(N, B, A, b, c, v);

    for (auto xi_ : x_) {
        std::cout << xi_ << std::endl;
    }
    
    return 0;
}



static void dbg_print(const simplex::indices_t& N, const simplex::indices_t& B, const simplex::mat_t& A,
                      const simplex::vec_t& b,const simplex::vec_t& c, simplex::elem_t v)
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

