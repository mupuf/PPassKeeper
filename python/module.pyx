cdef extern from "ppasskeeper.h":
    ctypedef struct ppk_module:
        char* id
        char* display_name

    unsigned int ppk_module_count()

cdef class ModuleList:
    cdef ppk_module* modules
    cdef unsigned int count

    def __new__(self):
        pass

    def __dealloc__(self):
        pass
