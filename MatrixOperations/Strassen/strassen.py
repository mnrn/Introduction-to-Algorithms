##
# @brief   行列積のためのStrassenのアルゴリズムを扱います
# 
# @date    2016/04/30
#


#********************************************************************************
# -*- coding: utf-8 -*-
#********************************************************************************



#********************************************************************************
# モジュールのインポート
#********************************************************************************

import numpy as np  # 数値計算のためのライブラリ



#********************************************************************************
# 単純な行列積の計算
#********************************************************************************

##
#  @brief  2つのnxn型行列AとBを入力として取り、これらの積であるnxn型行列Cを返す
#  @note   3重の入れ子になったfor文はそれぞれちょうどn回繰り返し、式(*1)の実行に
#          定数時間かかるので、SQUARE-MATRIX-MULTIPLY手続きの実行時間はΘ(n^3)である
#
def square_matrix_multiply(A, B) :
    n = len(A)  # n = A.rows
    C = [[0 for j in range(n)] for i in range(n)]  # Cを新しいnxn型行列とする

    for i in range(n):
        for j in range(n):
            # C[i][j] = 0
            for k in range(n):
                C[i][j] = C[i][j] + A[i][k] * B[k][j]  #...(*1)
    return C



##
# @brief   分割統治アルゴリズムを用いて2つのnxn型行列AとBの行列積C=A*Bを計算する
# @note    手続きSQUARE-MATRIX-MULTIPLY-RECURSIVEの実行時間はΘ(n^3)である
#
def square_matrix_multiply_recursive(A, B) :
    n = len(A)                       # n = A.rows
    C = np.zeros((n, n))             # Cを新しいnxn型行列とする
    
    if n == 1 :                      # n = 1のとき、再帰の基底であり、
        C[0][0] = A[0][0] * B[0][0]  # 1つのスカラー積を計算するだけ
    else :
        # n > 1のとき、添字計算を用いてΘ(1)時間でA, B, Cを分割する
        A11, A12 = A[   :n/2,    :n/2], A[   :n/2, n/2:  n] 
        A21, A22 = A[n/2:  n,    :n/2], A[n/2:  n, n/2:  n]

        B11, B12 = B[   :n/2,    :n/2], B[   :n/2, n/2:  n]
        B21, B22 = B[n/2:  n,    :n/2], B[n/2:  n, n/2:  n]

        # SQUARE-MATRIX-MULTIPLY-RECURSIVEを全体で8回再帰的に呼び出す
        C11 = square_matrix_multiply_recursive(A11, B11) \
            + square_matrix_multiply_recursive(A12, B21)
            
        C12 = square_matrix_multiply_recursive(A11, B12) \
            + square_matrix_multiply_recursive(A12, B22)
            
        C21 = square_matrix_multiply_recursive(A21, B11) \
            + square_matrix_multiply_recursive(A22, B21)

        C22 = square_matrix_multiply_recursive(A21, B12) \
            + square_matrix_multiply_recursive(A22, B22)

        C = np.r_[np.c_[C11, C12], np.c_[C21, C22]]
    return C



#********************************************************************************
# Strassenによる行列積計算アルゴリズム
#********************************************************************************

