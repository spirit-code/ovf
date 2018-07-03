program main
use ovf

    type(ovf_file)      :: file
    type(ovf_segment)   :: segment
    integer             :: success

    call file%open_file("python/test/testfile_py.ovf")

    write (*,*) "Found      = ", file%found
    write (*,*) "is_ovf     = ", file%is_ovf
    write (*,*) "n_segments = ", file%n_segments

    success = file%read_segment_header(segment)

    if ( success == OVF_OK) then
        write (*,*) "n_cells = ", segment%N_Cells
        write (*,*) "n_total = ", segment%N
    else
        write (*,*) "test read_segment_header did not work"
        STOP 1
    endif

    success = file%close_file()
    if ( success == OVF_OK) then
        write (*,*) "test file closed"
    else
        write (*,*) "test close_file did not work"
        STOP 1
    endif

end program main