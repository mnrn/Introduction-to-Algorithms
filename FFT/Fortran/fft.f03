  !> @brief FFT(高速フーリエ変換)ついて扱う
  !! @date  2016/03/08



module fft_mod
  implicit none
  
  double precision, parameter ::  pi = atan(1.0) * 4.0
  
contains
  !> @brief  再帰型FFT(高速フーリエ変換)アルゴリズム
  !! @note   このアルゴリズムはn要素ベクトルa = (a0, a1, ..., a(n-1))のDFT(離散フーリエ変換)を計算する
  !!         ただし、nは2の冪乗であると仮定する
  recursive subroutine recursive_fft(a, b, n)
    implicit none

    integer, intent(in) :: n         ! nは2の冪乗を仮定する
    complex(kind(0d0)), intent(in)  :: a(0:n-1)
    complex(kind(0d0)), intent(out) :: b(0:n-1)
    
    complex(kind(0d0)) :: a_0_(0:n/2 - 1), a_1_(0:n/2 - 1)
    complex(kind(0d0)) :: omega_n, omega
    complex(kind(0d0)) :: y(0:n-1), y_0_(0:n/2 - 1), y_1_(0:n/2 - 1)

    integer :: k

    if (n == 1) then  ! 再帰の基底
       b = a          ! 1要素のDFTはy0 = a0 * ω1^0 = a0 * 1 = a0だから、それ自身である
       return
    end if

    omega_n = cmplx(cos(2.0 * pi / n), sin(2.0 * pi / n), kind(0d0))
    omega   = (1.0, 0.0)

    a_0_ = a(0:n-2:2)     ! a^[0] = (a0, a2, ..., a(n-2))
    a_1_ = a(1:n-1:2)     ! a^[1] = (a1, a3, ..., a(n-1))

    ! 再帰型DFT(n/2)計算を実行し、k = 0, 1, ..., n/2 - 1に対して、以下のように置く
    call recursive_fft(a_0_, y_0_, n/2)  ! yk^[0] = A^[0]((ω(n/2))^k) = A^[0]((ωn)^(2k))
    call recursive_fft(a_1_, y_1_, n/2)  ! yk^[1] = A^[1]((ω(n/2))^k) = A^[1]((ωn)^(2k))

    ! 再帰型DFT(n/2)の計算結果を結合する
    do k = 0, n/2 - 1
       ! バタフライ演算
       y(k)       = y_0_(k) + omega * y_1_(k)
       y(k + n/2) = y_0_(k) - omega * y_1_(k)
       ! ωの更新
       omega = omega * omega_n
    end do

    b = y
  end subroutine recursive_fft
end module fft_mod

program main
  use fft_mod
  implicit none
  complex(kind(0d0)), dimension(8) :: input(0:7)
  complex(kind(0d0)), dimension(8) :: output(0:7)
  integer i
  
  do i = 0, 7
     input(i) = cmplx(i, 0)
     write(*, '("(", F20.15, ",", F20.15, "i )")') input(i)
  end do

  call recursive_fft(input,output, 8)

  do i = 0, 7
     write(*, '("(", F20.15, ",", F20.15, "i )")') output(i)
  end do
end program main
  
