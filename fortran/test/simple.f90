program main
use ovf

type(ovf_file)      :: file

call file%load("testfile.ovf")
    write (*,*) "Found = ",   file%found
    write (*,*) "is_ovf = ",  file%is_ovf
    write (*,*) "n_segments", file%n_segments
end program main