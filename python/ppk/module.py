from __init__ import _handle

from ctypes import c_char, byref
from types import StructModule, StructData, StructEntry
from data import _struct_to_data
from entry import _struct_to_entry
from constants import PPK_PARAM_MAX, EntryType

class Module:
    def __init__(self, struct_module):
        self._id = struct_module.id
        self._display_name = struct_module.display_name
        self.id = str(self._id)
        self.display_name = str(self._display_name)
    def is_available(self):
        return bool(_handle.ppk_moduleAvailable(self._id))
    def available_read_flags(self):
        return _handle.ppk_readFlagsAvailable(self._id)
    def available_write_flags(self):
        return _handle.ppk_writeFlagsAvailable(self._id)
    def available_listing_flags(self):
        return _handle.ppk_listingFlagsAvailable(self._id)
    def is_writable(self):
        return _handle.ppk_isWritable(self.id)
    def get_last_error(self):
        return _handle.ppk_getLastError(self.id)
    def save_param(self, key, value):
        return bool(_handle.ppk_saveParam(self.id, key, value))
    def get_param(self, key):
        returned_string = c_char * PPK_PARAM_MAX
        if _handle.ppk_getParam(self.id, key, returned_string, PPK_PARAM_MAX):
            return returned_string
        else:
            return None
    def set_custom_prompt_message(self, custom_message):
        return bool(_handle.ppk_setCustomPromptMessage(self.id, custom_message))
    def security_level(self):
        return _handle.ppk_securityLevel(self.id)
    def max_data_size(self, data_type):
        return _handle.ppk_maxDataSize(self.id, data_type)
    def get_entry(self, entry, flags = 0):
        edata = StructData()
        if _handle.ppk_getEntry(self.id, entry._to_struct(), byref(edata), flags):
            return _struct_to_data(edata)
        else:
            return None
    def set_entry(self, entry, data, flags = 0):
        return bool(_handle.ppk_setEntry(self.id, entry._to_struct(), data._to_struct(), flags))
    def remove_entry(self, entry, flags):
        return bool(_handle.ppk_removeEntry(self.id, entry._to_struct(), flags))
    def entry_count(self, entry_types = EntryType.all(), flags = 0):
        return _handle.ppk_getEntryListCount(self.id, entry_types, flags)
    def entry_list(self, entry_types = EntryType.all(), flags = 0):
        count = self.entry_count(entry_types, flags)
        l = (StructEntry * count)()
        _handle.ppk_getEntryList(self.id, entry_types, l, count, flags)
        return map(lambda x: _struct_to_entry(x), l)
    def has_entry(self, entry):
        return bool(_handle.ppk_entryExists(self.id, entry._to_struct(), flags))

class ModuleList:
    __shared_state = {} # Borg pattern
    def __init__(self):
        self.__dict__ = self.__shared_state
        try:
            b = self.__init
        except AttributeError:
            self._count = _handle.ppk_getAvailableModulesCount()
            self._modules = (StructModule * self._count)()
            _handle.ppk_getAvailableModules(self._modules, self._count)
            self.modules = {}
            for mod in self._modules:
                self.modules[mod.id] = Module(mod)
            self.__init = True
    def __iter__(self):
        for i in self.modules.keys():
            yield self.modules[i]
    def __len__(self):
        return self._count
    def count(self):
        return len(self)
    def __getitem__(self, i):
        return self.modules[i]
