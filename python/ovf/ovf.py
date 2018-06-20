import ovf.ovflib as ovflib
import ctypes


### Load Library
_ovf = ovflib.LoadOVFLibrary()



class ovf_segment(ctypes.Structure):
    ### Some properties
    _fields_ = [
        ("title",            ctypes.c_char_p),
        ("valuedim",         ctypes.c_int),
        ("valueunits",       ctypes.c_char_p),
        ("valuelabels",      ctypes.c_char_p),
        ("meshtype",         ctypes.c_char_p),
        ("meshunits",        ctypes.c_char_p),
        ("n_cells",          ctypes.POINTER(ctypes.c_int)),
        ("N",                ctypes.c_int),
        ("bounds_min",       ctypes.POINTER(ctypes.c_float)),
        ("bounds_max",       ctypes.POINTER(ctypes.c_float)),
        ("lattice_constant", ctypes.c_float),
        ("bounds_max",       ctypes.POINTER(ctypes.POINTER(ctypes.c_float)))
    ]


### --------------------------------------------------------------

### Read a segment header
_ovf_read_segment_header = _ovf.ovf_read_segment_header
_ovf_read_segment_header.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ovf_segment)]
_ovf_read_segment_header.restype  = ctypes.c_int

### Read a segment with float precision
_ovf_read_segment_data_4 = _ovf.ovf_read_segment_data_4
_ovf_read_segment_data_4.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ovf_segment), ctypes.POINTER(ctypes.c_float)]
_ovf_read_segment_data_4.restype  = ctypes.c_int

### --------------------------------------------------------------


class _ovf_file(ctypes.Structure):
    ### Some properties
    _fields_ = [
        ("found",        ctypes.c_bool),
        ("is_ovf",       ctypes.c_bool),
        ("n_segments",   ctypes.c_int),
        ("_file_handle", ctypes.c_void_p)
    ]

    def read_segment_header(self, index, segment):
        return int(_ovf_read_segment_header(ctypes.addressof(self), ctypes.c_int(index), ctypes.POINTER(ovf_segment)(segment)))

    def read_segment_data(self, index, segment, data):
        datap = data.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
        return int(_ovf_read_segment_data_4(ctypes.addressof(self), ctypes.c_int(index), ctypes.POINTER(ovf_segment)(segment), datap))


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