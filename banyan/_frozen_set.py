from banyan_c import FrozenSetTree
from banyan_c import TreeView
from ._common_base import _CommonInitInfo
from ._common_base import _updator_metadata
from ._common_base import _adopt_updator_methods
from ._common_base import RED_BLACK_TREE
from ._common_base import SPLAY_TREE
from ._common_base import SORTED_LIST
from ._views import KeysView
from ._node import Node


class FrozenSortedSet(FrozenSetTree):    
    """
    An immutable sorted set.
    """
    
    def __init__(
            self, 
            items = None, 
            key_type = None,
            alg = SORTED_LIST, 
            key = None,
            compare = None, 
            updator = None):
            
        """
        :param items: Sequence of initial items 
        :type items: iterable or ``None``        
        :param key_type: Optional keys' type, or ``None`` to show it is unknown or undefined 
            (specifying the key type can greatly improve performance).
        :type key_type: ``int``, ``float``, ``str``, ``bytes``, ``unicode`` (for pre Py3)   , ``(int, int)``, ``(float, float)``, or ``None``
        :param string alg: Underlying algorithm string. Should be one of 
            RED_BLACK_TREE, SPLAY_TREE, or SORTED_LIST.  
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

        Examples:            

        >>> # (Sorted-list) frozen sorted set with initial items
        >>> t = FrozenSortedSet([1, 3, 2])
        >>> list(t)
        [1, 2, 3]
        >>> assert 1 in t
        >>> assert 4 not in t
        >>> t.add(130)
        Traceback (most recent call last):
            ...
        AttributeError: 'FrozenSortedSet' object has no attribute 'add'

        Key-Type Example:
        
        >>> # Almost no change!
        >>> t = FrozenSortedSet([1, 3, 2], key_type = int)
        >>> # Identical from here.
        """

        self._init_info = _CommonInitInfo(key_type, alg, key, compare, updator)
        metadata = _updator_metadata(True, self._init_info)
        _adopt_updator_methods(self, updator)
        FrozenSetTree.__init__(
            self,
            alg,
            items,
            key_type,
            1,
            metadata,
            key,
            compare,
            0)                

    def __lt__(self, other):
        """
        :param iterable other: A sequence of items.
        :returns: Whether this set is a proper subset of the set of other.
        
        Examples:
        
        >>> assert FrozenSortedSet([1, 3, 2]).issubset([1, 2, 3])
        >>> assert FrozenSortedSet([1, 3, 2]) < [1, 2, 3, 4]
        >>> assert not FrozenSortedSet([1, 3, 2]) < [1, 2, 3]
        """

        return  len(self) < len(other) and self <= other           

    def issubset(self, other):
        """
        :param other: A sequence of items.
        :type other: iterable
        :returns: Whether this set is a subset of the set of other.
        
        Examples:
        
        >>> assert FrozenSortedSet([1, 3, 2]).issubset([1, 2, 3, 4])
        >>> assert FrozenSortedSet([1, 4, 3]).issubset([1, 4, 2, 3, 4])
        >>> assert not FrozenSortedSet([1, 3, 2]).issubset([])
        >>> assert FrozenSortedSet([1, 2, 3]).issubset([1, 2, 3])        
        >>> assert FrozenSortedSet([1, 2, 3]) <= [1, 2, 3, 4]
        >>> assert FrozenSortedSet([1, 2, 3]) <= [1, 4, 3, 2]
        """
        
        return FrozenSetTree._ext_cmp(self, other, 0)

    def __le__(self, other):
        """
        :param other: A sequence of items.
        :type others: iterable
        
        :returns: Whether this set is a subset of the set of other.
        
        Examples:
        
        >>> assert FrozenSortedSet([1, 3, 2]).issubset([1, 2, 3, 4])
        >>> assert FrozenSortedSet([1, 4, 3]).issubset([1, 4, 2, 3, 4])
        >>> assert not FrozenSortedSet([1, 3, 2]).issubset([])
        >>> assert FrozenSortedSet([1, 2, 3]).issubset([1, 2, 3])        
        >>> assert FrozenSortedSet([1, 2, 3]) <= [1, 2, 3, 4]
        >>> assert FrozenSortedSet([1, 2, 3]) <= [1, 4, 3, 2]
        """

        return self.issubset(other)                        

    def isdisjoint(self, other):
        """
        Checks whether there are no common items in this set and other.
        
        :type other: iterable
        
        Examples:
        >>> assert FrozenSortedSet([1, 3, 2]).isdisjoint([42])
        """
        
        return FrozenSetTree._ext_cmp(self, other, 3)
                        
    def issuperset(self, other):
        """
        :param other: A sequence of items.
        :type other: iterable
        :returns: Whether this set is a superset of the set of other.
        
        Examples:
        
        >>> assert not FrozenSortedSet([1, 3, 2]).issuperset([1, 2, 3, 4])
        >>> assert not FrozenSortedSet([1, 4, 3]).issuperset([1, 4, 2, 3, 4])
        >>> assert FrozenSortedSet([1, 3, 2]).issuperset([])
        >>> assert FrozenSortedSet([1, 2, 3]).issuperset([1, 2, 3])        
        >>> assert not FrozenSortedSet([1, 2, 3]) >= [1, 2, 3, 4]
        >>> assert not FrozenSortedSet([1, 2, 3]) >= [1, 4, 3, 2]
        """
        
        return FrozenSetTree._ext_cmp(self, other, 1)
        
    def __ge__(self, other):
        """
        :param other: A sequence of items.
        :type other: iterable
        :returns: Whether this set is a superset of the set of other.
        
        Examples:
        
        >>> assert not FrozenSortedSet([1, 3, 2]).issuperset([1, 2, 3, 4])
        >>> assert not FrozenSortedSet([1, 4, 3]).issuperset([1, 4, 2, 3, 4])
        >>> assert FrozenSortedSet([1, 3, 2]).issuperset([])
        >>> assert FrozenSortedSet([1, 2, 3]).issuperset([1, 2, 3])        
        >>> assert not FrozenSortedSet([1, 2, 3]) >= [1, 2, 3, 4]
        >>> assert not FrozenSortedSet([1, 2, 3]) >= [1, 4, 3, 2]
        """

        return self.issuperset(other)            
        
    def __gt__(self, other):
        """
        :param other: A sequence of items.
        :type other: iterable
        :returns: Whether this set is a proper superset of the set of other.
        
        Examples:
        
        >>> assert FrozenSortedSet([1, 3, 2]).issuperset([1, 2, 3])
        >>> assert FrozenSortedSet([1, 3, 2, 4]) > [1, 2, 3]
        >>> assert not FrozenSortedSet([1, 3, 2]) > [1, 2, 3]
        """

        return  len(self) > len(other) and self >= other           
        
    def __eq__(self, other):
        """
        :param other: A sequence of items.
        :type other: iterable
        :returns: Whether this set is equal to the set of other.
        
        Examples:
        
        >>> assert FrozenSortedSet([1, 3, 2]) == [1, 2, 3]
        """
            
        return FrozenSetTree._ext_cmp(self, other, 2)
        
    def __ne__(self, other):
        """
        :param other: A sequence of items.
        :type other: iterable
        :returns: Whether this set is unequal to the set of other.
        
        Examples:
        
        >>> assert FrozenSortedSet([1, 3, 2]) != [1, 2, 3, 4]
        >>> assert not FrozenSortedSet([1, 3, 2]) != [1, 2, 3]
        """

        return not self == other    
        
    def union(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        :returns: The union of the elements in this set and those in others.                

        Example:
        
        >>> FrozenSortedSet([1, 3, 2]).union([20])
        FrozenSortedSet([1, 2, 3, 20])
        >>> FrozenSortedSet([1, 3, 2]).union([20], [30, 3])
        FrozenSortedSet([1, 2, 3, 20, 30])
        """
        
        ret = self
        for other in others:
            ret |= other
        return ret           
        
    def __or__(self, other):
        """
        :param iterable other: Other items.
        :returns: The union of the items in this set and those in other. 
        
        Example:
        
        >>> FrozenSortedSet([1, 3, 2]) | [20]
        FrozenSortedSet([1, 2, 3, 20])
        """
        
        return self.__class__(
            FrozenSetTree._ext_union(self, other, 0),
            self._init_info.key_type,
            self._init_info.alg,
            self._init_info.key,
            self._init_info.compare,
            self._init_info.updator)
            
    def intersection(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        :returns: The intersection of the elements in this set and those in others.                

        Example:
        
        >>> FrozenSortedSet([1, 3, 2]).intersection([3, 1])
        FrozenSortedSet([1, 3])
        >>> FrozenSortedSet([1, 3, 2]).intersection([3], [1])
        FrozenSortedSet([])
        """
        
        ret = self
        for other in others:
            ret &= other
        return ret            

    def __and__(self, other):
        """
        :param iterable other: Other items.
        :returns: The intersection of the items in this set and those in other. 
        
        Example:
        
        >>> FrozenSortedSet([1, 3, 2]) & [3, 1]
        FrozenSortedSet([1, 3])
        """
        
        return self.__class__(
            FrozenSetTree._ext_union(self, other, 1),
            self._init_info.key_type,
            self._init_info.alg,
            self._init_info.key,
            self._init_info.compare,
            self._init_info.updator)

    def difference(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        :returns: The difference of the elements in this set and those in others.                

        Example:
        
        >>> FrozenSortedSet([1, 3, 2]).difference([3, 1])
        FrozenSortedSet([2])
        """
        
        ret = self
        for other in others:
            ret -= other
        return ret            

    def __sub__(self, other):
        """
        :param iterable other: Other items.
        :returns: The difference of the items in this set and those in other. 
        
        Example:
        
        >>> FrozenSortedSet([1, 3, 2]) - [3, 1]
        FrozenSortedSet([2])
        """
        
        return self.__class__(
            FrozenSetTree._ext_union(self, other, 2),
            self._init_info.key_type,
            self._init_info.alg,
            self._init_info.key,
            self._init_info.compare,
            self._init_info.updator)

    def symmetric_difference(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        :returns: The symmetric difference of the elements in this set and those in others.                

        Example:
        
        >>> FrozenSortedSet([1, 3, 2]).symmetric_difference([2, 4])
        FrozenSortedSet([1, 3, 4])
        """
        
        ret = self
        for other in others:
            ret ^= other
        return ret            

    def __xor__(self, other):
        """
        :param iterable other: Other items.
        :returns: The symmetric difference of the items in this set and those in other. 
        
        Example:
        
        >>> FrozenSortedSet([1, 3, 2]) ^ [3, 1, 5]
        FrozenSortedSet([2, 5])
        """
        
        return self.__class__(
            FrozenSetTree._ext_union(self, other, 3),
            self._init_info.key_type,
            self._init_info.alg,
            self._init_info.key,
            self._init_info.compare,
            self._init_info.updator)

    def __repr__(self):
        """
        Example:
        
        >>> FrozenSortedSet([-1, 3, 2])
        FrozenSortedSet([-1, 2, 3])
        """
        return 'FrozenSortedSet([' + ', '.join(e.__repr__() for e in self) + '])'                

    def __reduce__(self):
        """
        Pickle support.
        
        Example:
        
        >>> import pickle
        >>>
        >>> t = FrozenSortedSet([2, 1, 3])
        >>>
        >>> with open('data.pkl', 'wb') as output:
        ...     pickle.dump(t, output)
        ...
        >>> with open('data.pkl', 'rb') as input:
        ...     t1 = pickle.load(input)
        ...
        >>> assert t == t1
        """

        return (
            self.__class__, 
            (
                list(self), 
                self._init_info.key_type,
                self._init_info.alg,
                self._init_info.key,
                self._init_info.compare,
                self._init_info.updator))
                        
    def copy(self):
        """
        :returns: A shallow copy.
        
        Example:
        
        >>> t = FrozenSortedSet([1, 3, 2])
        >>> t1 = t.copy()
        >>> assert t == t1
        """
        
        return FrozenSortedSet(        
            self, 
            self._init_info.key_type,
            self._init_info.alg,
            self._init_info.key,
            self._init_info.compare,
            self._init_info.updator)                

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
        
        >>> t = FrozenSortedSet([1, 2])
        >>> v = t.keys(reverse = True)
        >>> v
        KeysView([2, 1])
        
        Example using start and stop options:
        
        >>> t = FrozenSortedSet([1, 2, 3, 4])
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
        :returns: The root :py:class:`Node` of the set.
        """
        
        c_node = FrozenSetTree.root(self)
        return Node(c_node) if c_node is not None else None    

                    
    def __iter__(self):
        """
        Iterates over all keys. 
        
        Example:
        
        >>> t = FrozenSortedSet([2, 1, 3])
        >>> for e in t:
        ...     print(e)
        ... 
        1
        2
        3
        """
                
        return TreeView(self, 0, None, 0, None, 1, 0)       

