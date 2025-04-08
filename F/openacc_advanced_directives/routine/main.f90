program main
    implicit none
    integer :: a(20) = [4,1,5,2,3,2,2,3,1,2,4,1,1,1,2,5,2,3,5,1]
    integer :: b(20) = [5,5,2,3,2,3,1,4,1,1,3,5,3,2,4,2,2,3,2,3]
    integer :: c(20)
    integer :: i
    
    write(*,*) "array a:"
    write(*,"(20I3)") a
    write(*,*)
    write(*,*) "array b:"
    write(*,"(20I3)") b
    write(*,*)

    !$acc kernels copyin(a,b) copyout(c)
    !$acc loop independent
    do i = 1, 20
	    call func(a(i), b(i), c(i))
    enddo
    !$acc end kernels
    
    write(*,*) "array c:"
    write(*,"(20I3)") c

contains

subroutine func(a, b, c)
    implicit none
    integer, intent(in) :: a, b
    integer, intent(out) :: c
    !$acc routine seq
    
    c = a * b
    
end subroutine func

end program main
