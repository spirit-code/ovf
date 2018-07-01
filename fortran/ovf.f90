module ovf
use, intrinsic :: iso_c_binding
implicit none

integer, parameter :: OVF_OK      = -1
integer, parameter :: OVF_ERROR   = -2
integer, parameter :: OVF_INVALID = -3

integer, parameter :: OVF_FORMAT_BIN  = -55
integer, parameter :: OVF_FORMAT_TEXT = -56
integer, parameter :: OVF_FORMAT_CSV  = -57

type, bind(c) :: c_ovf_file
    type(c_ptr)       :: filename
    integer(c_int)    :: found
    integer(c_int)    :: is_ovf
    integer(c_int)    :: n_segments
    type(c_ptr)       :: file_handle
end type c_ovf_file

type, bind(c) :: c_ovf_segment
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
end type c_ovf_segment

type :: ovf_segment 
    character(len=:), allocatable :: Title, ValueUnits, ValueLabels,  MeshType, MeshUnits
    integer                       :: ValueDim, PointCount, N_Cells(3), N 
    real(8)                       :: bounds_min(3), bounds_max(3), lattice_constant, bravais_vectors(3,3)
end type ovf_segment 

type :: ovf_file
    character(len=:), allocatable   :: filename
    logical                         :: found, is_ovf
    integer                         :: n_segments
    type(c_ptr)                     :: file_handle
contains
    procedure :: load => load_ovf_file
end type ovf_file

