from __future__ import print_function
import sys
import collections
import itertools

from banyan_c import TreeView


class _View(object):
    def __init__(self, _tree, type_, name, *args, **kwargs):

        self._stop_valid, self._stop = False, None
        if 'stop' in kwargs:
            assert len(args) != 1
            self._stop_valid, self._stop = True, kwargs['stop']
        elif len(args) == 1:
            assert len(args)
            self._stop_valid, self._stop = True, args[0]
        elif len(args) > 1:
            assert len(args)
            self._stop_valid, self._stop = True, args[1]
        if self._stop_valid and self._stop is None:
            self._stop_valid = False            

        self._start_valid, self._start = False, None
        if 'start' in kwargs:
            self._start_valid, self._start = True, kwargs['start']
        elif len(args) > 1:
            self._start_valid, self._start = True, args[0]
        if self._start_valid and self._start is None:
            self._start_valid = False            

        self._forward = True
        if 'reverse' in kwargs:
            self._forward = not kwargs['reverse']
        elif len(args) == 3:
            self._forward = not args[2]
            
        self._tree, self._type_, self._name,  = _tree, type_, name
        
    def __iter__(self):
        """
        Iterates over all items in the view.
        """
        
        return TreeView(
            self._tree, 
            int(self._start_valid), self._start,
            int(self._stop_valid), self._stop,
            int(self._forward), 
            self._type_)                       
        
    def __len__(self):
        """
        :returns: Number of items in the view.
        """
        
        if not self._start_valid and not self._stop_valid:
            return len(self._tree) 
            
        return sum(1 for _ in self)            
        
    def __contains__(self, key):
        """
        :returns: Whether key is in the view.
        """
        
        if key not in self._tree:
            return False
            
        key_fn = self._tree.root.key_fn
        
        if self._start_valid and key_fn(key) < key_fn(self._start):
            return False
        if self._stop_valid and key_fn(self._stop) <= key_fn(key):
            return False
            
        return True                                 
                
    def __repr__(self):
        rep = self._name
            
        rep += '(['
        
        def _inner_repr(e):
            if self._type_ in [0, 1]:
                return e.__repr__()
                
            assert self._type_ == 2                
            return '(' + e[0].__repr__() + ', ' + e[1].__repr__() + ')'
            
        rep += ', '.join(_inner_repr(e) for e in self)
        
        rep += '])'            
            
        return rep            
                        
        
class KeysView(_View, collections.KeysView):
    def __init__(self, _tree, *args, **kwargs):
        _View.__init__(self, _tree, 0, 'KeysView', *args, **kwargs)

    @classmethod
    def _from_iterable(self, it):
        # Tmp Ami
        raise NotImplementedError


class ValuesView(_View, collections.ValuesView):
    def __init__(self, _tree, *args, **kwargs):
        _View.__init__(self, _tree, 1, 'ValuesView', *args, **kwargs)


class ItemsView(_View, collections.ItemsView):
    def __init__(self, _tree, *args, **kwargs):
        _View.__init__(self, _tree, 2, 'ItemsView', *args, **kwargs)

    def __contains__(self, item):
        """
        :returns: Whether v is in the view.
        """
        
        return _View.__contains__(self, item[0]) and item[1] == self._tree[item[0]]

    @classmethod
    def _from_iterable(self, it):
        # Tmp Ami
        raise NotImplementedError


