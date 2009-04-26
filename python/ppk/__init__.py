from types import *
from constants import *
from data import *
from entry import *
from utils import *

import ctypes
_handle = ctypes.CDLL('libppasskeeper.so.1')

_handle.ppk_getAvailableModulesCount.restype = ctypes.c_uint
_handle.ppk_getAvailableModules.restype = ctypes.c_uint
_handle.ppk_readFlagsAvailable.restype = ctypes.c_uint
_handle.ppk_writeFlagsAvailable.restype = ctypes.c_uint
_handle.ppk_listingFlagsAvailable.restype = ctypes.c_uint
_handle.ppk_getEntryListCount.restype = ctypes.c_uint
_handle.ppk_getEntryList.restype = ctypes.c_uint
_handle.ppk_maxDataSize.restype = ctypes.c_uint
_handle.ppk_getLastError.restype = ctypes.c_char_p
_handle.ppk_getDefaultModule.restype = ctypes.c_char_p
_handle.ppk_settingDirectory.restype = ctypes.c_char_p

_handle.ppk_setPassword.argtypes = [ctypes.c_char_p]
_handle.ppk_unlock.argtypes = [ctypes.c_char_p]
_handle.ppk_getAvailableModules.argtypes = [ctypes.POINTER(StructModule), ctypes.c_uint]
_handle.ppk_moduleAvailable.argtypes = [ctypes.c_char_p]
_handle.ppk_readFlagsAvailable.argtypes = [ctypes.c_char_p]
_handle.ppk_writeFlagsAvailable.argtypes = [ctypes.c_char_p]
_handle.ppk_listingFlagsAvailable.argtypes = [ctypes.c_char_p]
_handle.ppk_getEntry.argtypes = [ctypes.c_char_p, StructEntry, ctypes.POINTER(StructData), ctypes.c_uint]
_handle.ppk_setEntry.argtypes = [ctypes.c_char_p, StructEntry, StructData, ctypes.c_uint]
_handle.ppk_removeEntry.argtypes = [ctypes.c_char_p, StructEntry, ctypes.c_uint]
_handle.ppk_getEntryListCount.argtypes = [ctypes.c_char_p, ctypes.c_uint, ctypes.c_uint]
_handle.ppk_getEntryList.argtypes = [ctypes.c_char_p, ctypes.c_uint, ctypes.POINTER(StructEntry), ctypes.c_uint, ctypes.c_uint]
_handle.ppk_entryExists.argtypes = [ctypes.c_char_p, StructEntry, ctypes.c_uint]
_handle.ppk_maxDataSize.argtypes = [ctypes.c_char_p, ctypes.c_int]
_handle.ppk_isWritable.argtypes = [ctypes.c_char_p]
_handle.ppk_securityLevel.argtypes = [ctypes.c_char_p]
_handle.ppk_saveParam.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
_handle.ppk_getParam.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_size_t]
_handle.ppk_listParam.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p)), ctypes.c_uint]
_handle.ppk_removeParam.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
_handle.ppk_setDefaultModule.argtypes = [ctypes.c_char_p]
_handle.ppk_getLastError.argtypes = [ctypes.c_char_p]
_handle.ppk_setCustomPromptMessage.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
