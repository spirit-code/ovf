module ovf
  use, intrinsic :: iso_c_binding
  implicit none

  type, bind(c) :: ovf_file
    logical(c_bool)   :: found
    logical(c_bool)   :: is_ovf
    integer(c_int)    :: n_segments
    type(c_ptr)       :: file_handle
  end type ovf_file

  type, bind(c) :: ovf_segment
    character(kind=c_char, len=1) :: title(300)
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
    type(ovf_file)               :: file_handle
    type(ovf_segment)            :: segment 
    integer(kind=c_int)          :: worked

    interface
      function ovf_open(filename) bind ( C, name = "ovf_open" ) result(handle)
      use, intrinsic :: iso_c_binding
      use ovf
        character(len=1, kind=c_bool), intent(in) :: filename
        type(ovf_file)                            :: handle
      end function ovf_open
    end interface

    interface
      function ovf_read_segment_header(file, index, segment) &
                      bind (C, name = "ovf_read_segment_header") &
                      result(succ)
        use, intrinsic :: iso_c_binding
        use ovf
        type(ovf_file), intent(in)       :: file
        integer(kind=c_int), intent(in)  :: index 
        type(ovf_segment)                :: segment
        integer(kind=c_int)              :: succ 
      end function ovf_read_segment_header
    end interface
    
    file_handle = ovf_open("testfile.ovf"//C_NULL_CHAR)

    write (*,*) "found = ", file_handle%found
    write (*,*) "number of segments = ", file_handle%n_segments

    worked = ovf_read_segment_header(file_handle, 0, segment)
    
    write (*,*) "worked = ", worked

end program main
  
  
  