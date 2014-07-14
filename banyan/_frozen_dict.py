from banyan_c import FrozenDictTree
from banyan_c import TreeView 
from ._common_base import _CommonInitInfo
from ._common_base import _updator_metadata
from ._common_base import _adopt_updator_methods
from ._common_base import RED_BLACK_TREE
from ._common_base import SPLAY_TREE
from ._common_base import SORTED_LIST
from ._views import ValuesView
from ._views import ItemsView
from ._views import KeysView
from ._node import Node


class FrozenSortedDict(FrozenDictTree):
    """
    An immutable sorted dictionary.
    
    .. Note::

        :pep:`0416` explicitly rejects frozen dictionaries, but, as opposed to regular
        dictionaries, sorted frozen dictionaries do have an
        algorithm particularly suited to it (sorted lists), and it's convenient to have such a 
        class with this algorithm as the default implementation. 

    """
    
    def __init__(
            self,
            items = None, 
            key_type = None,
            alg = RED_BLACK_TREE,         
            key = None,
            compare = None, 
            updator = None):
        """
        :param items: Sequence or mapping of initial items.
        :param key_type: Optional keys' type, or ``None`` to show it is unknown or undefined 
            (specifying the key type can greatly improve performance).
        :type key_type: ``int``, ``float``, ``str``, ``bytes``, ``unicode`` (for pre Py3)   , ``(int, int)``, ``(float, float)``, or ``None``
        :type items: iterable or ``None``        
        :param string alg: Underlying algorithm string. Should be one of 
            RED_BLACK_TREE, SPLAY_TREE, or SORTED_LIST  
        :param function key: Key function: transforms the set's keys into something that 
            can be compared.                               
        :param compare: Comparison function. Should take two parameters, say x and y, 
            and return the a negative number, 0, or positive number, for the cases 
            x < y, x == y, and x > y, respectively.
        :type compare: Function or ``None``
        :param updator: Node updator
        :type updator: Function or ``None``        
                
        .. Note::
            
            The compare fuction is deprecated in favor of the key function.

        Example:            

        >>> # (Sorted-list) frozen sorted dict with initial items
        >>> t = FrozenSortedDict([(1, 'a'), (2, 'b')])
        >>> list(t)
        [1, 2]
        >>> assert 1 in t
        >>> assert 4 not in t
        >>> t[130] = 'koko'
        Traceback (most recent call last):
          File "<stdin>", line 1, in <module>
        TypeError: 'FrozenSortedDict' object does not support item assignment
        
        Key-Type Example:
        
        >>> # Almost no change!
        >>> t = FrozenSortedDict([(1, 'a'), (2, 'b')], key_type = int)
        >>> # Identical from here.
        """
        
        try:
            items = items.items()
        except AttributeError:
            pass            
        
        self._init_info = _CommonInitInfo(key_type, alg, key, compare, updator)
        metadata = _updator_metadata(0, self._init_info)
        _adopt_updator_methods(self, updator)
        FrozenDictTree.__init__(
            self,
            alg,
            items,
            key_type,
            0,
            metadata,
            key,
            compare,
            0)                
        
    @classmethod            
    def fromkeys(
            cls,
            keys, 
            value = None,
            key_type = None,
            alg = SORTED_LIST,         
            key = None,
            compare = None,             
            updator = None):
        """
        Creates a frozen sorted dict from keys and a value.

        :param keys: Sequence of keys.
        :param value: Value mapped to the keys
        :param key_type: Optional keys' type, or ``None`` to show it is unknown or undefined 
            (specifying the key type can greatly improve performance).
        :type key_type: type or ``None``
        :param string alg: Underlying algorithm string. Should be one of 
            RED_BLACK_TREE, SPLAY_TREE, or SORTED_LIST  
        :param function key: Key function: transforms the set's keys into something that 
            can be compared.                               
        :param compare: Comparison function. Should take two parameters, say x and y, 
            and return the a negative number, 0, or positive number, for the cases 
            x < y, x == y, and x > y, respectively.
        :type compare: function or ``None``
        :param updator: Node updator
        :type updator: function or ``None``        
        
        .. Note::
            
            The compare fuction is deprecated in favor of the key function.

        Example:
        
        >>> t = FrozenSortedDict.fromkeys([1, 2], 'b')
        >>> assert t[1] == t[2] == 'b'
        """            
            
        return FrozenSortedDict(
            [(k, value) for k in keys],
            key_type,    
            alg,
            key,
            compare,
            updator)            

    def get(self, key, default = None):
        """
        :param key: Key
        :param default: default value
        :returns: default if key is not in the dict, otherwise value mapped by key. 

        Examples:            

        >>> t = SortedDict([(2, 'b'), (3, 'c')])
        >>> assert t.get(2, 'a') == 'b'
        >>> assert t.get(0, 'a') == 'a'
        """
        
        try:
            return FrozenDictTree.__getitem__(self, key)
        except KeyError:
            return default            

    def items(self, *args, **kwargs):        
        """
        :param start: optional parameter indicating, if given, the smallest key
            of the values in the view (default ``None``).
        :param stop: optional parameter, indicating, the smallest key that should
            be larger than all of the keys corresponding to the values in the view (default ``None``).
        :param bool reverse: Whether to iterate in reverse order.
        :returns: A dynamic :py:class:`ItemsView` view of the dict's items (default ``False``).
        
        Example:
        
        >>> t = SortedDict([(1, 'a'), (2, 'b')])
        >>> v = t.items()
        >>> v
        ItemsView([(1, 'a'), (2, 'b')])
        >>> del t[1]
        >>> v
        ItemsView([(2, 'b')])
        >>>
        >>> t = SortedDict([(1, 'a'), (2, 'b')])
        >>> v = t.items(reverse = True)
        >>> v
        ItemsView([(2, 'b'), (1, 'a')])
        >>> del t[1]
        >>> v
        ItemsView([(2, 'b')])
        
        Examples using start and stop options:
        
        >>> t = SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')])
        >>>
        >>> v = t.items()
        >>> v
        ItemsView([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')])
        >>>
        >>> v = t.items(3)
        >>> v
        ItemsView([(1, 'a'), (2, 'b')])
        >>>
        >>> v = t.items(3, reverse = True)
        >>> v
        ItemsView([(2, 'b'), (1, 'a')])
        >>>
        >>> v = t.items(0, 3)
        >>> v
        ItemsView([(1, 'a'), (2, 'b')])
        >>>
        >>> v = t.items(0, 23)
        >>> v
        ItemsView([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')])
        >>>
        >>> v = t.items(2.5)
        >>> v
        ItemsView([(1, 'a'), (2, 'b')])
        >>>
        >>> v = t.items(2.5, 23)
        >>> v
        ItemsView([(3, 'c'), (4, 'd')])
        >>>
        """

        return ItemsView(self, *args, **kwargs)

    def values(self, *args, **kwargs):        
        """
        :param start: optional parameter indicating, if given, the smallest key
            of the values in the view (default ``None``).
        :param stop: optional parameter, indicating, the smallest key that should
            be larger than all of the keys corresponding to the values in the view (default ``None``).
        :param bool reverse: Whether to iterate in reverse order (default ``False``).
        :returns: A dynamic :py:class:`ValuesView` view of the dict's values.
        
        Example:
        
        >>> t = SortedDict([(1, 'a'), (2, 'b')])
        >>> v = t.values()
        >>> v
        ValuesView(['a', 'b'])
        >>> del t[1]
        >>> v
        ValuesView(['b'])
        >>>
        >>> t = SortedDict([(1, 'a'), (2, 'b')])
        >>> v = t.values(reverse = True)
        >>> v
        ValuesView(['b', 'a'])
        >>> del t[1]
        >>> v
        ValuesView(['b'])
        
        Examples using start and stop options:
        
        >>> t = SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')])
        >>>
        >>> v = t.values()
        >>> v
        ValuesView(['a', 'b', 'c', 'd'])
        >>>
        >>> v = t.values(3)
        >>> v
        ValuesView(['a', 'b'])
        >>>
        >>> v = t.values(3, reverse = True)
        >>> v
        ValuesView(['b', 'a'])
        >>>
        >>> v = t.values(0, 3)
        >>> v
        ValuesView(['a', 'b'])
        >>>
        >>> v = t.values(0, 23)
        >>> v
        ValuesView(['a', 'b', 'c', 'd'])
        >>>
        >>> v = t.values(2.5)
        >>> v
        ValuesView(['a', 'b'])
        >>>
        >>> v = t.values(2.5, 23)
        >>> v
        ValuesView(['c', 'd'])
        """
                
        return ValuesView(self, *args, **kwargs)

    def __reduce__(self):
        """
        Pickle support.
        
        Example:
        
        >>> import pickle
        >>> 
        >>> t = SortedDict([(2, 'b')])
        >>>
        >>> with open('data.pkl', 'wb') as output:
        ...     pickle.dump(t, output)
        ...
        >>> with open('data.pkl', 'rb') as input:
        ...     t1 = pickle.load(input)
        ...
        >>> assert list(t) == list(t1)
        """
        
        return (
            self.__class__, 
            (
                list(self.items()), 
                self._init_info.key_type,
                self._init_info.alg,
                self._init_info.key,
                self._init_info.compare,
                self._init_info.updator))

    def __repr__(self):
        return self._repr('FrozenSortedDict')
    
    def _repr(self, class_name):
        def _inner_repr(e):
            return e[0].__repr__() + ': ' + e[1].__repr__()
        return class_name + '({' + ', '.join(_inner_repr(e) for e in self.items()) + '})'                

    def __hash__(self):
        """
        :returns: Hash value based on the contents.
        """

        h = 0
        for item in self:
            h = 101 * h + hash(item)
        return h            
        
        
    def keys(self, *args, **kwargs):        
        """           
        
        :param start: optional parameter indicating, if given, the smallest element
            in the view (default ``None``).
        :param stop: optional parameter, indicating, the smallest element that should
            be larger than all keys in the view (default ``None``).
        :param boolean reverse: Whether to iterate in reverse order (default ``False``).
        :returns: A dynamic :py:class:`KeysView` view of the set's keys.
        
        Example:
        
        >>> t = SortedDict([(1, 'a'), (2, 'b')])
        >>> v = t.keys()
        >>> v
        KeysView([1, 2])
        >>> del t[1]
        >>> v
        KeysView([2])
        >>>
        >>> t = SortedDict([(1, 'a'), (2, 'b')])
        >>> v = t.keys(reverse = True)
        >>> v
        KeysView([2, 1])
        
        Example using start and stop options:
        
        >>> t = SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')])
        >>>
        >>> v = t.keys()
        >>> v
        KeysView([1, 2, 3, 4])
        >>>
        >>> v = t.keys(3)
        >>> v
        KeysView([1, 2])
        >>>
        >>> v = t.keys(3, reverse = True)
        >>> v
        KeysView([2, 1])
        >>>
        >>> v = t.keys(0, 3)
        >>> v
        KeysView([1, 2])
        >>>
        >>> v = t.keys(0, 23)
        >>> v
        KeysView([1, 2, 3, 4])
        >>>
        >>> v = t.keys(2.5)
        >>> v
        KeysView([1, 2])
        """
                
        return KeysView(self, *args, **kwargs)    

    @property    
    def root(self):
        """
        :returns: The root :py:class:`Node` of the dict.
        """
        
        c_node = FrozenDictTree.root(self)
        return Node(c_node) if c_node is not None else None    

                    
    def __iter__(self):
        """
        Iterates over all keys. 
        
        Example:
        
        >>> t = SortedDict([(2, 'b'), (3, 'c')])
        >>> for e in t:
        ...     print(e)
        ... 
        2
        3
        """
                
        return TreeView(self, 0, None, 0, None, 1, 0)       

