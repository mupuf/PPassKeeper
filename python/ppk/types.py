from ctypes import c_char_p, Structure

class StructModule(Structure):
    _fields_ = [('id', c_char_p),
                ('display_name', c_char_p)]
