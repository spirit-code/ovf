cd build
make
cd ..
gfortran -c src/fortran_wrapper.f90
gfortran fortran_wrapper.o build/libovf_static.a -o test.x -lc -lstdc++   
./test.x
