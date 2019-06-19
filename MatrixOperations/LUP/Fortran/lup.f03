!> @brief LUP分解について扱う
!! @date  2016/03/09



!> @brief LUP分解モジュール
module lup_mod
  implicit none

  public  :: lup_solve, lup_decomposition
  private :: iswap, dswap
  
contains
  !>
  !! @brief  与えられたP, L, Uおよびbに対して、手続きLUP-SOLVEは前進代入と後進代入を組み合わせて解xを得る
  !! @note   ただし、置換行列Pは配列πで表現される
  !!
  !! @note   手続きの実行時間はΘ(n^2)
  !!
  !! @param  double A(:)   aij = { lij (i > j)、 uij (i <= j) }を満たすnxn型行列A
  !! @param  integer pi(:) 置換行列Pの配列表現(i=1,2,..,nに対してpi(i)はP(i,pi(i))=1でj!=pi(i)ならばPij=0)
  !! @param  double b(:)   入力ベクトルb
  !! @param  integer n     次元数n
  !! @return double x(:)   Lux = Pbを満たすようなx
  !!
  function lup_solve(A, pi, b, n)
    implicit none
    
    double precision, intent(in) :: A(:,:), b(:)
    integer, intent(in) :: pi(:), n
    double precision :: x(n), y(n), sigma, lup_solve(n)
    
    integer :: i, j

    x = 0.0
    y = 0.0

    do i = 1, n                           ! 前進代入(forward substitution)を行って、yを計算する
       sigma = 0.0
       do j = 1, i - 1
          sigma = sigma + A(i, j) * y(j)  ! Σ[j=1, i-1](lij * yj)を計算
       end do
       y(i) = b(pi(i)) - sigma;           ! y1,y2,...,y(i-1)をi番目の式に"前の方から順々に"代入することでyiの値を得る
    end do

    do i = n, 1, -1                       ! 後進代入(back substitution)を行って、xを計算する
       sigma = 0.0
       do j = i + 1, n 
          sigma = sigma + A(i, j) * x(j)  ! Σ[j=i+1, n](ui * xj)を計算
       end do
       x(i) = (y(i) - sigma) / A(i, i)    ! この方法は前進代入法と似ているが、対角成分で要素を割る必要がある
    end do

    lup_solve = x
    return
  end function lup_solve



  !>
  !! @brief  正則行列AのLUP分解を行う
  !! @note   置換行列Pは配列πとして動的に管理する. ここでπ[i] = jはPの第i行は第j列に1を持つことを意味する
  !!         さらに、A上でLとUを「その場で」計算する. したがって、手続きが終了したときは
  !!           aij = { lij i >  jのとき
  !!                   uij i <= jのとき }
  !!         が成立する
  !!
  !! @note   LUP-DECOMPOSITIONの実行時間はΘ(n^3)である
  !!
  !! @param[inout] mat_t& A  nxn型の正則行列A
  !! @return indices_t pi    置換行列Pの配列表現
  !!
  function lup_decomposition(A)
    implicit none

    double precision, intent(inout) :: A(:,:)

    integer, allocatable :: pi(:), lup_decomposition(:)
    integer :: i, j, k, n, k_
    double precision :: p
    
    
    n = size(A, 1)            ! n = A.rows
    allocate(pi(n))           ! π[1..n]を新しい配列とする
    pi = (/ (i, i = 1, n) /)  ! π[1..n]の初期化
    do k = 1, n
       ! 0による除算および数値的不安定性を避けるため、できるだけ大きい数をピボットとして選択する
       p = 0.0
       k_ = 0
       do i = k, n
          if (abs(A(i, k)) > abs(p)) then
             p = A(i, k)
             k_ = i
          end if
       end do
       
       ! assert(p != 0.0, "特異行列")

       ! 要素ak'kをピボットの位置(k, k)に移すために、
       call iswap(pi(k), pi(k_))         ! π[k]とπ[k']を交換し、
       do i = 1, n                       ! 行全体を入れ換えるために、
          call dswap(A(k, i), A(k_, i))  ! akiとak'iを交換する
       end do

       ! Schurの補行列を計算する
       do i = k + 1, n
          A(i, k) = A(i, k) / A(k, k)
          do j = k + 1, n
             A(i, j) = A(i, j) - A(i, k) * A(k, j)
          end do
       end do
    end do   

    allocate(lup_decomposition(n))
    lup_decomposition = pi
  end function lup_decomposition


  
  subroutine iswap(x, y)
    implicit none
    integer, intent(inout) :: x, y
    integer :: tmp

    tmp = x
    x = y
    y = tmp
  end subroutine iswap

  subroutine dswap(x, y)
    implicit none
    double precision, intent(inout) :: x, y
    double precision :: tmp

    tmp = x
    x = y
    y = tmp
  end subroutine dswap
end module lup_mod


program main
  use lup_mod

  implicit none

  double precision :: b(3) = (/3.0, 7.0, 8.0/)
  integer :: pi(3) = (/3, 1, 2/)

  double precision :: A(3,3) = reshape( &
       (/5.0, 0.2, 0.6, 6.0, 0.8, 0.5, 3.0, -0.6, 2.5/), (/3,3/))

  double precision :: C(4,4) = reshape( &
       (/2.0,  3.0, 5.0, -1.0, &
         0.0,  3.0, 5.0, -2.0, &
         2.0,  4.0, 4.0,  3.4, &
         0.6, -2.0, 2.0, -1.0/), (/4,4/))

  integer :: pi2(4)

  print *, lup_solve(A, pi, b, 3)

  pi2 = lup_decomposition(C)

  print *, C

  print *, pi2

  
end program main

  
