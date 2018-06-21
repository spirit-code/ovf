import os
import sys

ovf_py_dir = os.path.abspath(os.path.join(os.path.dirname( __file__ ), ".."))
sys.path.insert(0, ovf_py_dir)

from ovf import ovf

import numpy as np

# import unittest

##########

print("----- ovf test nonexistent")
with ovf.ovf_file("nonexistent.ovf") as ovf_file:
    print(ovf_file)
    print("found:      ", ovf_file.found)
    print("is_ovf:     ", ovf_file.is_ovf)
    print("n_segments: ", ovf_file.n_segments)
    segment = ovf.ovf_segment()
    if ovf_file.read_segment_header(0, segment) != -1:
        print("ovf file returned error: ", ovf_file.get_latest_message())
print("----- ovf test nonexistent done")


print("----- ovf test existent")
with ovf.ovf_file(os.path.abspath(os.path.join(os.path.dirname( __file__ ), "testfile.ovf"))) as ovf_file:
    print(ovf_file)
    print("found:      ", ovf_file.found)
    print("is_ovf:     ", ovf_file.is_ovf)
    print("n_segments: ", ovf_file.n_segments)
    data = np.zeros((100, 100, 1, 3), dtype='f')
    segment = ovf.ovf_segment()
    if ovf_file.read_segment_header(0, segment) != -1:
        print("read_segment_header failed")
    if ovf_file.read_segment_data(0, segment, data) != -1:
        print("read_segment_data failed")
    print("first: ", data[0,0,0,:])
    print("last:  ", data[-1,-1,-1,:])
print("----- ovf test existent done")


print("----- ovf test writing")
with ovf.ovf_file(os.path.abspath(os.path.join(os.path.dirname( __file__ ), "testfile_out.ovf"))) as ovf_file:
    print(ovf_file)
    data = np.zeros((2, 2, 1, 3), dtype='d')
    data[0,1,0,:] = [3.0, 2.0, 1.0]
    segment = ovf.ovf_segment(n_cells=[2,2,1])
    if ovf_file.write_segment(segment, data) != -1:
        print("read_segment_header failed")
print("----- ovf test writing done")    