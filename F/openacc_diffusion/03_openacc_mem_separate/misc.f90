module misc
  use omp_lib
  implicit none
  real(8) :: t_s

contains

  subroutine swap(f, fn)
    real(4), pointer, intent(inout) :: f(:,:,:), fn(:,:,:)
    real(4), pointer :: ftmp(:,:,:)

    ftmp => f
    f => fn
    fn => ftmp
  end subroutine swap

  subroutine start_timer()
    t_s = omp_get_wtime()
  end subroutine start_timer

  real(8) function get_elapsed_time()
    get_elapsed_time = omp_get_wtime() - t_s
  end function get_elapsed_time

end module misc
