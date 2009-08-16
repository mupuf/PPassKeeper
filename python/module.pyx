cdef extern from "ppasskeeper.h":
    ctypedef struct ppk_module:
        char* id
        char* display_name

    unsigned int ppk_module_count()
    ppk_module *ppk_module_list()
    ppk_error ppk_modules_reload_list()
    const char* ppk_module_get_default()
    ppk_error ppk_module_set_default(const char* module_id);
    ppk_boolean ppk_module_is_available(const char* module_id);
    ppk_error ppk_module_read_flags(const char* module_id, unsigned int* flags);
    ppk_error ppk_module_write_flags(const char* module_id, unsigned int* flags);
    ppk_error ppk_module_listing_flags(const char* module_id, unsigned int* flags);
    ppk_security_level ppk_module_security_level(const char* module_id);
    ppk_boolean ppk_module_is_writable(const char* module_id);
    size_t ppk_module_max_data_size(const char* module_id, ppk_data_type type);
    size_t ppk_module_get_entry_count(const char* module_id, int entry_types, unsigned int flags);
    size_t ppk_module_get_entry_list(const char* module_id, int entry_types, ppk_entry *entryList, size_t nbEntries, unsigned int flags);

cdef class Module:
    def __init__(self, module_id):
        self.module_id = module_id

    @staticmethod
    def get_default():
        return Module(ppk_module_get_default())

    @staticmethod
    def set_default(module_id):
        return ppk_module_set_default(module_id)

    def is_available(self):
        return ppk_module_is_available(self.module_id)

    def read_flags(self):
        return ppk_module_read_flags(self.module_id)

    def write_flags(self):
        return ppk_module_write_flags(self.module_id)

    def listing_flags(self):
        return ppk_module_listing_flags(self.module_id)

    def security_level(self):
        return ppk_module_security_level(self.module_id)

    def is_writable(self):
        return ppk_module_is_writable(self.module_id)

    def max_data_size(self, data_type):
        return ppk_module_max_data_size(self.module_id, data_type)

    def entry_count(self, entry_types, flags):
        return ppk_module_get_entry_count(self.module_id, entry_types, flags)

    #def get_entry_list(self, entry_types, entry_list, )

    #def hasEntry()

cdef class ModuleList:
    cdef ppk_module* modules

    def __init__(self):
        self.modules = ppk_module_list()
        self.length = ppk_module_count()

    def __dealloc__(self):
        #TODO: free the list
        pass

    def reload(self):
        ppk_module_reload_list()
        #TODO: free the list
        self.modules = ppk_modules_list()
        self.length = ppk_module_count()

    def __len__(self):
        return self.length

    def __getitem__(self, module_id):
        return Module(module_id)
