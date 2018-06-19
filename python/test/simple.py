import os
import sys

ovf_py_dir = os.path.abspath(os.path.join(os.path.dirname( __file__ ), ".."))
sys.path.insert(0, ovf_py_dir)

from ovf import ovf

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
    print("----- ovf test existent done")