from ctypes import c_char_p, c_void_p, c_ushort, c_int, c_ulong, Structure, Union

class StructModule(Structure):
    _fields_ = [('id', c_char_p),
                ('display_name', c_char_p)]

class StructNetworkEntry(Structure):
    _fields_ = [('host', c_char_p),
                ('login', c_char_p),
                ('port', c_ushort)]

class StructApplicationEntry(Structure):
    _fields_ = [('app_name', c_char_p),
                ('username', c_char_p)]

class StructEntry(Structure):
    class _UnionEntry(Union):
        _fields_ = [('net', StructNetworkEntry),
                    ('app', StructApplicationEntry),
                    ('item', c_char_p)]
    _fields_ = [('type', c_int),
                ('u', _UnionEntry)]

class StructBlobData(Structure):
    _fields_ = [('data', c_void_p),
                ('size', c_ulong)]

class StructData(Structure):
    class _UnionData(Union):
        _fields_ = [('string', c_char_p),
                    ('blob', StructBlobData)]
    _fields_ = [('type', c_int),
                ('u', _UnionData)]
