/**
 * @brief  行列乗算のためのマルチスレッドアルゴリズム
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
// 関数の定義
//****************************************

/**
 * @brief  マルチスレッド行列乗算
 * @param  行列A
 * @param  行列B
 * @return 行列C = A * B
 */
mat_t p_square_matrix_multiply(const mat_t& A, const mat_t& B)
{
	int n = A.size();

	mat_t C(n, vec_t(n));

	cilk_for (int i = 0; i < n; i++) {
		cilk_for(int j = 0; j < n; j++) {
			C[i][j] = 0;
			for (int k = 0; k < n; k++) { 　// ここでparallel forを単純に用いると競合状態が発生する
				C[i][j] = C[i][j] + A[i][k] * B[k][j];
			}
		}
	}
	return C;
}


int main()
{
	std::random_device rd;
	std::mt19937 mt(rd());


	const int N = 8;

	mat_t A(N, vec_t(N));
	mat_t B(N, vec_t(N));

	std::cout << "A = [";
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			A[i][j] = 1;
			std::cout << A[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "]" << std::endl;

	std::cout << "B = [";
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			B[i][j] = mt() & 0x01;
			std::cout << B[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "]" << std::endl;


	__cilkrts_set_param("nworkers", "4");


	mat_t C = p_square_matrix_multiply(A, B);


	std::cout << "C = [";
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			std::cout << C[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "]" << std::endl;

	getchar();
	return 0;
}

