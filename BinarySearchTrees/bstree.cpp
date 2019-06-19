/**
 * @brief 2分探索木のテストプログラム
 * @date  作成日     : 2016/02/06
 * @date  最終更新日 : 2016/02/06
 */



#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "BSTree.hpp"


#define N 20  // データの件数



int main(void)
{
    // 乱数の初期化
    srand((unsigned)time(NULL));

    BSTree<int, void*> t;
    BSTree<int, void*>::Node nodes[N];
    puts("挿入開始:");
    for (int i = 0; i < N; i++) {
        nodes[i].key = rand() % 1000;
        fprintf(stderr, "%d ", nodes[i].key);
        t.insert(&nodes[i]);
    }

    auto func = [](BSTree<int, void*>::Node*x) { printf("%d ", x->key); };
    puts("\n中間順木巡回開始:");
    t.inorder(t.root, func);
    puts("\n中間順木巡回終了:");

	int indexies[] = {5, 3, 6, 10, 15, 1, 19, 7, 8, 0 };
    for(int i = 0; i < 10; i++) {
		printf("erase %d\n", nodes[indexies[i]].key);
		t.erase(&nodes[indexies[i]]);
		t.inorder(t.root, func);
		putchar('\n');
    }

    

    return 0;
}
