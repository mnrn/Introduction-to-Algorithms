/**
 * @brief  動的マルチスレッド化された手続きP-FIBの解析を行う
 * @date   2016/03/05
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef __cilk
#include <cilk/cilk_stub.h>
#endif
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>



//****************************************
// 関数の定義
//****************************************

 /**
  * @brief  動的マルチスレッド版フィボナッチ数計算アルゴリズム
  * @note   P-FIB(n)の並列度はT1(n)/T∞(n)=Θ(φ^n/n)であり、nが大きくなるにつれ急激に増加する
  * @param  n番目フィボナッチ数を求める
  * @return n版目のフィボナッチ数
  */
int fib(int n) {
	if (n <= 1) {  // nが1以下のとき、再帰は底をつく(基底部分)
		return n;  // 1を返す
	}
	else {
		int x = cilk_spawn fib(n - 1);  // 手続きが生成した子らを同時に実行「できる」と言っている
		int y = fib(n - 2);
		cilk_sync;                      // この手続きが生成した子がすべての計算を終了するまで次に進めない
		return x + y;
	}
}


int main()
{
	int n = 39;

	__cilkrts_set_param("nworkers", "4");

	clock_t start = clock();
	int result = fib(n);
	clock_t end = clock();

	double duration = (double)(end - start) / CLOCKS_PER_SEC;
	printf("Fib(39) = %d\n", result);
	printf("calc in %.3f seconds using %d workers", duration, __cilkrts_get_nworkers());

	getchar();
	return 0;
}


