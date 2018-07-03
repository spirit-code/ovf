module ovf
use, intrinsic  :: iso_c_binding
implicit none

integer, parameter  :: OVF_OK      = -1
integer, parameter  :: OVF_ERROR   = -2
integer, parameter  :: OVF_INVALID = -3

integer, parameter  :: OVF_FORMAT_BIN  = -55
integer, parameter  :: OVF_FORMAT_TEXT = -56
integer, parameter  :: OVF_FORMAT_CSV  = -57

type, bind(c) :: c_ovf_file
    type(c_ptr)     :: filename
    integer(c_int)  :: found
    integer(c_int)  :: is_ovf
    integer(c_int)  :: n_segments
    type(c_ptr)     :: file_handle
end type c_ovf_file

type, bind(c) :: c_ovf_segment
    type(c_ptr)         :: title
    integer(kind=c_int) :: valuedim
    type(c_ptr)         :: valueunits
    type(c_ptr)         :: valuelabels

    type(c_ptr)         :: meshtype
    type(c_ptr)         :: meshunits

    integer(kind=c_int) :: pointcount

    integer(kind=c_int) :: n_cells(3)
    integer(kind=c_int) :: N

    real(kind=c_float)  :: bounds_min(3)
    real(kind=c_float)  :: bounds_max(3)

    real(kind=c_float)  :: lattice_constant
    real(kind=c_float)  :: bravais_vectors(3,3)
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
    character(len=:), allocatable   :: latest_message
    type(c_ptr)                     :: private_file_binding
contains
    procedure :: open_file           => open_file
    procedure :: read_segment_header => read_segment_header
    procedure :: read_segment_data_4
    procedure :: read_segment_data_8
    GENERIC   :: read_segment_data   => read_segment_data_4, read_segment_data_8
    procedure :: close_file          => close_file
end type ovf_file

