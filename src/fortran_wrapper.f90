module ovf
  use, intrinsic :: iso_c_binding
  implicit none

  type, bind(c) :: ovf_file
    integer(c_int)    :: found
    integer(c_int)    :: is_ovf
    integer(c_int)    :: n_segments
    type(c_ptr)       :: file_handle
  end type ovf_file

  type, bind(c) :: ovf_segment
    character(kind=c_char, len=1), allocatable :: title(:)
    integer(kind=c_int)           :: valuedim
    character(kind=c_char, len=1) :: valueunits(300)
    character(kind=c_char, len=1) :: valuelabels(300)

    character(kind=c_char, len=1) :: meshtype(300)
    character(kind=c_char, len=1) :: meshunits(300)

    integer(kind=c_int)           :: n_cells(3)
    integer(kind=c_int)           :: N

    real(kind=c_float)            :: bounds_min(3)
    real(kind=c_float)            :: bounds_max(3)

    real(kind=c_float)            :: lattice_constant
    real(kind=c_float)            :: bravais_vectors(3,3)
  end type ovf_segment
    

end module ovf
    
program main
  use, intrinsic :: iso_c_binding
  use ovf
    implicit none
    type(ovf_file)                :: file_handle
    type(ovf_segment)             :: segment 
    integer(kind=c_int)           :: worked
    character(len=1, kind=c_char) :: filename(30)

    interface
      function ovf_open(filename, handle) bind ( C, name = "ovf_open" )
      use, intrinsic :: iso_c_binding
      use ovf
        character(kind=c_char)       :: filename(*)
        type(ovf_file)                      :: handle
        integer(kind=c_int)           :: ovf_open
      end function ovf_open
    end interface


    file_handle%found = 0
    file_handle%is_ovf = 1
    file_handle%n_segments = -8

    write (*,*) ovf_open(C_CHAR_"testfile.ovf"//C_NULL_CHAR, file_handle)

    write (*,*) "found = ", file_handle%found
    write (*,*) "is_ovf =", file_handle%is_ovf
    write (*,*) "number of segments = ", file_handle%n_segments

    !worked = ovf_read_segment_header(file_handle, 0, segment)
    
    !write (*,*) "worked = ", worked

end program main
  
  
  