# Taken from http://docs.python.org/3.4/howto/sorting.html


class _LTDummyKey(object):
    def __init__(self, c_tree, item):
        self._c_tree, self._item = c_tree, item

    def __lt__(self, other):
        return self._c_tree.lt_keys(self._item, other._item)

    def __gt__(self, other):
        return self._c_tree.lt_keys(other._item, self._item)

    def __eq__(self, other):
        return not self.__lt__(other) and not other.__lt__(self)
    
    def __le__(self, other):
        return not other.__lt__(self)
    
    def __ge__(self, other):
        return not self.__lt__(other)

    def __ne__(self, other):
        return not self.__eq__(other)


class _CmpDummyKey(object):
    def __init__(self, compare, item):
        self._compare, self._item = compare, item

    def __lt__(self, other):
        return self._compare(self._item, other._item) < 0

    def __gt__(self, other):
        return self._compare(self._item, other._item) > 0

    def __eq__(self, other):
        return self._compare(self._item, other._item) == 0
    
    def __le__(self, other):
        return self._compare(self._item, other._item) <= 0
    
    def __ge__(self, other):
        return self._compare(self._item, other._item) >= 0

    def __ne__(self, other):
        return self._compare(self._item, other._item) == 0