contains

    ! Helper function to generate a Fortran string from a C char pointer
    function get_string(c_pointer) result(f_string)
        use, intrinsic :: iso_c_binding
        implicit none
        type(c_ptr), intent(in)         :: c_pointer
        character(len=:), allocatable   :: f_string

        character(len=:), pointer       :: f_ptr
        integer(c_size_t)               :: l_str

        interface
            function c_strlen(str_ptr) bind ( C, name = "strlen" ) result(len)
            use, intrinsic :: iso_c_binding
                type(c_ptr), value      :: str_ptr
                integer(kind=c_size_t)  :: len
            end function c_strlen
        end interface

        call c_f_pointer(c_pointer, f_ptr)
        l_str = c_strlen(c_pointer)

        f_string = f_ptr(1:l_str)
    end function get_string


    ! Helper function to create C-struct c_ovf_secment from Fortran type ovf_segment
    function get_c_ovf_segment(segment) result(c_segment)
        use, intrinsic :: iso_c_binding
        implicit none
        type(ovf_segment), intent(in) :: segment
        type(c_ovf_segment)           :: c_segment

        c_segment%title      = c_loc(segment%Title)
        c_segment%valueunits = c_loc(segment%ValueUnits)
        c_segment%valuedim   = segment%ValueDim
        c_segment%n_cells(:) = segment%n_cells(:)
        c_segment%N          = product(segment%n_cells)
    end function get_c_ovf_segment


    ! Helper function to turn C-struct c_ovf_secment into Fortran type ovf_segment
    subroutine fill_ovf_segment(c_segment, segment)
        use, intrinsic :: iso_c_binding
        implicit none
        type(c_ovf_segment), intent(in)    :: c_segment
        type(ovf_segment),   intent(inout) :: segment

        segment%Title      = get_string(c_segment%title)

        segment%ValueUnits = get_string(c_segment%valueunits)
        segment%ValueDim   = c_segment%valuedim

        segment%N_Cells(:) = c_segment%n_cells(:)
        segment%N          = product(c_segment%n_cells)
    end subroutine fill_ovf_segment


    ! Helper function to get latest message
    subroutine handle_messages(file)
        implicit none

        class(ovf_file) :: file
        type(c_ptr)     :: message_ptr

        interface
            function ovf_latest_message(file) &
                            bind ( C, name = "ovf_latest_message" ) &
                            result(message)
            use, intrinsic :: iso_c_binding
                type(c_ptr), value  :: file
                type(c_ptr)         :: message
            end function ovf_latest_message
        end interface

        message_ptr = ovf_latest_message(file%private_file_binding)
        file%latest_message = get_string(message_ptr)

    end subroutine handle_messages


    subroutine open_file(self, filename)
        implicit none
        class(ovf_file)                 :: self
        character(len=*), intent(in)    :: filename

        type(c_ovf_file), pointer       :: f_file
        type(c_ptr)                     :: c_file

        interface
            function ovf_open(filename) &
                            bind ( C, name = "ovf_open" ) 
            use, intrinsic :: iso_c_binding
                character(len=1,kind=c_char)    :: filename(*)
                type(c_ptr)                     :: ovf_open
            end function ovf_open
        end interface

        c_file = ovf_open(trim(filename) // c_null_char)
        call c_f_pointer(c_file, f_file)

        self%filename      = get_string(f_file%filename)
        self%found         = f_file%found  == 1
        self%is_ovf        = f_file%is_ovf == 1
        self%n_segments    = f_file%n_segments
        self%private_file_binding = c_file
    end subroutine open_file


    function read_segment_header(self, segment) result(success)
        implicit none
        class(ovf_file)     :: self
        type(ovf_segment)   :: segment
        integer             :: success

        type(c_ovf_segment) :: c_segment
        type(c_ptr)         :: c_segment_ptr

        interface
            function ovf_read_segment_header(file, index, segment) &
                                            bind ( C, name = "ovf_read_segment_header" ) &
                                            result(success)
            use, intrinsic :: iso_c_binding
            Import :: c_ovf_file, c_ovf_segment
                type(c_ptr), value              :: file
                integer(kind=c_int), value      :: index 
                type(c_ptr), value              :: segment
                integer(kind=c_int)             :: success
            end function ovf_read_segment_header
        end interface

        c_segment_ptr = c_loc(c_segment)
        success = ovf_read_segment_header(self%private_file_binding, 0, c_segment_ptr)

        if ( success == OVF_OK) then
            call fill_ovf_segment(c_segment, segment)
        end if

        call handle_messages(self)

    end function read_segment_header


    function read_segment_data_4(self, segment, array, index_in) result(success)
        implicit none
        class(ovf_file)               :: self
        type(ovf_segment), intent(in) :: segment
        real(kind=4), allocatable     :: array(:,:)
        integer, optional, intent(in) :: index_in
        integer                       :: success

        integer                       :: index
        type(c_ovf_segment)           :: c_segment
        type(c_ptr)                   :: c_segment_ptr
        type(c_ptr)                   :: c_array_ptr

        interface
            function ovf_read_segment_data_4(file, index, segment, array) &
                                            bind ( C, name = "ovf_read_segment_data_4" ) &
                                            result(success)
            use, intrinsic :: iso_c_binding
            Import :: c_ovf_file, c_ovf_segment
                type(c_ptr), value          :: file
                integer(kind=c_int), value  :: index
                type(c_ptr), value          :: segment
                type(c_ptr), value          :: array
                integer(kind=c_int)         :: success
            end function ovf_read_segment_data_4
        end interface

        if( present(index_in) ) then
            index = index_in
        else
            index = 1
        end if

        if (allocated(array)) then
            ! TODO: check array dimensions
        else
            allocate( array(segment%ValueDim, segment%N) )
            array(:,:) = 0
        endif

        ! Parse into C-structure
        c_segment = get_c_ovf_segment(segment)

        ! Get C-pointers to C-structures
        c_segment_ptr = c_loc(c_segment)
        c_array_ptr   = c_loc(array(1,1))

        ! Call the C-API
        success = ovf_read_segment_data_4(self%private_file_binding, index-1, c_segment_ptr, c_array_ptr)

        call handle_messages(self)

    end function read_segment_data_4


    function read_segment_data_8(self, segment, array, index_in) result(success)
        implicit none
        class(ovf_file)               :: self
        type(ovf_segment), intent(in) :: segment
        real(kind=8), allocatable     :: array(:,:)
        integer, optional, intent(in) :: index_in
        integer                       :: success

        integer                       :: index
        type(c_ovf_segment)           :: c_segment
        type(c_ptr)                   :: c_segment_ptr
        type(c_ptr)                   :: c_array_ptr

        interface
            function ovf_read_segment_data_8(file, index, segment, array) &
                                            bind ( C, name = "ovf_read_segment_data_8" ) &
                                            result(success)
            use, intrinsic :: iso_c_binding
            Import :: c_ovf_file, c_ovf_segment
                type(c_ptr), value          :: file
                integer(kind=c_int), value  :: index
                type(c_ptr), value          :: segment
                type(c_ptr), value          :: array
                integer(kind=c_int)         :: success
            end function ovf_read_segment_data_8
        end interface

        if( present(index_in) ) then
            index = index_in
        else
            index = 1
        end if

        if (allocated(array)) then
            ! TODO: check array dimensions
        else
            allocate( array(segment%ValueDim, segment%N) )
            array(:,:) = 0
        endif

        ! Parse into C-structure
        c_segment = get_c_ovf_segment(segment)

        ! Get C-pointers to C-structures
        c_segment_ptr = c_loc(c_segment)
        c_array_ptr   = c_loc(array(1,1))

        ! Call the C-API
        success = ovf_read_segment_data_8(self%private_file_binding, index-1, c_segment_ptr, c_array_ptr)

        call handle_messages(self)

    end function read_segment_data_8


    function close_file(self) result(success)
        implicit none
        class(ovf_file) :: self
        integer         :: success

        interface
            function ovf_close(file) &
                                bind ( C, name = "ovf_close" ) &
                                result(success)
            use, intrinsic :: iso_c_binding
                type(c_ptr), value              :: file
                integer(kind=c_int)             :: success
            end function ovf_close
        end interface

        success = ovf_close(self%private_file_binding)

        call handle_messages(self)

    end function close_file


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


