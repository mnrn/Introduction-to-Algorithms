/**
 * @brief 関数型っぽいかんじにする
 * @date  作成日     : 2016/02/12
 * @date  最終更新日 : 2016/02/12
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "func.hpp"
#include <iostream>




//****************************************
// 関数の定義
//****************************************

int add(int x, int y)
{
    return x + y;
}


int main(void)
{
    auto add1c = curry(add)(100);
    std::cout << add1c(200) << "\n";

    auto add1p = partial(add, 20);
    std::cout << add1p(-5) << "\n";

    return 0;
}


