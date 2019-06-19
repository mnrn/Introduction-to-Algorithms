/**
 * @brief  素朴な文字列照合アルゴリズムを扱います
 * @date   2016/03/31
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <string>
#include "naive_string_matcher.hpp"



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  素朴な文字列照合アルゴリズム
 *
 * @note   手続きNAIVE-STRING-MATCHERの実行にはΟ((n-m-1)m)時間かかり、この限界は最悪の場合には改良できない
 *         たとえば、テキスト文字列がa^n(n個のaからなる文字列)、パターンがa^mの場合を考える. n-m+1個の可能なシフトsの
 *         それぞれに対して、そのシフトが正当であることを確認するために、文字の比較をm回繰り返す必要がある
 *         したがって、最悪実行時間はΘ((n-m+1)m)となり、m = floor(n/2)ならば、Θ(n^2)である
 *         このアルゴリズムは前処理を必要とせず、NAIVE-STRING-MATCHERの実行時間は照合時間と一致する
 *
 * @param  string_t T テキストT
 * @param  string_t P パターンP
 */
void naive_string_matcher(string_t T, string_t P)
{
    strlen_t n = strlen(T);
    strlen_t m = strlen(P);

    // パターンを含む"テンプレート"をずらしてゆき、テンプレート上のすべての文字が
    // テキストの対応する文字と一致するシフトに出会えれば、それを"書き留めて"ゆく
    for (strlen_t s = 0; s <= n - m; s++) {

        // 現在のシフトが正当か無効かを判定する 
        strlen_t t;
        for (t = 0; t < m; t++) {
            if (T[s + t] != P[t]) { break; }
        }

        // 発見した正当なシフトsを出力する
        if (t == m) {
            std::cout << s << std::endl;
        }
    }
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;

    string T, P;
    cin >> T >> P;

    naive_string_matcher(T.c_str(), P.c_str());
    
    return 0;
}
