
!> @brief Fortranによるクイックソートの記述例
!!
!! @date 2016/03/07




!> @brief クイックソート用モジュール
module qsort
  implicit none
  
  public  :: qs
  private :: part, swap

contains

  !> @brief クイックソート
  !! @note  要素数nの入力配列上でのクイックソートアルゴリズムの最悪実行時間はΘ(n^2)である
  !!        ただし、期待実行時間はΘ(nlgn)であり、表記に隠されている定数部分は極めて小さい
  recursive subroutine qs(A, p, r)
    real, intent(inout), dimension(:) :: A
    integer, intent(in) :: p, r
    integer q

    if (p < r) then
       ! 分割: 配列A[p..r]を2つの(空の可能性もある)部分配列A[p..q-1]と
       ! A[q+1..r]にA[p..q-1]のどの要素もA[q]以下となり、A[q+1..r]の
       ! どの要素もA[q]以上となるように分割(再配置)する
       q = part(A, p, r)

       ! 統治: 2つの部分配列A[p..q-1]とA[q+1..r]をクイックソートを
       ! 再帰的に呼び出すことでソートする
       call qs(A, p, q - 1)
       call qs(A, q + 1, r)
    end if
  end subroutine qs



  !> @brief 要素xを常にピボットとして選択し、部分配列A[p..r]をピボットより大きい要素の集合と小さい要素の集合に2分割する
  !! @note  部分配列Aの要素数をnとするとき、実行時間はΘ(n)で表される
  integer function part(A, p, r)
    real, intent(inout), dimension(:) :: A
    integer, intent(in)  :: p, r

    integer i, j
    real x

    x = A(r)  ! 要素x=A[r]をピボットとして選ぶ
    i = p - 1

    ! for文の各繰り返しの直前では、任意の配列添字kに対して以下の命題(ループ不変式)が成立する
    ! 1. p <= k < iならば、A[k] <= xである
    ! 2. i + 1 <= k <= j - 1ならば、A[k] > xである
    ! 3. k = rならば、A[k] = xである
    
    do j = p, r - 1
       ! A[j] > xの場合
       ! ループはjに1を加えるだけである. jに1を加えるとA[j - 1]に対して条件2が成り立ち、他の部分は変化しない

       ! A[j] <= xの場合
       if (A(j) <= x) then
          ! この場合、iに1を加え、A[i]とA[j]を交換し、jに1を加える. 交換によってA[i] <= xが成立するので、条件1が成立する
          ! 同様に交換によってA[j - 1]に格納した要素がxより大きいことをループ不変式が保証するのでA[j - 1] > xが成立する
          i = i + 1
          call swap(A(i), A(j))
       end if
    end do

    ! 終了時にはj = rである. したがって、配列のすべての要素は不変式に示された3つの集合のいずれかに属しており、
    ! 配列内のすべての値はx以下の値からなる集合、xより大きい値からなる集合、xのみからなる集合に分割された

    ! ピボットxより大きい要素の中で最左のものをピボットと交換することによって、ピボットをその正しい位置である配列の中央に移す
    call swap(A(i + 1), A(r))

    ! そしてピボットの新しい位置を返す
    part = i + 1 
    return
  end function part
  
  

  subroutine swap(x, y)
    real, intent(inout) :: x, y
    real tmp

    tmp = x
    x = y
    y = tmp
  end subroutine swap
end module qsort


program main
  use qsort
  implicit none

  integer, parameter :: r = 32
  real array(r)

  call random_number(array)
  
  print *, "array  = ", array
  call qs(array, 1, r)
  print *, "sorted = ", array
end program main


