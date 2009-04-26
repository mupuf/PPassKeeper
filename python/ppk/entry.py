from constants import EntryType
from types import StructNetworkEntry, StructApplicationEntry, StructEntry

class NetworkEntry:
    def __init__(self, host, login, port):
        self.host, self.login, self.port = host, login, port
    def __str__(self):
        return '%s@%s:%s' % (self.login, self.host, self.port)
    def type(self):
        return EntryType.network
    def _to_struct(self):
        s = StructEntry()
        s.type = EntryType.network
        s.u.net.host, s.u.net.login, s.u.net.port = self.host, self.login, self.port
        return s

class ApplicationEntry:
    def __init__(self, app_name, username):
        self.app_name, self.username = app_name, username
    def __str__(self):
        return '%s@%s' % (self.username, self.app_name)
    def type(self):
        return EntryType.application
    def _to_struct(self):
        s = StructEntry()
        s.type = EntryType.application
        s.u.app.app_name, s.u.app.username = self.app_name, self.username
        return s

class ItemEntry:
    def __init__(self, item):
        self.item = item
    def __str__(self):
        return self.item
    def type(self):
        return EntryType.item
    def _to_struct(self):
        s = StructEntry()
        s.type = EntryType.item
        s.u.item = self.item
        return s

def _struct_to_entry(struct):
    if struct.type == EntryType.network:
        return NetworkEntry(struct.u.net.host, struct.u.net.login, struct.u.net.port)
    elif struct.type == EntryType.application:
        return ApplicationEntry(struct.u.app.app_name, struct.u.app.username)
    elif struct.type == EntryType.item:
        return ItemEntry(struct.u.item)
    return None
