OVF Parser Library
=================================
**Simple API for powerful OOMMF Vector Field file parsing**<br />


How to use
---------------------------------

TODO...

### C/C++
- `struct ovf_file *myfile = ovf_open("myfilename.ovf");` to open a file
- `myfile->found` to check if the file exists on disk
- `myfile->is_ovf` to check if the file contains an OVF header
- `myfile->n_segments` to check the number of segments the file should contain
- `ovf_close(myfile);` to close the file and free resources

### Python

### Fortran


How to embed it into your project
---------------------------------

TODO...


Build
---------------------------------

On Unix systems (usually):
```
mkdir build
cd build
cmake ..
make
```

On Windows (one possibility):
- open the folder in the CMake GUI
- generate the VS project
- open the resulting project in VS and build it