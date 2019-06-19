/**
 * @brief  剰余演算(modular arthmetic)に関するアルゴリズムを扱います
 *
 * @note   剰余演算とは、形式ばらずにいうと、モジュロnについて考えているとき、すべての結果xはnを法として
 *         xに等しい{0,1,...,n-1}の要素で置き換えられる(すなわち、xはx mod nで置き換えられる)ことを除いて、
 *         整数に関する普通通りの算術と考えることができる. 加算、減算、および乗算の演算に固執するならば、
 *         この略式モデルでも十分である. これから与える剰余演算に対する形式的なモデルは、群論の枠内で記述するのが最善である
 *
 * @date   2016/03/30
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "modular.hpp"
#include "../GCD/gcd.hpp"
#include <tuple>
#include <iostream> 



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  ユークリッドのアルゴリズム拡張版
 *
 * @note   手続きEXTENDED-EUCLIDは任意の整数の対を入力とし、式 d = gcd(a, b) = ax + byを満たす
 *         (d, x, y)という形式の3項組を返す
 *
 * @note   EUCLIDでの再帰呼び出しの回数はEXTENDED-EUCLIDでの再帰呼び出しの回数に等しいから、
 *         EUCLIDとEXTENDED-EUCLIDの実行時間は定数倍の範囲内で同じである
 *        すなわち、a > b > 0に対して、再帰呼び出しの回数はΟ(lgb)である
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
 * @brief  1次合同式ax ≡ b (mod n)の解をすべて出力する
 *
 * @note   入力aとnは任意の正整数でbは任意の整数である
 *
 * @note   EXTENDED-EUCLIDが算術演算をΟ(lgn)回実行し、for文の各繰り返しが算術演算を定数回実行するから、
 *         MODULAR-LINEAR-EQUATION-SOLVERは算術演算をΟ(lgn + gcd(a, n))回実行する
 */
intset_t modular_linear_equation_solver(int_t a, int_t b, int_t n)
{
    int_t d, x_, y_;
    std::tie(d, x_, y_) = extended_gcd(a, n);

    intset_t x;
    if (b % d == 0) {
        x_ = ((x_ % n) + n) % n;  // 剰余群は負の整数に対しても定義される
        int_t x0 = (x_ * (b / d)) % n;
        for (int_t i = 0; i < d; i++) {
            x.insert((x0 + i * (n / d)) % n);
        }
    }
    else { /* 解は存在しない */ }

    return x;
}


/**
 * @brief 反復2乗法(repeated squaring)はbの2進表現を用いてベキ乗剰余(modular exponentiation)を解く
 *
 * @note  aとbを非負整数, nを正整数とするとき、a^b mod nを効率良く計算する
 *
 * @note  次の手続きでは、2倍にする操作と1を加える操作を繰り返してcを0からbまで増加させるのにしたがって、
 *        対応するa^c mod nを計算する
 *
 * @note  入力a, b, nがβビットの数ならば、必要な算術演算の総数はΟ(β)であり、したがって、必要なビット演算の総数はΟ(β^3)である
 */
uint_t modular_exponentiation(uint_t a, uint_t b, uint_t n)
{
    uint_t d = 1;
    while (b > 0) {
        if (b & 0x01) {
            d = (d * a) % n;
        }
        a = (a * a) % n; 
        b >>= 1;
    }
    return d;
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;

    int_t a, b, n;
    a = 14; b = 30; n = 100;

    intset_t x = modular_linear_equation_solver(a, b, n);

    for (auto& xi : x) {
        cout << xi << endl;
    }
    
    return 0;
}
