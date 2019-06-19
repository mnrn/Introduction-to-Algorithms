/**
 * @brief 最大公約数について扱います
 * @date  2016/03/30
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "gcd.hpp"
#include <iostream>



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  ユークリッドのアルゴリズム(以下、EUCLIDとする)を用いて2つの整数の最大公約数を効率よく計算する
 *
 * @note   EUCLIDはGCD再帰定理に基づいて再帰コードとして書かれている
 *         ただし、入力aとbは任意の非負整数である
 *
 * @note   GCD再帰定理...任意の非負整数aと任意の正整数bに対して次式が成り立つ
 *           gcd(a, b) = gcd(b, a mod b)
 *
 * @note   EUCLIDの全体の実行時間は再帰呼び出しの実行回数に比例する
 *         漸化式 F(k+2) = F(k+1) + Fkによって定義されるフィボナッチ数Fkを利用すると、以下の補題が成り立つ
 *
 *         (補題) a > b >= 0であり、しかもEUCLID(a, b)の呼び出しがk >= 1回の再帰呼び出しを実行するなら、
 *               a >= F(k+2)および b >= F(k + 1)が成り立つ
 *
 *         補題から、次に示す定理が成り立つ
 *         (Lameの定理) 任意の整数k >= 1に対して、a > b >= 0かつb < F(k+1)ならば、
 *                     EUCLID(a, b)の実行では再帰呼び出しの回数はkより少ない
 *
 *         Lameの定理の上界はそれ以上改善できない(best possible)ことを示すことができる
 *         連続するフィボナッチ数がEUCLIDに対する最悪の場合の入力である. EUCLID(F3, F2)は1回だけ
 *         再帰呼び出しをすることと、k >= 2に対してF(k+1) mod Fk = F(k-1)であることから、次式を得る
 *           gcd(F(k+1), Fk) = gcd(Fk, (F(k+1) mod Fk))
 *                           = gcd(Fk, F(k-1))
 *         したがって、EUCLID(F(k+1), Fk)はちょうどk-1回だけ再帰を繰り返すから、Lameの定理の上界に一致する
 *
 * @note   Fkは近似的に(φ^k)/√5(φは黄金比(1+√5)/2)であるから、EUCLIDにおける再帰呼び出しの回数はΟ(lgb)である
 *         実際には、もっとタイトに1+logφ(b/gcd(a,b))に改善できる
 *
 * @param  int_t a   非負整数a
 * @param  int_t b   非負整数b
 * @return a mod b  aとbの最大公約数
 */
int_t euclid_gcd(int_t a, int_t b)
{
    if (b == 0) {
        return a;
    }
    else {
        return euclid_gcd(b, a % b);
    }
}



/**
 * @brief  ユークリッドのアルゴリズム拡張版
 *
 * @note   手続きEXTENDED-EUCLIDは任意の整数の対を入力とし、式 d = gcd(a, b) = ax + byを満たす
 *         (d, x, y)という形式の3項組を返す
 *
 * @note   EUCLIDでの再帰呼び出しの回数はEXTENDED-EUCLIDでの再帰呼び出しの回数に等しいから、
 *         EUCLIDとEXTENDED-EUCLIDの実行時間は定数倍の範囲内で同じである
 *         すなわち、a > b > 0に対して、再帰呼び出しの回数はΟ(lgb)である
 */
gcdtrpl_t extended_gcd(int_t a, int_t b)
{
    if (b == 0) {
        return std::make_tuple(a, 1, 0);
    }
    else {
        int_t d_, x_, y_, d, x, y;
        std::tie(d_, x_, y_) = extended_gcd(b, a % b);
        d = d_; x = y_; y = x_ - (a / b) * y_;
        return std::make_tuple(d, x, y);
    }
}


/**
 * @brief  2つの非負整数a, bの最小公倍数を求めます
 *
 * @param  int_t a 非負整数a
 * @param  int_t b 非負整数b
 * @return a * b / gcd(a, b)
 */
int_t lcm(int_t a, int_t b)
{
    return a * b / euclid_gcd(a, b);
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;
    
    int_t a, b, x, y, d;
    cin >> a >> b;


    tie(d, x, y) = extended_gcd(a, b);

    cout << x << " " << y << endl;

    return 0;
}