##
# @brief   Strassenによる行列積計算アルゴリズム
#
# @note    Strassenの方法のキーは再帰木の辺数を減らすことである. すなわち、8回あったn/2 x n/2型行列に対する
#          再帰的な行列乗算をStrassenの方法では7回で済まし、1回の行列乗算を定数回の新しいn/2 x n/2型行列に対する
#          加算によって置き換える. 行列加算の回数はそれが定数である限りΘ記法に埋もれてしまう
#
# @details Strassenの方法は決して自明ではない. Strassenの方法は4ステップから構成されている
#
#          1. 入力行列A, Bと出力行列Cをn/2 x n/2型部分行列に分割する. SQUARE-MATRIX-MULTIPLY-RECURSIVEで
#             行ったように、添字計算を用いると第1ステップをΘ(1)時間で実行できる
#
#          2. 10個のn/2 x n/2型行列S1, S2, ..., S10を生成する. これらは、第1ステップでAおよびBから作成された
#             8個の部分行列の中の2個の和または差である. これら10個の行列のすべてをΘ(n^2)時間で生成できる
#
#          3. 第1ステップで生成された行列と第2ステップで生成された10個の行列を用いて、7個の行列積P1, P2, ..., P7を
#             再帰的に計算する. 各行列Piはn/2 x n/2型である
#
#          4. 様々なPi行列の加減演算を用いて、結果となる行列Cの部分行列C11, C12, C21, C22を計算する
#             これら4個の部分行列のすべてをΘ(n^2)時間で計算する
#
# @note    Strassenの方法による計算時間はΘ(n^lg7)であり、素朴なSQUARE-MATRIX-MULTIPLY手続きよりも
#          漸近的に高速である
#
def strassen(A, B) :
    n = len(A)                       # n = A.rows
    C = np.zeros((n, n))             # Cを新たなnxn型行列とする
    if n == 1 :                      # n = 1のとき、再帰は基底であり、
        C[0][0] = A[0][0] * B[0][0]  # 1つのスカラー積を計算するだけ
    else :
        # Step 1 : nxn型行列をn/2 x n/2型部分行列に分割する
        A11, A12 = A[   :n/2,    :n/2], A[   :n/2, n/2:  n] 
        A21, A22 = A[n/2:  n,    :n/2], A[n/2:  n, n/2:  n]
        B11, B12 = B[   :n/2,    :n/2], B[   :n/2, n/2:  n]
        B21, B22 = B[n/2:  n,    :n/2], B[n/2:  n, n/2:  n]

        # Step 2 : 10個の行列S1, S2, ..., S10を生成する
        # n/2 x n/2型行列の10回の加減算を行うからこのステップの実行にΘ(n^2)時間かかる
        S1  = B12 - B22
        S2  = A11 + A12
        S3  = A21 + A22
        S4  = B21 - B11
        S5  = A11 + A22
        S6  = B11 + B22
        S7  = A12 - A22
        S8  = B21 + B22
        S9  = A11 - A21
        S10 = B11 + B12

        # Step 3 : n/2 x n/2型行列の乗算を再帰的に7回行い、7個のn/2 x n/2型行列P1, P2, ..., P7を計算する
        P1 = strassen(A11, S1)   # A11 * S1   =  A11 * B12 - A11 * B22
        P2 = strassen(S2, B22)   # S2  * B22  =  A11 * B22 + A12 * B22
        P3 = strassen(S3, B11)   # S3  * B11  =  A21 * B22 + A22 * B11
        P4 = strassen(A22, S4)   # A22 * S4   =  A22 * B21 - A22 * B11
        P5 = strassen(S5, S6)    # S5  * S6   =  A11 * B11 + A11 * B22 + A22 * B11 + A22 * B22 
        P6 = strassen(S7, S8)    # S7  * S8   =  A12 * B21 + A12 * B22 - A22 * B21 - A22 * B22
        P7 = strassen(S9, S10)   # S9  * S10  =  A11 * B11 + A11 * B12 - A21 * B11 - A21 * B12 

        # Step 4 : 第3ステップで生成したPi行列の加減算によって、積Cの4個のn/2 x n/2型部分行列を計算する
        # n/2 x n/2型部分行列の加減算を8回行うので全体でΘ(n^2)時間かかる
        C11 = P5 + P4 - P2 + P6  # = A11 * B11 + A12 * B21
        C12 = P1 + P2            # = A11 * B12 + A12 * B22
        C21 = P3 + P4            # = A21 * B11 + A22 * B21
        C22 = P5 + P1 - P3 - P7  # = A21 * B12 + A22 * B22

        C = np.r_[np.c_[C11, C12], np.c_[C21, C22]]
    return C


