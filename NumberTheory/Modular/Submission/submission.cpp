/**
 * @brief  剰余演算(modular arthmetic)に関するアルゴリズムを扱います
 *
 * @note   剰余演算とは、形式ばらずにいうと、モジュロnについて考えているとき、すべての結果xはnを法として
 *         xに等しい{0,1,...,n-1}の要素で置き換えられる(すなわち、xはx mod nで置き換えられる)ことを除いて、
 *         整数に関する普通通りの算術と考えることができる. 加算、減算、および乗算の演算に固執するならば、
 *         この略式モデルでも十分である. これから与える剰余演算に対する形式的なモデルは、群論の枠内で記述するのが最善である
 *
 * @note   関連URL: http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=NTL_1_B
 *
 * @date   2016/03/30
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <tuple>
#include <iostream>
#include <cstdint> 
#include <set>



//****************************************
// 型シノニム
//****************************************

using int_t    = std::int64_t;
using uint_t   = std::uint64_t;
using bit_t    = uint_t;
using intset_t = std::set<int_t>;


//****************************************
// 関数の定義
//****************************************

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

    const int_t n = 1000000007;
    int_t a, b;

    cin >> a >> b;
    cout << modular_exponentiation(a, b, n) << endl;
    
    return 0;
}
