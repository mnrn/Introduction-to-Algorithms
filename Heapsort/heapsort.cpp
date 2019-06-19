/**
 * @brief 2分ヒープのテストプログラム
 * @date  作成日    : 2016/01/27
 * @date  最終更新日 : 2016/01/27
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "heapsort.hpp"



//****************************************
// オブジェクト形式マクロの定義
//****************************************

#define N 256  // データの件数



//****************************************
// 関数の定義
//****************************************

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
    hsort(sort, N, std::less<int>());
    puts("ソート終了:");

    for(int i = 0; i < N; i++) {
        printf("%d ", sort[i]);
    }

    putchar('\n');

    return 0;
}

