from constants import EntryType

class NetworkEntry:
    def __init__(self, host, login, port):
        self.type = EntryType.network
        self.host, self.login, self.port = host, login, port
    def __str__(self):
        return '%s@%s:%s' % (self.login, self.host, self.port)

class ApplicationEntry:
    def __init__(self, app_name, username):
        self.type = EntryType.application
        self.app_name, self.username = app_name, username
    def __str__(self):
        return '%s@%s' % (self.username, self.app_name)

class ItemEntry:
    def __init__(self, item):
        self.type = EntryType.item
        self.item = item
    def __str__(self):
        return self.item