contains

    function get_string(c_pointer) result(f_string)
        use, intrinsic :: iso_c_binding
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

    subroutine load_ovf_file(self, filename)
        implicit none
        class(ovf_file)               :: self
        character(len=*), intent(in)  :: filename
        type(c_ovf_file), pointer       :: f_handle

        interface
            function ovf_open(filename) &
                            bind ( C, name = "ovf_open" ) 
            use, intrinsic :: iso_c_binding
                character(len=1,kind=c_char)       :: filename(*)
                type(c_ptr)                        :: ovf_open
            end function ovf_open
        end interface

        call c_f_pointer(ovf_open(trim(filename) // c_null_char), f_handle)

        self%filename    = get_string(f_handle%filename)
        self%found       = f_handle%found  == 1
        self%is_ovf      = f_handle%is_ovf == 1
        self%n_segments  = f_handle%n_segments
        self%file_handle = f_handle%file_handle
    end subroutine load_ovf_file


end module ovf

    
! program main
! use, intrinsic :: iso_c_binding
! use ovf
!     implicit none
!     type(c_ptr)                       :: c_file, c_segment
!     type(ovf_file), pointer           :: fortran_handle
!     type(ovf_segment)                 :: f_segment
!     integer(kind=c_int)               :: success
!     integer                           :: size
!     real(kind=4), allocatable, target :: array_4(:,:)
!     real(kind=8), allocatable, target :: array_8(:,:)
!     character(len=:), allocatable     :: test_str




!     interface
!         function ovf_read_segment_header(file, index, segment) &
!                                         bind ( C, name = "ovf_read_segment_header" ) &
!                                         result(success)
!         use, intrinsic :: iso_c_binding
!         use ovf
!             type(c_ptr), value              :: file
!             integer(kind=c_int), value      :: index 
!             type(c_ovf_segment)               :: segment
!             integer(kind=c_int)             :: success
!         end function ovf_read_segment_header
!     end interface

!     interface
!         function ovf_read_segment_data_4(file, index, segment, array) &
!                                         bind ( C, name = "ovf_read_segment_data_4" ) &
!                                         result(success)
!         use, intrinsic :: iso_c_binding
!         use ovf
!             type(c_ptr), value              :: file
!             integer(kind=c_int), value      :: index
!             type(c_ovf_segment)               :: segment
!             type(c_ptr), value              :: array
!             integer(kind=c_int)             :: success
!         end function ovf_read_segment_data_4
!     end interface

!     interface
!         function ovf_read_segment_data_8(file, index, segment, array) &
!                                         bind ( C, name = "ovf_read_segment_data_8" ) &
!                                         result(success)
!         use, intrinsic :: iso_c_binding
!         use ovf
!             type(c_ptr), value              :: file
!             integer(kind=c_int), value      :: index
!             type(c_ovf_segment)               :: segment
!             type(c_ptr), value              :: array
!             integer(kind=c_int)             :: success
!         end function ovf_read_segment_data_8
!     end interface

!     interface
!         function ovf_write_segment_4(file, segment, array, fileformat) &
!                                     bind ( C, name = "ovf_write_segment_4" ) &
!                                     result(success)
!         use, intrinsic :: iso_c_binding
!         use ovf
!             type(c_ptr), value              :: file
!             type(c_ovf_segment)               :: segment
!             type(c_ptr), value              :: array
!             integer(kind=c_int), value      :: fileformat
!             integer(kind=c_int)             :: success
!         end function ovf_write_segment_4
!     end interface

!     interface
!         function ovf_write_segment_8(file, segment, array, fileformat) &
!                                     bind ( C, name = "ovf_write_segment_8" ) &
!                                     result(success)
!         use, intrinsic :: iso_c_binding
!         use ovf
!             type(c_ptr), value              :: file
!             type(c_ovf_segment)               :: segment
!             type(c_ptr), value              :: array
!             integer(kind=c_int), value      :: fileformat
!             integer(kind=c_int)             :: success
!         end function ovf_write_segment_8
!     end interface

!     interface
!         function ovf_append_segment_4(file, segment, array, fileformat) &
!                                     bind ( C, name = "ovf_append_segment_4" ) &
!                                     result(success)
!         use, intrinsic :: iso_c_binding
!         use ovf
!             type(c_ptr), value              :: file
!             type(c_ovf_segment)               :: segment
!             type(c_ptr), value              :: array
!             integer(kind=c_int), value      :: fileformat
!             integer(kind=c_int)             :: success
!         end function ovf_append_segment_4
!     end interface

!     interface
!         function ovf_append_segment_8(file, segment, array, fileformat) &
!                                     bind ( C, name = "ovf_append_segment_8" ) &
!                                     result(success)
!         use, intrinsic :: iso_c_binding
!         use ovf
!             type(c_ptr), value              :: file
!             type(c_ovf_segment)               :: segment
!             type(c_ptr), value              :: array
!             integer(kind=c_int), value      :: fileformat
!             integer(kind=c_int)             :: success
!         end function ovf_append_segment_8
!     end interface

!     interface
!         function ovf_close(file) &
!                             bind ( C, name = "ovf_close" ) &
!                             result(success)
!         use, intrinsic :: iso_c_binding
!             type(c_ptr), value              :: file
!             integer(kind=c_int)             :: success
!         end function ovf_close
!     end interface

!     write (*,*) "Running"

!     !---------------------

!     ! c_file = ovf_open(C_CHAR_"python/test/testfile_out.ovf"//C_NULL_CHAR)
!     ! write (*,"(A, Z20)") "Fortran pointer = ", c_file

!     ! call C_F_POINTER(c_file, fortran_handle)
!     ! write (*,*) "n_segments = ", fortran_handle%n_segments

!     ! !---------------------

!     ! if (ovf_read_segment_header(c_file, 1, f_segment) == -1) then
!     !     write (*,*) "n_cells =    ", f_segment%n_cells
!     !     write (*,*) "n_total =    ", product(f_segment%n_cells)

!     !     test_str = get_string(f_segment%valueunits)
!     !     write (*,*) "test_str =   ", test_str
!     ! else
!     !     write (*,*) "something did not work with ovf_read_segment_header"
!     ! end if

!     ! !---------------------

!     ! size = product(f_segment%n_cells)
!     ! allocate(array_4(3,size))
!     ! allocate(array_8(3,size))

!     ! !---------------------

!     ! if (ovf_read_segment_data_4(c_file, 0, f_segment, c_loc(array_4(1,1))) == -1) then
!     !     write (*,*) "array_4(:,2) = ", array_4(:,2)
!     ! else
!     !     write (*,*) "something did not work with ovf_read_segment_data_4"
!     ! end if

!     ! if (ovf_read_segment_data_8(c_file, 0, f_segment, c_loc(array_8(1,1))) == -1) then
!     !     write (*,*) "array_8(:,2) = ", array_8(:,2)
!     ! else
!     !     write (*,*) "something did not work with ovf_read_segment_data_8"
!     ! end if

!     ! !---------------------

!     ! if (ovf_read_segment_data_4(c_file, 1, f_segment, c_loc(array_4(1,1))) == -1) then
!     !     write (*,*) "array_4(:,2) = ", array_4(:,2)
!     ! else
!     !     write (*,*) "something did not work with ovf_read_segment_data_4"
!     ! end if

!     ! !---------------------

!     ! success = ovf_close(c_file)

!     ! !---------------------

!     ! c_file = ovf_open(C_CHAR_"python/test/testfile_f_out.ovf"//C_NULL_CHAR)

!     ! array_4(:,1) = 4
!     ! array_8(:,1) = 8

!     ! if (ovf_write_segment_4(c_file, f_segment, c_loc(array_4(1,1)), OVF_FORMAT_BIN) == -1) then
!     !     write (*,*) "array_4(:,1) = ", array_4(:,1)
!     ! else
!     !     write (*,*) "something did not work with ovf_write_segment_4"
!     ! end if

!     ! if (ovf_append_segment_8(c_file, f_segment, c_loc(array_8(1,1)), OVF_FORMAT_CSV) == -1) then
!     !     write (*,*) "array_8(:,1) = ", array_8(:,1)
!     ! else
!     !     write (*,*) "something did not work with ovf_append_segment_8"
!     ! end if

!     ! !---------------------

!     ! success = ovf_close(c_file)

! end program main


