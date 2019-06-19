!>
!! @brief マージソートのマルチスレッド化
!! @date   2016/03/11
!!



module p_mergesort_mod
  implicit none
  
  public  :: p_mergesort
  private :: p_merge, bsearch
  
contains
  !>
  !! @brief  2分探索を行う
  !!
  !! @note   手続きBINARY-SEARCH(x, T, p, r)はキーxと部分配列T[p..r]を入力として、
  !!         次のうちの1つを返す
  !!
  !!         ・T[p..r]が空(p < r)なら、添字pを返す
  !!         ・x <= T[p]であって、xがT[p..r]の任意の要素よりも大きくならないなら、添字pを返す
  !!         ・x > T[p]のとき、p < q <= r + 1の中で、T[q-1] < xを満たす最大の添字qを返す
  !!
  function bsearch(x, T, p, r)
    implicit none
    
    double precision, intent(in) :: x, T(:)
    integer, intent(in) :: p, r

    integer :: low, high, mid, bsearch

    low  = p
    high = max(p, r + 1)

    do while (low < high)
       mid = low + (high - low) / 2
       if (x <= T(mid)) then
          high = mid
       else
          low = mid + 1
       end if
    end do
    bsearch = high
  end function bsearch


  !>
  !! @brief  マルチスレッドマージ手続き
  !!
  !! @note   マルチスレッド化されたマージを導出するための分割統治戦略は、配列Tの部分配列上で動作する
  !!         長さn1 = r1 - p1 + 1のソート済み部分配列T[p1..r1]と長さn2 = r2 - p2 + 1のソート済み部分配列T[p2..r2]をマージして、
  !!         長さn3 = r3 - p3 + 1 = n1 + n2の部分配列A[p3..r3]に格納することを考える. 簡単化のために、一般性を失うことなくn1 >= n2を仮定する
  !!
  !! @note   まず、部分配列T[p1..r1]の真ん中の要素x = T[q1]を求める. ここで、q1 = floor((p1 + r1) / 2)である
  !!         この部分配列はソート済みだから、xはT[p1..r1]の中央値である. すなわち、T[p1..q1-1]のすべての要素はx以下だし、T[q1+1..r1]のすべての要素はx以上である
  !!         そして、2分探索を用いて、部分配列T[p2..r2]の中で、T[q2-1]とT[q2]の間にxを挿入しても部分配列が正しくソートされているような添字q2を発見する
  !!         次に、部分配列T[p1..r1]とT[p2..r2]をマージして、以下の要領でA[p3..r3]に格納する
  !!
  !!           1. q3 = p3 + (q1 - p1) + (q2 - p2)とする
  !!           2. xをA[q3]にコピーする
  !!           3. 再帰的にT[p1..q1-1]とT[p2..q2-1]をマージし、結果を部分配列A[p3..q3-1]に置く
  !!           4. 再帰的にT[q1+1..r1]とT[q2..r2]をマージし、その結果を部分配列A[q3+1..r3]に置く
  !!
  !!         q3の計算において、q1 - p1は部分配列T[p1..q1-1]の要素数、q2 - p2は部分配列T[p2..q2-1]の要素数だから、
  !!         その和は部分配列A[p3..r3]においてxより前にくる要素数である
  !!
  !! @note   基底はn1 = n2 = 0の場合である. このとき、2つの空の部分配列をマージするのに仕事をする必要はない
  !!         部分配列T[p1..r1]の長さはT[p2..r2]の長さ以上であると仮定した. すなわち、仮定n1 >= n2から、
  !!         部分配列T[p2..r2]だけが空である場合についても、再帰が正しく働くことを保証する必要がある
  !!
  !! @note   MERGEとは異なり、P-MERGEではマージする部分配列は配列の中で隣接していると仮定しない(すなわち、P-MERGEではp2 = r1 + 1とは限らない)
  !!         MERGEとP-MERGEの別の相違は、P-MERGEではマージした列を蓄えるための出力用部分配列Aを引数としてとることである
  !!         P-MERGE(T, p1, r1, p2, r2, A, p3)を呼び出すと、ソート済み部分配列T[p1..r1]とT[p2..r2]をマージして部分配列A[p3..r3]に格納する
  !!         ここで、r3 = p3 + (r1 - p1 + 1) + (r2 -p2 + 1) - 1 = p3 + (r1 - p1) + (r2 - p2) + 1であり、これは入力として与えられていない
  !!
  !! @note   n個の要素に対するP-MERGEのスパンPM∞(n) = Θ((lg^2)n)であり、最悪時の仕事量RM1(n) = Θ(n)であるから、
  !!         P-MERGEの並列度はPM1(n)/PM∞(n) = Θ(n/((lg^2)n))である
  !! 
  recursive subroutine p_merge(T, p1, r1, p2, r2, A, p3)
    implicit none
    
    double precision, intent(in)  :: T(:)
    double precision, intent(out) :: A(:)
    integer, intent(in) :: p1, r1, p2, r2, p3

    integer :: n1, n2, q1, q2, q3

    n1 = r1 - p1 + 1  ! 部分配列T[p1..r1]の長さn1と
    n2 = r2 - p2 + 1  ! T[p2..r2]の長さn2を求める

    if (n1 < n2) then       ! n1 >= n2を保証するため、(必要ならば)変数を置き換える 
       call p_merge(T, p2, r2, p1, r1, A, p3)
    ! 基底判定を行う
    !else if (n1 .eq. 0) then   ! 基底のとき(n1 = 0のとき)、
       !return  ! 部分配列T[p1..r1]もT[p2..r2]も空だから、何もせず、手続きから戻る
    else if (n1 .ne. 0) then  ! 分割統治戦略を実現する
       ! T[p1..r1]の中央値の要素を計算し、
       q1 = p1 + (r1 - p1) / 2
       ! T[p2..r2]の添字q2で、T[p2..q2-1]のすべての要素が(xに対応する)T[q1]以下、
       ! T[q2..r2]のすべての要素がT[q1]以上となるものを求める
       q2 = bsearch(T(q1), T, p2, r2)
       ! 出力用部分配列A[p3..r3]をA[p3..q3-1]とA[q3+1..r3]に分割する添字q3を計算し、
       q3 = p3 + (q1 - p1) + (q2 - p2)

       ! T[q1]を直接A[q3]にコピーする
       A(q3) = T(q1)
       
       !$omp parallel sections                                 ! つぎに、入れ子並列性を用いて再帰を行う
       !$omp section
       call p_merge(T, p1,     q1 - 1, p2, q2 - 1, A, p3)      ! 最初の部分問題を生成し、
       !$omp section
       call p_merge(T, q1 + 1, r1,     q2, r2,     A, q3 + 1)  ! 2番目の部分問題を並列に呼び出す
       !$omp end parallel sections
    end if
    ! NOTE : 部分配列T[p2..r2]が空のときにも正しく働くことを保証するために、多少の工夫が施されている. すなわち、
    !        各再帰呼び出しにおいて、T[p1..r1]が最終的に空になって基底処理に進むまで、この配列の中央値を出力用の配列の上に置く
  end subroutine p_merge


  !>
  !! @brief  マルチスレッドマージソート
  !!
  !! @note   このマージソートは引数として出力用部分配列Bを取り、そこにソート結果を格納する
  !!         特に、呼び出しP-MERGE-SORT(A, p, r, B, s)はA[p..r]の要素をソートし、結果をB[s..s + r - p]に格納する
  !!
  !! @note   P-MERGE-SORTの仕事量PMS1(n) = Θ(nlgn)であり、最悪時のスパンPMS∞(n) = Θ((lg^3)n)である
  !!         したがって、P-MERGE-SORTの並列度はPMS1(n) / PMS∞(n) = Θ(nlgn) / Θ((lg^3)n) = Θ(n / ((lg^2)n))であり、
  !!         理論上でも実際上でも格段に優れている
  !!
  recursive subroutine p_mergesort(A, p, r, B, s)
    implicit none
    
    double precision, intent(in)  :: A(:)
    double precision, intent(out) :: B(:)
    integer, intent(in) :: p, r, s

    double precision, allocatable :: T(:)
    integer :: q, q_, n

    n = r - p + 1      ! 入力配列A[p..r]の要素数nを計算した後、

    if (n == 1) then   ! 基底、すなわち、配列の要素が1個の要素しか持たない場合を扱う
       B(s) = A(p)
    else
       ! 並列に実行する再帰生成と再帰呼び出しの準備をする
       allocate(T(n))        ! 再帰的マージソートの結果を格納するn個の要素を持つ一時配列Tを割り当てる 
       q  = p + (r - p) / 2  ! A[p..r]の要素を2つの部分配列A[p..q]とA[q+1..r](これらは再帰的にソートされる)に分割する添字qを計算し、
       q_ = q - p + 1        ! これを用いて、部分配列A[p..q]の要素数q'を計算する

       !$omp parallel sections                   ! この時点で、
       !$omp section
       call p_mergesort(A, p, q, T, 1)           ! 手続き生成と
       !$omp section
       call p_mergesort(A, q + 1, r, T, q_ + 1)  ! 再帰呼び出しを実行する
       !$omp end parallel sections

       ! 最後に、ソート済み部分配列T[1..q']とT[q'+1..n]をマージし、
       call p_merge(T, 1, q_, q_ + 1, n, B, s)   ! その結果を出力用部分配列B[s..s + r - p]に格納するためにP-MERGEを呼び出す
    end if
  end subroutine p_mergesort
end module p_mergesort_mod


program main
  use p_mergesort_mod
  !$ use omp_lib
  implicit none

  double precision :: A(8) = (/5.0, 2.0, 4.0, 7.0, 1.0, 3.0, 2.0, 6.0/)
  double precision :: B(8)
  !$ double precision st, en

  call random_number(A)
  
  !$ st = omp_get_wtime()
  call p_mergesort(A, 1, 8, B, 1)
  !$ en = omp_get_wtime()

  !print *, B

  !$ print *, "Elapsed time in second is:", en - st
  
end program main




