program main
  implicit none
  
  integer, parameter :: n = 1000
  real(4), allocatable :: a(:), b(:)
  real(4) :: c
  integer :: i
  real(8) :: sum

  allocate(a(n),b(n))
  c = 2.0

!$acc kernels
  do i = 1, n
     a(i) = 10.0
  end do
!$acc end kernels

!$acc kernels
  do i = 1, n
     b(i) = a(i) + c
  end do
!$acc end kernels

  sum = 0.d0
  do i = 1, n
     sum = sum + b(i)
  end do

  write(*,*) sum/n
  deallocate(a,b)
  
end program main

