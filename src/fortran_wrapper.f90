module ovf
use, intrinsic :: iso_c_binding
implicit none

type, bind(c) :: ovf_file
    type(c_ptr)       :: filename
    integer(c_int)    :: found
    integer(c_int)    :: is_ovf
    integer(c_int)    :: n_segments
    type(c_ptr)       :: file_handle
end type ovf_file

type, bind(c) :: ovf_segment
    type(c_ptr)  :: title
    integer(kind=c_int)           :: valuedim
    type(c_ptr) :: valueunits
    type(c_ptr) :: valuelabels

    type(c_ptr) :: meshtype
    type(c_ptr) :: meshunits

    integer(kind=c_int)           :: pointcount

    integer(kind=c_int)           :: n_cells(3)
    integer(kind=c_int)           :: N

    real(kind=c_float)            :: bounds_min(3)
    real(kind=c_float)            :: bounds_max(3)

    real(kind=c_float)            :: lattice_constant
    real(kind=c_float)            :: bravais_vectors(3,3)
end type ovf_segment

contains
    function get_string(c_pointer) result(f_string)
        implicit none
        type(c_ptr), intent(in)                 :: c_pointer
        character(len=:), pointer               :: f_ptr
        character(len=:), allocatable           :: f_string
        integer(c_size_t)                       :: l_str

        interface
            function c_strlen(str_ptr) bind ( C, name = "strlen" ) result(len)
            use, intrinsic :: iso_c_binding
                type(c_ptr), value              :: str_ptr
                integer(kind=c_size_t)          :: len
            end function c_strlen
        end interface

        call c_f_pointer(c_pointer, f_ptr )
        l_str = c_strlen(c_pointer)

        f_string = f_ptr(1:l_str)
    end function get_string


end module ovf
    
program main
use, intrinsic :: iso_c_binding
use ovf
    implicit none
    type(c_ptr)                       :: c_file, c_segment
    type(ovf_file), pointer           :: fortran_handle
    type(ovf_segment)                 :: f_segment
    integer(kind=c_int)               :: success
    integer                           :: size
    real(kind=4), allocatable, target :: array_4(:,:)
    real(kind=8), allocatable, target :: array_8(:,:)
    character(len=:), allocatable     :: test_str


    interface
        function ovf_open(filename) &
                        bind ( C, name = "ovf_open" ) 
        use, intrinsic :: iso_c_binding
            character(len=1,kind=c_char)       :: filename(*)
            type(c_ptr)                        :: ovf_open
        end function ovf_open
    end interface

    interface
        function ovf_read_segment_header(file, index, segment) &
                                        bind ( C, name = "ovf_read_segment_header" ) &
                                        result(success)
        use, intrinsic :: iso_c_binding
        use ovf
            type(c_ptr), value              :: file
            integer(kind=c_int), value      :: index 
            type(ovf_segment)               :: segment
            integer(kind=c_int)             :: success
        end function ovf_read_segment_header
    end interface

    interface
        function ovf_read_segment_data_4(file, index, segment, array) &
                                        bind ( C, name = "ovf_read_segment_data_4" ) &
                                        result(success)
        use, intrinsic :: iso_c_binding
        use ovf
            type(c_ptr), value              :: file
            integer(kind=c_int), value      :: index
            type(ovf_segment)               :: segment
            type(c_ptr), value              :: array
            integer(kind=c_int)             :: success
        end function ovf_read_segment_data_4
    end interface

    interface
        function ovf_read_segment_data_8(file, index, segment, array) &
                                        bind ( C, name = "ovf_read_segment_data_8" ) &
                                        result(success)
        use, intrinsic :: iso_c_binding
        use ovf
            type(c_ptr), value              :: file
            integer(kind=c_int), value      :: index
            type(ovf_segment)               :: segment
            type(c_ptr), value              :: array
            integer(kind=c_int)             :: success
        end function ovf_read_segment_data_8
    end interface

    interface
        function ovf_close(file) &
                            bind ( C, name = "ovf_close" ) &
                            result(success)
        use, intrinsic :: iso_c_binding
            type(c_ptr), value              :: file
            integer(kind=c_int)             :: success
        end function ovf_close
    end interface

    !---------------------

    c_file = ovf_open(C_CHAR_"python/test/testfile_out.ovf"//C_NULL_CHAR)
    write (*,"(A, Z20)") "Fortran pointer = ", c_file

    call C_F_POINTER(c_file, fortran_handle)
    write (*,*) "n_segments = ", fortran_handle%n_segments

    !---------------------

    if (ovf_read_segment_header(c_file, 1, f_segment) == -1) then
        write (*,*) "n_cells =    ", f_segment%n_cells
        write (*,*) "n_total =    ", product(f_segment%n_cells)

        test_str = get_string(f_segment%valueunits)
        write (*,*) "test_str =   ", test_str
    else
        write (*,*) "something did not work with ovf_read_segment_header"
    end if

    !---------------------

    size = product(f_segment%n_cells)
    allocate(array_4(3,size))
    allocate(array_8(3,size))

    !---------------------

    if (ovf_read_segment_data_4(c_file, 0, f_segment, c_loc(array_4(1,1))) == -1) then
        write (*,*) "array_4(:,2) = ", array_4(:,2)
    else
        write (*,*) "something did not work with ovf_read_segment_data_4"
    end if

    if (ovf_read_segment_data_8(c_file, 0, f_segment, c_loc(array_8(1,1))) == -1) then
        write (*,*) "array_8(:,2) = ", array_8(:,2)
    else
        write (*,*) "something did not work with ovf_read_segment_data_8"
    end if

    !---------------------

    if (ovf_read_segment_data_4(c_file, 1, f_segment, c_loc(array_4(1,1))) == -1) then
        write (*,*) "array_4(:,2) = ", array_4(:,2)
    else
        write (*,*) "something did not work with ovf_read_segment_data_4"
    end if

    !---------------------

    success = ovf_close(c_file)

end program main


