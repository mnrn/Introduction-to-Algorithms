/**
 * @brief 動的計画法の例としてロッド切出し問題(rod-cutting problem)を扱います
 *
 * @note  ロッド切出し問題を以下で定義する
 *        長さがnインチの1本の金属棒と、i = 1, 2, ..., nに対する価格piの表が与えられたとき、
 *        この金属棒から切り出される金属棒を価格表に従って販売して得ることができる収入の最大値を計算せよ
 *        長さnインチの金属棒の価格pnが十分に大きいならば、一度も切断しないことが最適解となる可能性があることに注意せよ
 *
 * @note  一般に、n >= 1に対する収入rnを、n未満の長さの金属棒に対する最適収入を用いて
 *        rn = max(pn, r1 + r(n-1), r2 + r(n-2), ..., r(n-1) + r1)
 *        と表現できる. 最初の引数pnは切断を1度も行わず、長さnの金属棒をそのまま販売することに対応する
 *        maxの残りのn-1個の引数は、各i = 1, 2, ..., n-1に対して、長さnの金属棒を最初に長さがiとn-iの2つの金属棒に切断し、
 *        つぎにそれぞれの金属棒を最適に分割することで得られる総収入、すなわちriとr(n-i)の和ri + r(n-i)を最大化することに対応する
 *        収入を最大化するiの値を事前にすることができないから、可能性があるすべてのiの値を考慮して、最大化を達成するiを選択する必要がある
 *        さらに、金属棒を切断せず、まるごと販売することで最大の収入を得ることができるなら、どのiも選択しないという選択肢もある
 *
 * @date  2016/03/16
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <vector>
#include <algorithm>



//****************************************
// 型シノニム
//****************************************

using value_t  = std::int32_t;          // ロッドの価格
using values_t = std::vector<value_t>;  // ロッドの価格の集合



//****************************************
// 関数の宣言
//****************************************

static value_t memoized_cut_rod_aux(const values_t& p, std::size_t n, values_t& r);



//****************************************
// 関数の定義
//****************************************

/**
 * @brief 価格の配列の配列p[1..n]と整数nを入力として取り、
 *        長さnの金属棒から得ることのできる最大の収入を返す
 */
value_t cut_rod(const values_t& p, std::size_t n)
{
    if (n == 0) { return 0; }  // n = 0のときは収入を得ることはできないので0を返す

    value_t q = std::numeric_limits<value_t>::min();  // 最大収入qを-∞に初期化

    for (std::size_t i = 1; i <= n; i++) {
        q = std::max(q, p[i] + cut_rod(p, n - i));
    }

    return q;
}


/**
 * @brief 履歴管理を用いるトップダウン型CUT-ROD手続き
 */
value_t memoized_cut_rod(const values_t& p, std::size_t n)
{
    // r[0..n]を新しい配列とする
    values_t r(n+1);

    // 新しい補助配列r[0..n]の各要素の値を、「未知」であることを示すのに適当な値である-∞に初期化する(既知の収入は常に非負である)
    for (std::size_t i = 0; i <= n; i++) {
        r[i] = std::numeric_limits<value_t>::min();
    }

    // 補助手続きMEMOIZED-CUT-ROT-AUXを呼び出す
    return memoized_cut_rod_aux(p, n, r);
}


/**
 * @brief MEMOIZED-CUT-RODの補助手続き
 */
value_t memoized_cut_rod_aux(const values_t& p, std::size_t n, values_t& r)
{
    if (r[n] >= 0) { return r[n]; }  // 求めている値が既知か否かを調べ、既知ならば、その値を返す

    // 未知ならば、求めている値qを計算し、
    value_t q;
    if (n == 0) {
        q = 0;
    }
    else {
        q = std::numeric_limits<value_t>::min();
        for (std::size_t i = 1; i <= n; i++) {
            q = std::max(q, p[i] + memoized_cut_rod_aux(p, n - i, r));
        }
    }
    
    r[n] = q;  // 保存し、
    return q;  // 返す
}


/**
 * @brief ボトムアップ型動的計画アルゴリズム
 *
 * @note  BUTTOM-UP-CUT-RODは部分問題の自然な順序を用いる
 *        すなわち、i < jならば、サイズiの部分問題はサイズjの部分問題よりも「小さい」と定義する
 *        したがって、手続きはサイズがj = 0, 1, ..., nの部分問題をこの順序で解く
 */
value_t bottom_up_cut_rod(const values_t& p, std::size_t n)
{
    values_t r(n + 1);  // 部分問題の解を保存するための新しい配列r[0..n]を生成

    r[0] = 0;  // 長さが0の金属棒から得ることができる収入は0だから、r[0]を0に初期化する

    // j = 1, 2, ..., nに対して、この順番でサイズjの部分問題を解く
    for (std::size_t j = 1; j <= n; j++) {
        value_t q = std::numeric_limits<value_t>::min();

        for (std::size_t i = 1; i <= j; i++) {
            // サイズj - iの部分問題を解くために再帰呼び出しを行う代わりに、直接に配列要素r[j - i]を参照する
            q = std::max(q, p[i] + r[j - i]);
        }
        r[j] = q;  // サイズjの部分問題に対する解をr[j]に保存する
    }
    return r[n];  // r[n]を返す. この値は最適値rnに等しい
}


/**
 * @brief 各金属棒のサイズjに対して、最大収入rjとともにrjを達成するために左端から切り取る金属棒の長さsjを計算する
 * @note  この手続きは、配列sを生成し、サイズjの部分問題を最適に解くときに左端から切り出す金属棒の最適な長さiを保存するために
 *        s[j]を更新することを覗くとBOTTOM-UP-CUT-RODと同じである
 */
std::pair<values_t, values_t> extended_bottom_up_cut_rod(const values_t& p, std::size_t n)
{
    values_t r(n + 1), s(n + 1);  // r[0..n]とs[0..n]を新しい配列とする

    r[0] = 0;

    for (std::size_t j = 1; j <= n; j++) {
        value_t q = std::numeric_limits<value_t>::min();

        for (std::size_t i = 1; i <= j; i++) {
            if (q < p[i] + r[j - i]) {
                q = p[i] + r[j - i];
                s[j] = i;
            }
        }
        r[j] = q;
    }
    return std::make_pair(r, s);
}


/**
 * @brief 価格表pと金属棒のサイズnを入力として取り、EXTENDED-BOTTOM-UP-CUT-RODを呼び出して
 *        最適な左端からの切り出しサイズを保存する配列s[1..n]を計算し、長さnの金属に対する最適な切出し方を印刷する
 */
void print_cut_rod_solution(const values_t& p, std::size_t n)
{
    values_t r(n + 1), s(n + 1);

    std::tie(r, s) = extended_bottom_up_cut_rod(p, n);

    std::cout << "n = " << n << "のとき、最大収入は"  << r[n] << "であり、切出し方は";
    
    while (n > 0) {
        std::cout << s[n] << " ";
        n = n - s[n];
    }
    std::cout << std::endl;
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    //                    長さ    1   2   3   4   5   6   7   8   9   10
    values_t value_table = { 0,  1,  5,  8,  9,  10, 17, 17, 20, 24, 30, };  // 価格

    for (int i = 1; i <= 10; i++) {
        print_cut_rod_solution(value_table, i);
    }
    
    return 0;
}

