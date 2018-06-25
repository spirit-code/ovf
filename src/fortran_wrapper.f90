module ovf
    use, intrinsic :: iso_c_binding
    implicit none

    type, bind(c) :: ovf_file
      logical(c_bool)   :: found
      logical(c_bool)   :: is_ovf
      integer(c_int)    :: n_segments
      type(c_ptr)       :: file_handle
    end type ovf_file
end module ovf
    
program main
  use, intrinsic :: iso_c_binding
  use ovf
    implicit none
    type(ovf_file)               :: file_handle

    interface
      function ovf_open(filename) bind ( C, name = "ovf_open" ) result(handle)
      use, intrinsic :: iso_c_binding
      use ovf
        character(len=1, kind=c_bool), intent(in) :: filename
        type(ovf_file)                            :: handle
      end function ovf_open
    end interface
  
    file_handle = ovf_open("testfile.ovf"//C_NULL_CHAR)
end program main
  
  
  