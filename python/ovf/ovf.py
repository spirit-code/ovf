import ovf.ovflib as ovflib
import ctypes


### Load Library
_ovf = ovflib.LoadOVFLibrary()


### --------------------------------------------------------------

### Read a segment with float precision
_ovf_read_segment_4 = _ovf.ovf_read_segment_4
_ovf_read_segment_4.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_float)]
_ovf_read_segment_4.restype = ctypes.c_int

### --------------------------------------------------------------


class ovf_segment(ctypes.Structure):
    ### Some properties
    _fields_ = [
        ("title",       ctypes.c_char_p),
        ("meshunits",   ctypes.c_char_p),
        ("valuedim",    ctypes.c_int),
        ("valueunits",  ctypes.c_char_p),
        ("valuelabels", ctypes.c_char_p),
        ("meshtype",    ctypes.c_char_p)
    ]

class _ovf_file(ctypes.Structure):
    ### Some properties
    _fields_ = [
        ("found",        ctypes.c_bool),
        ("is_ovf",       ctypes.c_bool),
        ("n_segments",   ctypes.c_int),
        ("_file_handle", ctypes.c_void_p)
    ]

    def read_segment_4(self, index, expected_geometry, segment, data):
        return int(_ovf_read_segment_4(self._p_file, ctypes.c_int(index), expected_geometry, segment, data))


### Setup State
_ovf_open = _ovf.ovf_open
_ovf_open.argtypes = [ctypes.c_char_p]
_ovf_open.restype = ctypes.POINTER(_ovf_file)
def open(filename):
    return _ovf_open(ctypes.c_char_p(filename.encode('utf-8')))


### Delete State
_ovf_close = _ovf.ovf_close
_ovf_close.argtypes = [ctypes.POINTER(_ovf_file)]
_ovf_close.restype = ctypes.c_int
def close(p_file):
    return int(_ovf_close(p_file))


### ovf file wrapper class to be used in 'with' statement
class ovf_file():
    """Wrapper Class for an OVF file"""

    ### Functions to make 'with' statement work
    def __init__(self, filename):
        self._p_file = open(filename)

    def __enter__(self):
        return self._p_file.contents

    def __exit__(self, exc_type, exc_value, traceback):
        close(self._p_file)