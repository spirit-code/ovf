import ovf.ovflib as ovflib
import ctypes

### Load Library
_ovf = ovflib.LoadOVFLibrary()

### Setup State
_ovf_open = _ovf.ovf_open
_ovf_open.argtypes = [ctypes.c_char_p]
_ovf_open.restype = ctypes.c_void_p
def open(filename):
    return _ovf_open(ctypes.c_char_p(filename.encode('utf-8')))

### Delete State
_ovf_close = _ovf.ovf_close
_ovf_close.argtypes = [ctypes.c_void_p]
_ovf_close.restype = ctypes.c_int
def close(p_file):
    return _ovf_close(ctypes.c_void_p(p_file))


### ovf file wrapper class to be used in 'with' statement
class ovf_file(ctypes.Structure):
    """Wrapper Class for an OVF file"""
    _fields_ = [
        ("found",       ctypes.c_bool),
        ("is_ovf",      ctypes.c_bool),
        ("n_segments",  ctypes.c_int)
    ]

    def __init__(self, filename):
        self.p_file = open(filename)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        close(self.p_file)