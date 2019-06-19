/**
* @brief  nxn型行列Aとnベクトルxの積を並列に計算する
* @date   2016/03/06
*/



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#ifndef __cilk
#include <cilk/cilk_stub.h>
#endif
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cstdint>
#include <vector>
#include <iostream>
#include <random>



//****************************************
// 型シノニム
//****************************************

using elem_t = std::int32_t;
using vec_t  = std::vector<elem_t>;
using mat_t  = std::vector<vec_t>;



//****************************************
// 関数の宣言
//****************************************

void matvec_mainloop(const mat_t& A, const vec_t& x, vec_t& y,
	int n, int i, int i_);



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  nxn型行列A=(aij)にnベクトルx=(xj)を掛ける問題を考える
 *
 * @note   結果であるnベクトルy=(yi)はi=0,1,...,n-1に対して
 *           yi = Σ[i = 0, n-1](aij)*(xj)
 *         と定義される
 *
 * @note   手続き全体に対するスパンはΘ(n)であり、仕事量がΘ(n^2)だから、
 *         並列度はΘ(n^2)/Θ(n)=Θ(n)である
 * @param  const mat_t& A nxn型行列A
 * @param  const vec_t& x nベクトルx
 * @return vec_t y
 */
vec_t matvec(const mat_t& A, const vec_t& x)
{
	int n = A.size();  // n = A.rows

	vec_t y(n);        // yを長さnの新たなベクトルとする


	// NOTE : ループの繰り返しの並列実行は分割統治法を用いて再帰的に呼び出される関数に変換される


	cilk_for(int i = 0; i < n; i++) {  // yを並列に初期化
		y[i] = 0; 
	}

	// メインループ
	//cilk_for(int i = 0; i < n; i++) {
	//	for (int j = 0; j < n; j++) {
	//		y[i] = y[i] + A[i][j] * x[j];
	//	}
	//}
	matvec_mainloop(A, x, y, n, 0, n - 1);

	return y;
}



int main()
{
	std::random_device rd;
	std::mt19937 mt(rd());

	const int N = 8;

	vec_t x(N);
	mat_t A(N, vec_t(N));

	std::cout << "A = [";
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			A[i][j] = mt() & 0x01;
			std::cout << A[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "]" << std::endl;

	std::cout << "x = [";
	for (int i = 0; i < N; i++) {
		x[i] = 1;
		std::cout << x[i] << " ";
	}
	std::cout << "]" << std::endl;

	__cilkrts_set_param("nworkers", "4");

	vec_t y = matvec(A, x);


	std::cout << "y = [";
	for (int i = 0; i < N; i++) {
		std::cout << y[i] << " ";
	}
	std::cout << "]" << std::endl;

	getchar();

	return 0;
}


void matvec_mainloop(const mat_t& A, const vec_t& x, vec_t& y, int n, int i, int i_)
{
	if (i == i_) {
		for (int j = 0; j < n; j++) {
			y[i] = y[i] + A[i][j] * x[j];
		}
	}
	else {
		int mid = (i + i_) / 2;
		cilk_spawn matvec_mainloop(A, x, y, n, i, mid);
		matvec_mainloop(A, x, y, n, mid + 1, i_);
		cilk_sync;
	}
}
