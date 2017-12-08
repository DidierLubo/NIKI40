
"""Enum-like data."""

class Enum():
    """Provides three fields:

    list, a list of name/value pairs
    index_map, a dict mapping names to their index in list
    value_map, a dict mapping names to their values."""

    def __init__(self,list):
        self.list = list
        self.index_map = { k:i for i,(k,_) in enumerate(list) }
        self.value_map = { k:v for k,v in list }

