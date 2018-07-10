OVF Parser Library
=================================
**Simple API for powerful OOMMF Vector Field file parsing**<br />

**[Python package](https://pypi.org/project/ovf/):** [![PyPI version](https://badge.fury.io/py/ovf.svg)](https://badge.fury.io/py/ovf)

How to use
---------------------------------

For usage examples, take a look into the test folders: [test](https://github.com/spirit-code/ovf/tree/master/test), [python/test](https://github.com/spirit-code/ovf/tree/master/python/test) or [fortran/test](https://github.com/spirit-code/ovf/tree/master/fortran/test).

Except for opening a file, all functions return status codes (generally `OVF_OK` or `OVF_ERROR`).
When the return code is not `OVF_OK`, you can take a look into the latest message, which should
tell you what the problem was (`const char * ovf_latest_message(struct ovf_file *)` in the C API).

### C/C++

- `struct ovf_file *myfile = ovf_open("myfilename.ovf");` to open a file
- `myfile->found` to check if the file exists on disk
- `myfile->is_ovf` to check if the file contains an OVF header
- `myfile->n_segments` to check the number of segments the file should contain
- `ovf_close(myfile);` to close the file and free resources

### Python

To install the *ovf python package*, either build and install from source
or simply use

    pip install ovf

To use `ovf` from Python, e.g.

```Python
from ovf import ovf
import numpy as np

data = np.zeros((2, 2, 1, 3), dtype='f')
data[0,1,0,:] = [3.0, 2.0, 1.0]

with ovf.ovf_file("out.ovf") as ovf_file:

    # Write one segment
    segment = ovf.ovf_segment(n_cells=[2,2,1])
    if ovf_file.write_segment(segment, data) != -1:
        print("write_segment failed: ", ovf_file.get_latest_message())

    # Add a second segment to the same file
    data[0,1,0,:] = [4.0, 5.0, 6.0]
    if ovf_file.append_segment(segment, data) != -1:
        print("append_segment failed: ", ovf_file.get_latest_message())
```

### Fortran

The Fortran bindings are written in object-oriented style for ease of use.
Writing a file, for example:

```fortran
type(ovf_file)      :: file
type(ovf_segment)   :: segment
integer             :: success
real(kind=4), allocatable :: array_4(:,:)
real(kind=8), allocatable :: array_8(:,:)

! Initialize segment
call segment%initialize()

! Write a file
call file%open_file("fortran/test/testfile_f.ovf")
segment%N_Cells = [ 2, 2, 1 ]
segment%N = product(segment%N_Cells)

allocate( array_4(3, segment%N) )
array_4 = 0
array_4(:,1) = [ 6.0, 7.0, 8.0 ]
array_4(:,2) = [ 5.0, 4.0, 3.0 ]

success = file%write_segment(segment, array_4, OVF_FORMAT_TEXT)
if ( success == OVF_OK) then
    write (*,*) "test write_segment succeeded."
    ! write (*,*) "n_cells = ", segment%N_Cells
    ! write (*,*) "n_total = ", segment%N
else
    write (*,*) "test write_segment did not work. Message: ", file%latest_message
    STOP 1
endif
```

For more information on how to generate modern Fortran bindings,
see also https://github.com/MRedies/Interfacing-Fortran

How to embed it into your project
---------------------------------

TODO...


Build
---------------------------------

### On Unix systems

Usually:
```
mkdir build
cd build
cmake ..
make
```

### On Windows

One possibility:
- open the folder in the CMake GUI
- generate the VS project
- open the resulting project in VS and build it

### CMake Options

The following options are `ON` by default.
If you want to switch them off, just pass `-D<OPTION>=OFF` to CMake,
e.g. `-DOVF_BUILD_FORTRAN_BINDINGS=OFF`.

- `OVF_BUILD_PYTHON_BINDINGS`
- `OVF_BUILD_FORTRAN_BINDINGS`
- `OVF_BUILD_TEST`

On Windows, you can also set these from the CMake GUI.

### Create and install the Python package

Instead of `pip`-installing it, you can e.g. build everything
and then install the package locally, where the `-e` flag will
let you change/update the package without having to re-install it.

```
cd python
pip install -e .
```

### Build without CMake

The following is an example of how to manually build the C library and
link it with bindings into a corresponding Fortran executable, using gcc.

C library:
```
g++ -DFMT_HEADER_ONLY -Iinclude -fPIC -std=c++11 -c src/ovf.cpp

ar qc libovf_static.a  ovf.o
ranlib libovf_static.a
```

Fortran library:
```
gfortran -fPIC -c fortran/ovf.f90 -o ovf.f90.o

ar qc libovf_fortran.a  ovf.f90.o
ranlib libovf_fortran.a
```

Fortran executable
```
gfortran -c fortran/test/simple.f90 -o simple.f90.o
gfortran simple.f90.o libovf_fortran.a libovf_static.a -lstdc++ -o test_fortran_simple
```

*Note: on OSX simply replace `-lstdc++` with `-lc++`*