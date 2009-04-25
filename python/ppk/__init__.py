from ctypes import CDLL, POINTER, c_int, c_uint, c_char_p, c_size_t
from types import StructModule, StructEntry, StructData

_handle = CDLL('libppasskeeper.so.1')

_handle.ppk_getAvailableModulesCount.restype = c_uint
_handle.ppk_getAvailableModules.restype = c_uint
_handle.ppk_readFlagsAvailable.restype = c_uint
_handle.ppk_writeFlagsAvailable.restype = c_uint
_handle.ppk_listingFlagsAvailable.restype = c_uint
_handle.ppk_getEntryListCount.restype = c_uint
_handle.ppk_getEntryList.restype = c_uint
_handle.ppk_maxDataSize.restype = c_uint
_handle.ppk_getLastError.restype = c_char_p
_handle.ppk_getDefaultModule.restype = c_char_p
_handle.ppk_settingDirectory.restype = c_char_p

_handle.ppk_setPassword.argtypes = [c_char_p]
_handle.ppk_unlock.argtypes = [c_char_p]
_handle.ppk_getAvailableModules.argtypes = [POINTER(StructModule), c_uint]
_handle.ppk_moduleAvailable.argtypes = [c_char_p]
_handle.ppk_readFlagsAvailable.argtypes = [c_char_p]
_handle.ppk_writeFlagsAvailable.argtypes = [c_char_p]
_handle.ppk_listingFlagsAvailable.argtypes = [c_char_p]
_handle.ppk_getEntry.argtypes = [c_char_p, StructEntry, POINTER(StructData), c_uint]
_handle.ppk_setEntry.argtypes = [c_char_p, StructEntry, StructData, c_uint]
_handle.ppk_removeEntry.argtypes = [c_char_p, StructEntry, c_uint]
_handle.ppk_getEntryListCount.argtypes = [c_char_p, c_uint, c_uint]
_handle.ppk_getEntryList.argtypes = [c_char_p, c_uint, POINTER(StructEntry), c_uint, c_uint]
_handle.ppk_entryExists.argtypes = [c_char_p, StructEntry, c_uint]
_handle.ppk_maxDataSize.argtypes = [c_char_p, c_int]
_handle.ppk_isWritable.argtypes = [c_char_p]
_handle.ppk_securityLevel.argtypes = [c_char_p]
_handle.ppk_saveParam.argtypes = [c_char_p, c_char_p, c_char_p]
_handle.ppk_getParam.argtypes = [c_char_p, c_char_p, c_size_t]
_handle.ppk_listParam.argtypes = [c_char_p, POINTER(POINTER(c_char_p)), c_uint]
_handle.ppk_removeParam.argtypes = [c_char_p, c_char_p]
_handle.ppk_setDefaultModule.argtypes = [c_char_p]
_handle.ppk_getLastError.argtypes = [c_char_p]
_handle.ppk_setCustomPromptMessage.argtypes = [c_char_p, c_char_p]
