program main
    implicit none
    
    integer :: data(20) = [0,1,5,2,3,2,2,3,1,0,4,1,1,1,2,5,2,3,0,1]
    integer :: histogram(6)
    integer :: i, j
    
    histogram(:) = 0
    
    write(*,*) "input data:"
    write(*,*) data
    write(*,*)

    !$acc kernels copyin(data) copyout(histogram)
    !$acc loop independent
    do j = 1, 6
        !$acc loop independent
	    do i = 1, 20
	        if(data(i) == j) then
                !$acc atomic 
		        histogram(j) = histogram(j) + 1
		    endif
	    enddo
	enddo
	!$acc end kernels

    write(*,*) "Count numbers"
    do j = 1, 6
        write(*,*) "Number of (", j, "): ", histogram(j)
    enddo
end program main
