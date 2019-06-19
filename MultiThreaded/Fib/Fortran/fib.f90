!> @brief 並列フィボナッチ計算
!!
!! @date 2016/03/08



module fibonacci
  implicit none
contains
  !> @brief 逐次版フィボナッチ計算
  recursive integer function fib(n) result(fibn)
    implicit none

    integer, intent(in) :: n
    integer :: x, y

    if (n <= 1) then
       fibn = 1
    else
       x = fib(n - 1)
       y = fib(n - 2)

       fibn = x + y
    end if
  end function fib

  !> @brief 並列版フィボナッチ計算
  recursive integer function p_fib(n) result(fibn)
    implicit none

    integer, intent(in) :: n
    integer :: x, y

    if (n <= 1) then
       fibn = 1
    else
       !$omp task shared(x) firstprivate(n)
       x = p_fib(n - 1)
       !$omp end task
       !$omp task shared(y) firstprivate(n)
       y = p_fib(n - 2)
       !$omp end task
       !$omp taskwait
       fibn = x + y
    end if
  end function p_fib
end module fibonacci




program main
  use fibonacci
  !$ use omp_lib
  implicit none
  integer, parameter :: n = 39
  integer :: result
  !$ double precision st, en

  !$ st = omp_get_wtime()
  
  !$omp parallel num_threads(4)
  !$omp single
  result =  p_fib(n)
  !$omp end single
  !$omp end parallel

  !$ en = omp_get_wtime()

  print *, "fib(", n, ")=", result
  !$ print *, "Elapsed time in second is:", en - st
end program main