##
# @brief  Strassenによる行列積計算アルゴリズム
# @note   nが2のベキ乗でないときにも正しく働くようにnxn型行列の積を計算する
#         Strassenのアルゴリズムを修正したものである 
#
def modified_strassen(A, B) :
    n = len(A)
    a = np.zeros(n)
    b = np.zeros(n+1)
    order_odd = False

    if n == 1 :                   # n = 1のとき、再帰の基底であり、
        return A[0][0] * B[0][0]  # 1つのスカラー積を計算するだけ
    elif n & 0x01 :                                  # nが奇数のとき、
        A, B = np.c_[A, a],       np.c_[B, a]        # 行にパディングを施し、
        A, B = np.vstack([A, b]), np.vstack([B, b])  # 列にもパディングを施す
        order_odd = True
        n = len(A)
    
    # Step 1 : nxn型行列をn/2 x n/2型部分行列に分割する
    A11, A12 = A[   :n/2,    :n/2], A[   :n/2, n/2:  n] 
    A21, A22 = A[n/2:  n,    :n/2], A[n/2:  n, n/2:  n]
    B11, B12 = B[   :n/2,    :n/2], B[   :n/2, n/2:  n]
    B21, B22 = B[n/2:  n,    :n/2], B[n/2:  n, n/2:  n]

    # Step 2 : 10個の行列S1, S2, ..., S10を生成する
    # n/2 x n/2型行列の10回の加減算を行うからこのステップの実行にΘ(n^2)時間かかる
    S1  = B12 - B22
    S2  = A11 + A12
    S3  = A21 + A22
    S4  = B21 - B11
    S5  = A11 + A22
    S6  = B11 + B22    
    S7  = A12 - A22
    S8  = B21 + B22
    S9  = A11 - A21
    S10 = B11 + B12

    # Step 3 : n/2 x n/2型行列の乗算を再帰的に7回行い、7個のn/2 x n/2型行列P1, P2, ..., P7を計算する
    P1 = modified_strassen(A11, S1)   # A11 * S1   =  A11 * B12 - A11 * B22
    P2 = modified_strassen(S2, B22)   # S2  * B22  =  A11 * B22 + A12 * B22
    P3 = modified_strassen(S3, B11)   # S3  * B11  =  A21 * B22 + A22 * B11
    P4 = modified_strassen(A22, S4)   # A22 * S4   =  A22 * B21 - A22 * B11
    P5 = modified_strassen(S5, S6)    # S5  * S6   =  A11 * B11 + A11 * B22 + A22 * B11 + A22 * B22 
    P6 = modified_strassen(S7, S8)    # S7  * S8   =  A12 * B21 + A12 * B22 - A22 * B21 - A22 * B22
    P7 = modified_strassen(S9, S10)   # S9  * S10  =  A11 * B11 + A11 * B12 - A21 * B11 - A21 * B12 

    # Step 4 : 第3ステップで生成したPi行列の加減算によって、積Cの4個のn/2 x n/2型部分行列を計算する
    # n/2 x n/2型部分行列の加減算を8回行うので全体でΘ(n^2)時間かかる
    C11 = P5 + P4 - P2 + P6  # = A11 * B11 + A12 * B21
    C12 = P1 + P2            # = A11 * B12 + A12 * B22
    C21 = P3 + P4            # = A21 * B11 + A22 * B21
    C22 = P5 + P1 - P3 - P7  # = A21 * B12 + A22 * B22

    C = np.r_[np.c_[C11, C12], np.c_[C21, C22]]
    if order_odd : C = C[:n-1, :n-1]
    return C
    
        

#********************************************************************************
# エントリポイント
#********************************************************************************
    
if __name__ == '__main__' :
    mtx1 = [[2, 1], [5, 1]]
    mtx2 = [[1, 1], [1, 2]]
    print(square_matrix_multiply(mtx1, mtx2))

    mtx1 = np.array([[2, 1], [5, 1]])
    mtx2 = np.array([[1, 1], [1, 2]])
    print(square_matrix_multiply(mtx1, mtx2))

    mtx3 = [[2, 2, 1, 1], [1, 1, 2, 2], [1, 2, 1, 2], [2, 1, 2, 1]]
    mtx4 = [[1, 2, 2, 1], [2, 1, 1, 2], [1, 1, 1, 1], [2, 2, 2, 2]]
    print(square_matrix_multiply(mtx3, mtx4))
    mtx3 = np.array([[2, 2, 1, 1], [1, 1, 2, 2], [1, 2, 1, 2], [2, 1, 2, 1]])
    mtx4 = np.array([[1, 2, 2, 1], [2, 1, 1, 2], [1, 1, 1, 1], [2, 2, 2, 2]])
    print(square_matrix_multiply_recursive(mtx3, mtx4))
    print(strassen(mtx3, mtx4))

    mtx7 = np.arange(36).reshape(6, 6)
    mtx8 = np.arange(36).reshape(6, 6)
    print(modified_strassen(mtx7, mtx8))
