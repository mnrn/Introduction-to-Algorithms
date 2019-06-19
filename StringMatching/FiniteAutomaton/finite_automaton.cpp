/**
 * @brief  有限オートマトンを用いる文字列照合を扱います
 *
 * @note   有限オートマトン(finite automaton)Mは5項組(Q, q0, A, Σ, δ)である. ここで、
 *
 *         * Qは状態(state)の有限集合
 *         * q0 ∈ Qは開始状態(start state)
 *         * A ⊆ Qは受理状態(accepting state)の集合
 *         * Σは有限入力アルファベット(input alphabet)
 *         * δは、Mの遷移関数(transition function)と呼ぶ、Q x ΣからQへの関数である
 *
 * @note   有限オートマトンは状態q0から開始し、入力文字列を1文字ずつ読む. オートマトンの状態がqのときに文字aを読めば、
 *         状態はδ(q, a)に移る("遷移する"). 現在の状態qがAの要素ならば、機械Mは、これまで読み込んだ文字列を受理する(accepted)
 *         受理されなかった入力は却下される(rejected)
 *
 * @note   有限オートマトンMは最終状態関数(final-state function)と呼ぶ、Σ*からQへの関数φを誘導する
 *         ここで、φ(w)はMが文字列wを操作し終わった時に到達する状態である. したがって、φ(w)∈Aのとき、
 *         かつそのときに限り、Mはwを受理する. 関数φを
 *           φ(ε)  = q0
 *           φ(wa) = δ(φ(w),a), 任意のw∈Σ*, a∈Σに対して
 *         と、遷移関数を用いて再帰的に定義する
 *
 * @date   2016/03/31
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <string>
#include "finite_automaton.hpp"



//****************************************
// 関数の定義
//****************************************

/**
 * @brief COMPUTE-TRANSITION-FUNCTIONの補助手続きであり、入力パターンPとその長さm、状態qと入力文字aから
 *        次の状態kを出力する
 */
static state_t get_next_state(string_t P, strlen_t m, state_t q, chridx_t a)
{
    if (q < m && a == P[q]) {  // 文字aが継続してパターンに一致する場合、
        return q + 1;          // δ(q,a)=q+1だから、遷移はオートマトンの"背骨"に沿って進む
    }
    
    // 文字aが継続せずパターンに一致しない場合、Tiaの接尾語になるPのより短い接頭語を発見する必要がある
    for (state_t k = q; k > 0; k--) {

        if (P[k - 1] != a) { continue; }  // 後退辺を辿る

        strlen_t s;
        for (s = 0; s < k - 1; s++) {  // 接尾語になる接頭語を探してゆく
            if (P[s] != P[q - k + 1 + s]) { break; }
        }

        if (s == k - 1) {  // 接頭語を発見
            return k;      // 接尾語になる最大のkを返す
        }
    }
    return 0;
}


/**
 * @brief 次の手続きは、与えられたパターンP[0..m-1]から遷移関数δを計算する
 *
 * @note  この手続きは、δ(q, a)を定義式に従って直接的に計算する
 */
static delta_t compute_transition_function(string_t P)
{
    const chridx_t char_end = 256;

    strlen_t m = strlen(P);
    delta_t delta(m + 1, chrset_t(char_end));

    for(state_t q = 0; q <= m; q++) {                  // 入れ子のループによってすべての状態qと
        for (chridx_t a = 0; a < char_end; a++) {      // すべての文字aを考えており、
            delta[q][a] = get_next_state(P, m, q, a);  // δ(q,a)に遷移後の状態kを代入する
        }
    }
    return delta;
}


/**
 * @brief  入力テキストT[0..n-1]の中から長さmのパターンPの出現を発見するオートマトンの挙動を模倣する
 *
 * @note   長さmのパターンに対する任意の文字列照合オートマトンの状態集合Qは{0,1,..,m}、
 *         開始状態は0、そして、唯一の受理状態は状態mである
 *
 * @note   FINITE-AUTOMATON-MATCHERの単純なループ構造から、明らかに、長さnのテキスト文字列に対する照合時間はΘ(n)である
 *         しかし、この照合時間は遷移関数δの計算に必要な処理時間を含まない
 */
void finite_automaton_matcher(string_t T, const delta_t& delta, strlen_t m)
{
    strlen_t n = strlen(T);
    state_t  q = 0;

    for (strlen_t i = 0; i < n; i++) {
        q = delta[q][T[i]];
        if (q == m) {  // マシンが受理状態mにあることと、これがパターンPをちょうど走査し終わったこととは等価である
            std::cout << i - m + 1 << std::endl;
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

    delta_t delta = compute_transition_function(P.c_str());
    strlen_t m = strlen(P.c_str());

    finite_automaton_matcher(T.c_str(), delta, m);
    
    return 0;
}

