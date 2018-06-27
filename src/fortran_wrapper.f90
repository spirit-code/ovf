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
    integer(kind=c_int)    :: a
    type(c_ptr)  :: title
    integer(kind=c_int)           :: valuedim
    type(c_ptr) :: valueunits
    type(c_ptr) :: valuelabels

    type(c_ptr) :: meshtype
    type(c_ptr) :: meshunits

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
    type(c_ptr)                      :: c_file, c_segment
    type(ovf_file), pointer          :: fortran_handle
    type(ovf_segment)                :: f_segment
    integer(kind=c_int)              :: succ

    interface
    function ovf_open(filename) bind ( C, name = "ovf_open" ) 
    use, intrinsic :: iso_c_binding
        character(len=1,kind=c_char)       :: filename(*)
        type(c_ptr)                        :: ovf_open
    end function ovf_open
    end interface

    interface
    function ovf_read_segment_header(file, index, segment)&
                                    bind ( C, name = "ovf_read_segment_header" ) &
                                    result(succ)
    use, intrinsic :: iso_c_binding
    use ovf
        type(c_ptr), value              :: file
        integer(kind=c_int), value      :: index 
        type(ovf_segment)               :: segment
        integer(kind=c_int)             :: succ
    end function ovf_read_segment_header
    end interface

    c_file = ovf_open(C_CHAR_"testfile.ovf"//C_NULL_CHAR)
    !call C_F_POINTER(c_file, fortran_handle)

    write (*,"(Z20)") "Fortran pointer = ", c_file
    succ = ovf_read_segment_header(c_file, 1, f_segment)

    !call C_F_POINTER(c_segment, f_segment)

    write (*,*) "a= ", f_segment%a
    write (*,*) "n_cells = ", f_segment%n_cells

end program main


