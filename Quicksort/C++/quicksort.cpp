/**
 * @brief クイックソートのテストプログラム
 * @date  作成日     : 2016/01/31
 * @date  最終更新日 : 2016/01/31
 */


#include "quicksort.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>




#define N 100  // データの件数



int main(void)
{
    // 乱数の初期化
    srand((unsigned)time(NULL));

    int sort[N];
    puts("ソート準備:");
    for (int i = 0; i < N; i++) {
        sort[i] = rand() % 1000;
        printf("%d ", sort[i]);
    }

    puts("\nソート開始:");
    introsort(sort, sort + N, std::greater<int>());
    puts("ソート終了:");

    for(int i = 0; i < N; i++) {
        printf("%d ", sort[i]);
    }

    putchar('\n');

    return 0;
}
