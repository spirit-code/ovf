import os
import sys

ovf_py_dir = os.path.abspath(os.path.join(os.path.dirname( __file__ ), ".."))
sys.path.insert(0, ovf_py_dir)

from ovf import ovf

import numpy as np

# import unittest

##########

with ovf.ovf_file("nonexistent.ovf") as ovf_file:
    print("----- ovf test nonexistent")
    print(ovf_file)
    print("found:      ", ovf_file.found)
    print("is_ovf:     ", ovf_file.is_ovf)
    print("n_segments: ", ovf_file.n_segments)
    print("----- ovf test nonexistent done")


with ovf.ovf_file(os.path.abspath(os.path.join(os.path.dirname( __file__ ), "testfile.ovf"))) as ovf_file:
    print("----- ovf test existent")
    print(ovf_file)
    print("found:      ", ovf_file.found)
    print("is_ovf:     ", ovf_file.is_ovf)
    print("n_segments: ", ovf_file.n_segments)
    data = np.zeros((100, 100, 1, 3), dtype=np.float)
    segment = ovf.ovf_segment()
    if ovf_file.read_segment_header(0, segment) != -1:
        print("read_segment_header failed")
    if ovf_file.read_segment_data(0, segment, data) != -1:
        print("read_segment_data failed")
    print("first: ", data[0,0,0,:])
    print("last:  ", data[-1,-1,0,:])
    print("----- ovf test existent done")