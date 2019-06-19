!>
!! @brief マージソート(merge sort)を扱います
!! @date  20016/03/11
!!



module mergesort_mod
  implicit none

  public  :: mergesort, mergesort_
  private :: merge

contains

  !>
  !! @brief  2つの既ソート列のマージ(結合)を行う
  !!
  !! @note   n = r -p + 1をマージされる総要素数とすると、以下のように動作するMERGE手続きの実行時間はΘ(n)である
  !!
  !!         テーブル上に表を上にしたカードの山が2つあると想像しよう. それぞれの山は既にソートされていて、最小のカードが先頭に置かれている
  !!         この2つの山をマージして1つのソート済みの山を作りたい. ただし、今度は表を下にしてテーブルに置くものとする
  !!         この仕事を達成するために我々が行う基本操作は、表向きの山の先頭にあるカードのうちの小さいほうを選び、
  !!         それを山から取り除き(これによって新たなカードが先頭に現れる)、裏向きにして出力用の山の先頭に置くことである
  !!         この操作をどちらかの入力用の山が空になるまで繰り返し、その時点で残りの入力用の山を裏向きにして出力用の山に積み重なる
  !!         各基本操作は先頭の2枚のカードを比較するだけだから定数時間しかかからない
  !!         基本ステップは高々n回しか繰り返されないから、マージにかかる時間はΘ(n)である
  !!
  subroutine merge(A, p, q, r)
    implicit none
    double precision, intent(inout) :: A(:)
    integer, intent(in) :: p, q, r

    double precision, allocatable :: Left(:), Right(:)
    integer :: i, j, k, n1, n2

    ! 部分列A[p..q]の長さn1、部分列A[q+1..r]の長さn2を計算する
    n1 = q - p + 1    
    n2 = r - q

    ! 長さがそれぞれn1 + 1とn2 + 1の配列LとR(「左」と「右」を表す)を作る
    allocate(Left(n1 + 1))
    allocate(Right(n2 + 1))

    ! do文で部分列A[p..q]をL[1..n1]にコピーし、
    do i = 1, n1
       Left(i) = A(p + i - 1)
    end do
    ! do文で部分列A[q+1..r]をR[1..n2]にコピーする
    do j = 1, n2
       Right(j) = A(q + j)
    end do
    ! 配列LとRの最後に番兵を置く
    Left(n1 + 1) = huge(0.0d0)
    Right(n2 + 1) = huge(0.0d0)

    ! 以下のdo文の各繰り返しの開始時点では、部分列A[p..k-1]には、L[1..n1+1]とR[1..n2+1]が
    ! 含む要素全体の中で小さいほうからk - p個の要素がソートされた順序で置かれている
    ! また、L[i]とR[j]は配列LとRのまだAに書き戻されていない要素の中で最小の要素である

    ! 上記のループ不変式を維持しつつ、基本操作をr - p + 1回繰り返す

    ! ループの最初の繰り返しが開始される直前はk = pであり、部分列A[p..k-1]は空である
    i = 1  ! この部分列はLとRの小さいほうからk - p = 0個の要素を(ソートした順序で)含んでおり、
    j = 1  ! i = j = 1であるからL[i]とR[j]はともにAにまだ書き戻されていない最小の配列要素である
    do k = p, r
       if (Left(i) <= Right(j)) then  ! L[i] <= R[j]のとき、L[i]はAにまだ書き戻されていない要素の中で最小である. A[p..k-1]は小さいほうから
          A(k) = Left(i)  ! k - p個の要素を含んでいるのだから、L[i]をA[k]にコピーした後は、部分配列A[p..k]は小さいほうからk - p + 1個の要素を含む
          i = i + 1    ! (do文の更新で)kの値を1増やし、iの値を1増やすと、次の繰り返しの開始時点でループ不変式は再び真となる
       else                          ! 逆に、L[i] > R[j]が成立する場合には、
          A(k) = Right(j)  ! 左記のelse文による操作がループ不変式を維持する
          j = j + 1
       end if
    end do
    ! k = r + 1が成立したときにループは停止する. ループ不変式から、部分列A[p..k-1]、すなわちA[p..r]には
    ! L[1..n1+1]とR[1..n2+1]に含まれる全要素の中で小さいほうからk - p = r - p + 1個の要素がソートされた順序で置かれている
    ! 一方、配列LとRは全部でn1 + n2 + 2 = r - p + 3個の要素を含む. したがって、2個の最大要素を除くすべての要素がAに既に書き戻されていて、
    ! 残された2つの最大要素は番兵である
    
  end subroutine merge

  !>
  !! @brief 手続きMERGE-SORT(A, p, r)は部分列A[p..r]に格納された要素をソートする
  !!
  !! @note  MERGE-SORTの最悪実行時間はΘ(nlgn)である
  !!
  recursive subroutine mergesort(A, p, r)
    implicit none
    double precision, intent(inout) :: A(:)
    integer, intent(in) :: p, r
    integer :: q

    ! p >= rならば、部分列Aは高々1個の要素鹿含まないのでソート済みである

    if (p < r) then
       ! p < rならば、分割段階はA[p..r]を2つの部分配列、ceil(n/2)個の要素を含むA[p..q]と
       ! floor(n/2)個の要素を含むA[q+1..r]に分割するための添字qを計算する
       q = p + (r - p) / 2
       call mergesort(A, p, q)
       call mergesort(A, q+1, r)
       call merge(A, p, q, r)
    end if
  end subroutine mergesort

  recursive subroutine mergesort_(A, p, r)
    implicit none
    double precision, intent(inout) :: A(:)
    integer, intent(in) :: p, r
    integer :: q

    ! p >= rならば、部分列Aは高々1個の要素鹿含まないのでソート済みである

    if (p < r) then
       ! p < rならば、分割段階はA[p..r]を2つの部分配列、ceil(n/2)個の要素を含むA[p..q]と
       ! floor(n/2)個の要素を含むA[q+1..r]に分割するための添字qを計算する
       q = p + (r - p) / 2

       !$omp parallel sections
       !$omp section
       call mergesort(A, p, q)
       !$omp section
       call mergesort(A, q+1, r)
       !$omp end parallel sections
       call merge(A, p, q, r)
    end if
  end subroutine mergesort_
end module mergesort_mod



program main
  use mergesort_mod
  !$ use omp_lib
  implicit none

  double precision :: A(10000)
  !$ double precision st, en

  call random_number(A)

  !$ st = omp_get_wtime()
  call mergesort(A, 1, 10000)
  !$ en = omp_get_wtime()

  !$ print *, "Elapsed time in second is:", en - st

  call random_number(A)

  !$ st = omp_get_wtime()
  call mergesort_(A, 1, 10000)
  !$ en = omp_get_wtime()

  !$ print *, "Elapsed time in second is:", en - st
  
end program main

