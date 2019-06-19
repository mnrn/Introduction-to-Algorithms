  !> @brief Fortranによるstackの実装例
  !! @date 2016/03/07

module module_stack
  implicit none

  !> @brief スタック
  type :: stack
     integer :: top = 0
     integer, allocatable :: stack(:)
     
   contains
     procedure :: push   ! INSERT操作
     procedure :: pop    ! DELETE操作
     procedure :: empty  ! 質問操作(スタックが空かどうか調べる)
     final :: destructor
  end type stack

  interface stack ! 引数仕様宣言
     module procedure const
  end interface stack

contains
  !> @brief コンストラクタ
  type(stack) function const(n) result(this)
    implicit none
    integer, intent(in) :: n
    allocate(this%stack(n))
  end function const

  !> @brief デストラクタ
  subroutine destructor(this)
    implicit none
    type(stack), intent(inout) :: this
    print *, "called destuctor"
    deallocate(this%stack)
  end subroutine destructor

  subroutine push(S, x)
    implicit none
    class(stack), intent(inout) :: S
    integer, intent(in) :: x

    S%top = S%top + 1
    S%stack(S%top) = x
  end subroutine push

  integer function pop(S)
    implicit none
    class(stack), intent(inout) :: S
    S%top = S%top - 1
    pop = S%stack(S%top + 1)
  end function pop

  logical function empty(S)
    implicit none
    class(stack), intent(in) :: S
    if (S%top == 0) then
       empty = .true.
    else
       empty = .false.
    end if
  end function empty
  
    
end module module_stack



program main
  use module_stack
  implicit none
  type(stack), pointer :: stack_

  allocate(stack_)
  stack_ = stack(10)

  call stack_%push(5)
  call stack_%push(1)
  call stack_%push(2)

  do
     if (stack_%empty()) exit
     print *, stack_%pop()
  end do

  deallocate(stack_)
end program main
