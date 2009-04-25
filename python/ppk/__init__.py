from ctypes import CDLL, c_uint, c_char_p

_handle = CDLL('libppasskeeper.so.1')

_handle.ppk_getAvailableModulesCount.restype = c_uint
_handle.ppk_readFlagsAvailable.restype = c_uint
_handle.ppk_writeFlagsAvailable.restype = c_uint
_handle.ppk_listingFlagsAvailable.restype = c_uint
_handle.ppk_getDefaultModule.restype = c_char_p
_handle.ppk_settingDirectory.restype = c_char_p
_handle.ppk_getLastError.restype = c_char_p
_hanle.ppk_maxDataSize.restype = c_uint
