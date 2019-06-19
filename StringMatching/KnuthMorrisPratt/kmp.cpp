/**
 * @brief  Knuth, Morris, Prattによる線形時間の文字列照合アルゴリズムを扱う
 *
 * @note  このアルゴリズムは遷移関数δを一度に計算するのを避け、補助関数πだけを用いて照合時間Θ(n)を達成する
 *        ここでは、πは事前にパターンからΘ(m)時間をかけて計算し、配列π[0..m-1]に保存しておく
 *        遷移関数δは必要になった時点で"動的に"配列πから(ならし時間の意味で)効率よく計算する
 *        大雑把に言えば、任意の状態q = 0,1,...,mと任意の文字a ∈ Σに対して、値π[q]はδ(q, a)の計算に必要な
 *        すべての情報を含むが、この情報はaに依存しない. 配列πはm個の要素しか持たず、一方、δはΘ(m|Σ|)個の
 *        要素を持つから、δではなくπを計算することで、前処理時間を1/|Σ|に削減できる
 *
 * @date   2016/04/28
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "kmp.hpp"



 //****************************************
// 関数の宣言
//****************************************

static states_t compute_prefix_function(string_t P);



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  テキストTをPに対して照合する
 * @note   KMP-MATCHERの照合時間はΘ(n)となる
 *
 * @param  string_t T テキストT
 * @param  string_t P パターンP
 */
void kmp_matcher(string_t T, string_t P)
{
    strlen_t n = strlen(T);
    strlen_t m = strlen(P);

    states_t pi = compute_prefix_function(P);
    state_t  q = 0;  // 一致した文字数

    for (strlen_t i = 0; i < n; i++) {                      // 左から右にテキストを走査
        
        while (q > 0 && (P[q] != T[i])) { q = pi[q - 1]; }  // 次の文字が一致しない

        if (P[q] == T[i]) { q = q + 1; }                    // 次の文字が一致した

        if (q == m) {                                       // Pが完全に一致したか判定
            std::cout << i - m + 1 << std::endl;
            q = pi[q - 1];                                  // 次のシフトを探す
        }
    }
}


/**
 * @brief  COMPUTE-PREFIX-FUNCTIONはPをそれ自身に対して照合する
 * @note   COMPUTE-PREFIX-FUNCTIONの実行時間はΘ(m)である
 *
 * @param  string_t P パターンP
 * @return 接頭語関数(prefix function) π[q] = max { k : k < q かつ Pk ] Pq }
 */
states_t compute_prefix_function(string_t P)
{
    strlen_t m = strlen(P);
    states_t pi(m);

    pi[0] = 0;
    strlen_t k = 0;  // 一致した文字数

    for (state_t q = 1; q < m; q++) {                       // 左から右へテキストを走査

        while (k > 0 && (P[k] != P[q])) { k = pi[k - 1]; }  // 次の文字が一致しない
        
        if (P[k] == P[q]) { k = k + 1; }                    // 次の文字が一致した

        pi[q] = k;                                          // kとπ[q]は正しく設定される
    }

    return pi;
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;

    string T, P;

    cin >> T >> P;

    kmp_matcher(T.c_str(), P.c_str());
    
    return 0;
}
