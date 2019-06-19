
/**
 * @brief  マルチスレッドアルゴリズムにおける決定性競合(determinacy race)の例を発生させる
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


void RaceExample()
{
	int x = 0;
	cilk_for(int i = 0; i < 2; i++) {
		x = x + 1;
	}
	printf("%d\n", x);
}

int main()
{
	RaceExample();
	getchar();

	return 0;
}
