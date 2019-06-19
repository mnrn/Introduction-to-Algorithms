/**
 * @brief 選択アルゴリズムのテストプログラム
 * @date  作成日     : 2016/02/03
 * @date  最終更新日 : 2016/02/03
 */



#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "selection.hpp"


#define N 16  // データの件数



int main(void)
{
    // 乱数の初期化
    srand((unsigned)time(NULL));

    int values[N];
    puts("データ準備:");
    for (int i = 0; i < N; i++) {
        values[i] = rand() % 1000;
        printf("%d ", values[i]);
    }

    puts("\n選択開始:");
    auto it = minimum(values, values + N);
    std::cout << "1番小さい数は..."<< *it << std::endl;

    puts("\n選択開始:");
    it = maximum(values, values + N);
    std::cout << "1番大きい数は..."<< *it << std::endl;

    puts("\n選択開始:");
    it = randselect(values, values + N, 2, std::greater<int>());
    std::cout << "2番目に大きい数は..."<< *it << std::endl;

    puts("\n選択開始:");
    std::pair<int*, int*> lhs = minmax(values, values + N);
    std::cout << "1番小さい数は..." << *(lhs.first)  << std::endl;
    std::cout << "1番大きい数は..." << *(lhs.second) << std::endl;
    

    return 0;
}
