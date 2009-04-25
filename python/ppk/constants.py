PPK_PARAM_MAX = 4096

class SecurityLevel:
    lowest, scrambled, safe, perfect = (0, 1, 2, 3)

class EntryType:
    network, application, item = (1, 2, 4)

class DataType:
    string, blob = (1, 2)

class ReadFlag:
    none, silent = (0, 1)

class WriteFlag:
    none, silent = (0, 1)

class ListingFlag:
    none, silent = (0, 1)
