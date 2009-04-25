from constants import DataType

class Data:
    def __init__(self):
        set_string('')
    def set_string(self, s):
        self.type = DataType.string
        self.data = str(s)
    def set_blob(self, b):
        self.type = DataType.blob
        self.data = bytearray(b) # python >= 2.6 only !
    def __str__(self):
        return self.data
