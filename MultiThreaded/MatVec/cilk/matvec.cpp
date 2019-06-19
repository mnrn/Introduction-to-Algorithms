/**
* @brief  nxn�^�s��A��n�x�N�g��x�̐ς����Ɍv�Z����
* @date   2016/03/06
*/



//****************************************
// �K�v�ȃw�b�_�t�@�C���̃C���N���[�h
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
// �^�V�m�j��
//****************************************

using elem_t = std::int32_t;
using vec_t  = std::vector<elem_t>;
using mat_t  = std::vector<vec_t>;



//****************************************
// �֐��̐錾
//****************************************

void matvec_mainloop(const mat_t& A, const vec_t& x, vec_t& y,
	int n, int i, int i_);



//****************************************
// �֐��̒�`
//****************************************

/**
 * @brief  nxn�^�s��A=(aij)��n�x�N�g��x=(xj)���|��������l����
 *
 * @note   ���ʂł���n�x�N�g��y=(yi)��i=0,1,...,n-1�ɑ΂���
 *           yi = ��[i = 0, n-1](aij)*(xj)
 *         �ƒ�`�����
 *
 * @note   �葱���S�̂ɑ΂���X�p���̓�(n)�ł���A�d���ʂ���(n^2)������A
 *         ����x�̓�(n^2)/��(n)=��(n)�ł���
 * @param  const mat_t& A nxn�^�s��A
 * @param  const vec_t& x n�x�N�g��x
 * @return vec_t y
 */
vec_t matvec(const mat_t& A, const vec_t& x)
{
	int n = A.size();  // n = A.rows

	vec_t y(n);        // y�𒷂�n�̐V���ȃx�N�g���Ƃ���


	// NOTE : ���[�v�̌J��Ԃ��̕�����s�͕��������@��p���čċA�I�ɌĂяo�����֐��ɕϊ������


	cilk_for(int i = 0; i < n; i++) {  // y�����ɏ�����
		y[i] = 0; 
	}

	// ���C�����[�v
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
