!> @brief 挿入ソートの記述例
!! @date  2016/03/10



!> @brief 挿入ソートモジュール
module insertion_sort_mod
  implicit none
  
  public :: insertion_sort

contains
  !>
  !! @brief  挿入ソートを行う
  !!
  !! @note   長さnの列を格納する配列A[1..n]を引数として受け取る
  !!         アルゴリズムは入力された数列をその場でソート(sorted in place)する
  !!         すなわち、数は配列Aの中で再配置され、どの時点でも、A以外の場所に記憶される要素は高々定数でしかない
  !!         INSERTION-SORTが終了したとき、入力配列Aにソート済みの出力列が格納されている
  !!
  !! @note   挿入ソートは最悪実行時間Θ(n^2)をもつ
  !!
  !! @param[inout] A 入力配列A[1..n]
  !!
  subroutine insertion_sort(A)
    double precision, intent(inout) :: A(:)

    double precision :: key
    integer :: i, j, n

    n = size(A)  ! n = A.length

    ! do文の各繰り返しが開始されるときには、部分配列A[1..j-1]には
    ! 開始時点でA[0..j-1]に格納されていた要素がソートされた状態で格納されている

    ! ループが最初の繰り返し(j=2の繰り返し)の直前では、部分配列A[1..j-1]は唯一の要素A[1]から構成され、
    ! これは、実際、もともとA[1]に格納されていた要素である. さらに、この部分配列は(もちろん、明らかに)
    ! ソート済みだから、最初の繰り返し直前においてループ不変式は真である
    do j = 2, n
       ! do文の本体が行っていることはA{j]を入れるべき場所が見つかるまでA[j-1],A[j-2],A[j-3],...を
       ! それぞれ1つ右に移し空いた場所にA[j]の値を挿入することである
       key = A(j)
       i = j - 1 
       do while (i > 0 .and. A(i) > key)
          A(i + 1) = A(i)
          i = i - 1
       end do
       A(i + 1) = key
    end do
    ! do文が停止するのはj > A.length = nを満たすときである. ループ不変式のjにn + 1を1だけ増加させるから、
    ! 停止時に j = n + 1が成立する. ループ不変式のjにn + 1を代入すると、部分配列A[1..n]には、
    ! 開始時点でA[1..n]が配列全体で格納されていた要素が格納されているが、これらの要素はすでにソートされている
    ! 部分配列A[1..n]が配列全体であることに注意すると、配列全体がソート済みであると結論できる
    
  end subroutine insertion_sort
end module insertion_sort_mod


program main
  use insertion_sort_mod
  implicit none

  double precision :: A(6) = (/5.0, 2.0, 4.0, 6.0, 1.0, 3.0/)

  print *, A

  call insertion_sort(A)

  print *, A
  
end program main


