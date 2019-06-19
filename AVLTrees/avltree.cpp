/**
 * @brief AVL木のテストプログラム
 * @date  作成日     : 2016/02/07
 * @date  最終更新日 : 2016/02/07
 */



#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "avltree.hpp"



#define N 50  // データの件数



int main(void)
{
    // 乱数の初期化
    srand((unsigned)time(NULL));

    avltree<int, void*> t(N);
    int keys[N];
    puts("挿入開始:");
    for (int i = 0; i < N; i++) {
        int key = rand() % 1000;
        fprintf(stderr, "%d ", key);
        t.insert(key, nullptr);
        keys[i] = key;
    }

    auto func = [](int x) { printf("%d ", x); };
    puts("\n中間順木巡回開始:");
    t.inorder(func);
    puts("\n中間順木巡回終了:");

    for (int i = 0; i < N; i++) {
        int key = keys[i];
        printf("\nerase.. %d\n", key);
        t.erase(key);
        t.inorder(func);
    }
    putchar('\n');

    return 0;
}
