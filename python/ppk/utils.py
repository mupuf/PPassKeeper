from __init__ import _handle

from module import ModuleList

def is_locked():
    return bool(_handle.ppk_isLocked())

def set_password(pwd):
    return bool(_handle.ppk_setPassword(pwd))

def unlock(pwd):
    return bool(_handle.ppk_unlock(pwd))

def set_default_module(module):
    return bool(_handle.ppk_setDefaultModule(module.id))

def get_default_module():
    return ModuleList()[_handle.ppk_getDefaultModule()]

def setting_directory():
    return _handle.ppk_settings_directory()
