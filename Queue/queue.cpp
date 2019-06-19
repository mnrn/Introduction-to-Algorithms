/**
 * @brief キューのテストプログラム
 *
 * @date  作成日    : 2016/02/11
 * @date  最終更新日 : 2016/02/11
 */




//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "../Quicksort/xorshift.hpp"
#include "queue.hpp"



//****************************************
// オブジェクト形式マクロの定義
//****************************************

#define N 20  // データの件数



//****************************************
// 関数の定義
//****************************************

int main(void)
{
    queue<int> q(N);
    int key[N];
    for (int i = 0; i < N; i++) {
        key[i] = xorshift128() % 1000;
        q.enqueue(key[i]);
        std::cout << key[i] << " ";
    }
    std::cout << std::endl;

    while (!q.empty()) {
        std::cout << q.dequeue() << " ";

	}
    std::cout << std::endl;
}

