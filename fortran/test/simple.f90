program main
use ovf

    type(ovf_file)      :: file
    type(ovf_segment)   :: segment
    integer             :: success
    real(kind=4), allocatable :: array_4(:,:)
    real(kind=8), allocatable :: array_8(:,:)

    call file%open_file("python/test/testfile_py.ovf")
    write (*,*) "Found      = ", file%found
    write (*,*) "is_ovf     = ", file%is_ovf
    write (*,*) "n_segments = ", file%n_segments

    success = file%read_segment_header(segment)
    if ( success == OVF_OK) then
        write (*,*) "n_cells = ", segment%N_Cells
        write (*,*) "n_total = ", segment%N
    else
        write (*,*) "test read_segment_header did not work. Message: ", file%latest_message
        STOP 1
    endif

    ! Allocation of arrays is optional with ovf read functions
    ! allocate( array_4(3, segment%N) )
    ! allocate( array_8(3, segment%N_Cells) )

    success = file%read_segment_data(segment, array_4)
    if ( success == OVF_OK) then
        write (*,*) "array_4(:,2) = ", array_4(:,2)
    else
        write (*,*) "test read_segment_data on array_4 did not work. Message: ", file%latest_message
        STOP 1
    endif

    success = file%read_segment_data(segment, array_8)
    if ( success == OVF_OK) then
        write (*,*) "array_8(:,2) = ", array_8(:,2)
    else
        write (*,*) "test read_segment_data on array_8 did not work. Message: ", file%latest_message
        STOP 1
    endif

    success = file%close_file()
    if ( success == OVF_OK) then
        write (*,*) "test file closed"
    else
        write (*,*) "test close_file did not work. Message: ", file%latest_message
        STOP 1
    endif

end program main