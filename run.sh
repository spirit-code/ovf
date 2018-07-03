cd build
make
cd ..
gfortran -c fortran/ovf.f90 -o f_ovf.o
gfortran -c fortran/test/simple.f90 -o f_simple.o
gfortran f_simple.o f_ovf.o build/libovf_static.a -o simple.x  -lstdc++   
./simple.x
