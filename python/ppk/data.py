from constants import DataType
from types import StructData

class StringData:
    def __init__(self, string):
        self.string = str(string)
    def __str__(self):
        return self.string
    def __len__(self):
        return len(self.string)
    def data(self):
        return self.string
    def type(self):
        return DataType.string
    def _to_struct(self):
        s = StructData()
        s.type = DataType.string
        s.u.string = self.string
        return s

class BlobData:
    def __init__(self, blob):
        self.blob = bytearray(blob)
    def __len__(self):
        return len(self.blob)
    def data(self):
        return self.blob
    def type(self):
        return DataType.blob
    def _to_struct(self):
        s = StructData()
        s.type = DataType.blob
        s.u.blob.data = self.blob
        s.u.blob.size = len(self.blob)
        return s

def _struct_to_data(struct):
    if struct.type == DataType.string:
        return StringData(struct.u.string)
    elif struct.type == DataType.blob:
        return BlobData(struct.u.blob)
    return None
