from __future__  import print_function
from banyan_c import SetTree
from banyan_c import TreeView
from ._common_base import _CommonInitInfo
from ._common_base import _updator_metadata
from ._common_base import _adopt_updator_methods
from ._common_base import RED_BLACK_TREE
from ._common_base import SPLAY_TREE
from ._common_base import SORTED_LIST
from ._views import KeysView
from ._node import Node


class SortedSet(SetTree):    
    """
    A sorted set.
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
        :param items: Sequence of initial items 
        :type items: iterable or ``None``        
        :param key_type: Optional keys' type, or ``None`` to show it is unknown or undefined 
            (specifying the key type can greatly improve performance).
        :type key_type: ``int``, ``float``, ``str``, ``bytes``, ``unicode`` (for pre Py3)   , ``(int, int)``, ``(float, float)``, or ``None``
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

        Examples:            

        >>> # Red-black tree sorted set
        >>> t = SortedSet()
        >>>
        >>> # (Red-black tree) sorted set with initial items
        >>> t = SortedSet([1, 3, 2])
        >>> list(t)
        [1, 2, 3]
        >>>
        >>> # Splay-tree sorted set
        >>> t = SortedSet(alg = SPLAY_TREE)
        >>>
        >>> # Splay-tree larger-first sorted set with initial items.
        >>> t = SortedSet([1, 3, 2], alg = SPLAY_TREE, compare = lambda x, y: y - x)
        >>> list(t)
        [3, 2, 1]

        Key-Type Example:
        
        >>> # Almost no change!
        >>> t = SortedSet([1, 3, 2], key_type = int)
        >>> # Identical from here.
        """
                
        self._init_info = _CommonInitInfo(key_type, alg, key, compare, updator)
        metadata = _updator_metadata(True, self._init_info)
        _adopt_updator_methods(self, updator)
        SetTree.__init__(
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
        
        >>> assert SortedSet([1, 3, 2]).issubset([1, 2, 3])
        >>> assert SortedSet([1, 3, 2]) < [1, 2, 3, 4]
        >>> assert not SortedSet([1, 3, 2]) < [1, 2, 3]
        """

        return  len(self) < len(other) and self <= other           

    def issubset(self, other):
        """
        :param iterable other: A sequence of items.
        :returns: Whether this set is a subset of the set of other.
        
        Examples:
        
        >>> assert SortedSet([1, 3, 2]).issubset([1, 2, 3, 4])
        >>> assert SortedSet([1, 4, 3]).issubset([1, 4, 2, 3, 4])
        >>> assert not SortedSet([1, 3, 2]).issubset([])
        >>> assert SortedSet([1, 2, 3]).issubset([1, 2, 3])        
        >>> assert SortedSet([1, 2, 3]) <= [1, 2, 3, 4]
        >>> assert SortedSet([1, 2, 3]) <= [1, 4, 3, 2]
        """
        
        return SetTree._ext_cmp(self, other, 0)

    def __le__(self, other):
        """
        :param iterable other: A sequence of items.
        
        :returns: Whether this set is a subset of the set of other.
        
        Examples:
        
        >>> assert SortedSet([1, 3, 2]).issubset([1, 2, 3, 4])
        >>> assert SortedSet([1, 4, 3]).issubset([1, 4, 2, 3, 4])
        >>> assert not SortedSet([1, 3, 2]).issubset([])
        >>> assert SortedSet([1, 2, 3]).issubset([1, 2, 3])        
        >>> assert SortedSet([1, 2, 3]) <= [1, 2, 3, 4]
        >>> assert SortedSet([1, 2, 3]) <= [1, 4, 3, 2]
        """

        return self.issubset(other)                        

    def isdisjoint(self, other):
        """
        Checks whether there are no common items in this set and other.
        
        :type other: iterable
        
        Examples:
        >>> assert SortedSet([1, 3, 2]).isdisjoint([42])
        """
        
        return SetTree._ext_cmp(self, other, 3)
                        
    def issuperset(self, other):
        """
        :param iterable other: A sequence of items.
        :returns: Whether this set is a superset of the set of other.
        
        Examples:
        
        >>> assert not SortedSet([1, 3, 2]).issuperset([1, 2, 3, 4])
        >>> assert not SortedSet([1, 4, 3]).issuperset([1, 4, 2, 3, 4])
        >>> assert SortedSet([1, 3, 2]).issuperset([])
        >>> assert SortedSet([1, 2, 3]).issuperset([1, 2, 3])        
        >>> assert not SortedSet([1, 2, 3]) >= [1, 2, 3, 4]
        >>> assert not SortedSet([1, 2, 3]) >= [1, 4, 3, 2]
        """
        
        return SetTree._ext_cmp(self, other, 1)
        
    def __ge__(self, other):
        """
        :param iterable other: A sequence of items.
        :returns: Whether this set is a superset of the set of other.
        
        Examples:
        
        >>> assert not SortedSet([1, 3, 2]).issuperset([1, 2, 3, 4])
        >>> assert not SortedSet([1, 4, 3]).issuperset([1, 4, 2, 3, 4])
        >>> assert SortedSet([1, 3, 2]).issuperset([])
        >>> assert SortedSet([1, 2, 3]).issuperset([1, 2, 3])        
        >>> assert not SortedSet([1, 2, 3]) >= [1, 2, 3, 4]
        >>> assert not SortedSet([1, 2, 3]) >= [1, 4, 3, 2]
        """

        return self.issuperset(other)            
        
    def __gt__(self, other):
        """
        :param iterable other: A sequence of items.
        :returns: Whether this set is a proper superset of the set of other.
        
        Examples:
        
        >>> assert SortedSet([1, 3, 2]).issuperset([1, 2, 3])
        >>> assert SortedSet([1, 3, 2, 4]) > [1, 2, 3]
        >>> assert not SortedSet([1, 3, 2]) > [1, 2, 3]
        """

        return  len(self) > len(other) and self >= other           
        
    def __eq__(self, other):
        """
        :param iterable other: A sequence of items.
        :returns: Whether this set is equal to the set of other.
        
        Examples:
        
        >>> assert SortedSet([1, 3, 2]) == [1, 2, 3]
        """
            
        return SetTree._ext_cmp(self, other, 2)
        
    def __ne__(self, other):
        """
        :param iterable other: A sequence of items.
        :returns: Whether this set is unequal to the set of other.
        
        Examples:
        
        >>> assert SortedSet([1, 3, 2]) != [1, 2, 3, 4]
        >>> assert not SortedSet([1, 3, 2]) != [1, 2, 3]
        """

        return not self == other    
        
    def union(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        :returns: The union of the elements in this set and those in others.                

        Example:
        
        >>> SortedSet([1, 3, 2]).union([20])
        SortedSet([1, 2, 3, 20])
        >>> SortedSet([1, 3, 2]).union([20], [30, 3])
        SortedSet([1, 2, 3, 20, 30])
        """
        
        ret = self
        for other in others:
            ret |= other
        return ret           
        
    def update(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        
        Update this set to the union of its own items and those in others.

        Example:
        
        >>> t = SortedSet([1, 3, 2])
        >>> t.update([20])        
        >>> t
        SortedSet([1, 2, 3, 20])
        >>> 
        >>> t = SortedSet([1, 3, 2])
        >>> t.update([20], [30, 3])
        >>> t
        SortedSet([1, 2, 3, 20, 30])
        """
        
        t = self.union(*others)
        SetTree.__init__(
            self,
            self._init_info.alg,
            t,
            self._init_info.key_type,
            1,
            _updator_metadata(1, self._init_info),
            self._init_info.key,
            self._init_info.compare,
            1)                
        
    def __or__(self, other):
        """
        :param iterable other: Other items.
        :returns: The union of the items in this set and those in other. 
        
        Example:
        
        >>> SortedSet([1, 3, 2]) | [20]
        SortedSet([1, 2, 3, 20])
        """
        
        return self.__class__(
            SetTree._ext_union(self, other, 0),
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
        
        >>> SortedSet([1, 3, 2]).intersection([3, 1])
        SortedSet([1, 3])
        >>> SortedSet([1, 3, 2]).intersection([3], [1])
        SortedSet([])
        """
        
        ret = self
        for other in others:
            ret &= other
        return ret            
        
    def intersection_update(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        
        Update this set to the intersection of its own items and those in others.

        Example:
        
        >>> t = SortedSet([1, 3, 2])
        >>> t.intersection_update([20])
        >>> t
        SortedSet([])
        >>> t = SortedSet([1, 3, 2])
        >>> t.intersection_update([1, 3], [3])
        >>> t
        SortedSet([3])
        """

        t = self.intersection(*others)
        SetTree.__init__(
            self,
            self._init_info.alg,
            t,
            self._init_info.key_type,
            1,
            _updator_metadata(1, self._init_info),
            self._init_info.key,
            self._init_info.compare,
            1)                

    def __and__(self, other):
        """
        :param iterable other: Other items.
        :returns: The intersection of the items in this set and those in other. 
        
        Example:
        
        >>> SortedSet([1, 3, 2]) & [3, 1]
        SortedSet([1, 3])
        """
        
        return self.__class__(
            SetTree._ext_union(self, other, 1),
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
        
        >>> SortedSet([1, 3, 2]).difference([3], [1])
        SortedSet([2])
        """
        
        ret = self
        for other in others:
            ret -= other
        return ret            
        
    def difference_update(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        
        Update this set to the difference of its own items and those in others.

        Example:
        
        >>> t = SortedSet([1, 3, 2])
        >>> t.difference_update([20])
        >>> t
        SortedSet([1, 2, 3])
        """

        t = self.difference(*others)
        SetTree.__init__(
            self,
            self._init_info.alg,
            t,
            self._init_info.key_type,
            1,
            _updator_metadata(1, self._init_info),
            self._init_info.key,
            self._init_info.compare,
            1)                

    def __sub__(self, other):
        """
        :param iterable other: Other items.
        :returns: The difference of the items in this set and those in other. 
        
        Example:
        
        >>> SortedSet([1, 3, 2]) - [3, 1]
        SortedSet([2])
        """
        
        return self.__class__(
            SetTree._ext_union(self, other, 2),
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
        
        >>> SortedSet([1, 3, 2]).symmetric_difference([4, 5, 1], [1])
        SortedSet([1, 2, 3, 4, 5])
        """
        
        ret = self
        for other in others:
            ret ^= other
        return ret            
        
    def symmetric_difference_update(self, *others):
        """
        :param iterable others: Other elemens.
        :type others: iterable or multiple iterables
        
        Update this set to the symmetric difference of its own items and those in others.

        Example:
        
        >>> t = SortedSet([1, 3, 2])
        >>> t.symmetric_difference_update([2, 5])
        >>> t
        SortedSet([1, 3, 5])
        """

        t = self.symmetric_difference(*others)
        SetTree.__init__(
            self,
            self._init_info.alg,
            t,
            self._init_info.key_type,
            1,
            _updator_metadata(1, self._init_info),
            self._init_info.key,
            self._init_info.compare,
            1)                

    def __xor__(self, other):
        """
        :param iterable other: Other items.
        :returns: The symmetric difference of the items in this set and those in other. 
        
        Example:
        
        >>> SortedSet([1, 3, 2]) ^ [3, 1, 5]
        SortedSet([2, 5])
        """
        
        return self.__class__(
            SetTree._ext_union(self, other, 3),
            self._init_info.key_type,
            self._init_info.alg,
            self._init_info.key,
            self._init_info.compare,
            self._init_info.updator)

    def __repr__(self):
        """
        Example:
        
        >>> SortedSet([-1, 3, 2])
        SortedSet([-1, 2, 3])
        """
        return 'SortedSet([' + ', '.join(e.__repr__() for e in self) + '])'                

    def __reduce__(self):
        """
        Pickle support.
        
        Example:
        
        >>> import pickle
        >>>
        >>> t = SortedSet([2, 1, 3])
        >>> t.add(4)
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
                        
    def f(self):
        """
        :returns: A shallow copy.
        
        Example:
        
        >>> t = SortedSet([1, 3, 2])
        >>> t1 = t.copy()
        >>> assert t == t1
        """
        
        return SortedSet(        
            self, 
            self._init_info.key_type,
            self._init_info.alg,
            self._init_info.key,
            self._init_info.compare,
            self._init_info.updator)                
        
    def keys(self, *args, **kwargs):        
    
        """                   
        :param start: optional parameter indicating, if given, the smallest element
            in the view (default ``None``).
        :param stop: optional parameter, indicating, the smallest element that should
            be larger than all keys in the view (default ``None``).
        :param boolean reverse: Whether to iterate in reverse order (default ``False``).
        :returns: A dynamic :py:class:`KeysView` view of the set's keys.
        
        Example:
        
        >>> t = SortedSet([1, 2])
        >>> v = t.keys()
        >>> v
        KeysView([1, 2])
        >>> t.remove(1)
        >>> v
        KeysView([2])
        
        Example using start and stop options:
        
        >>> t = SortedSet([1, 2, 3, 4])
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
        :returns: The root :py:class:`Node` of the tree.
        """
        
        c_node = SetTree.root(self)
        return Node(c_node) if c_node is not None else None    
                    
    def __iter__(self):
        """
        Iterates over all keys. 
        
        Example:
        
        >>> t = SortedSet([2, 1, 3])
        >>> for e in t:
        ...     print(e)
        ... 
        1
        2
        3

        .. Warning:: While iterating over a mapping (either directly or through a view), the mapping should not 
            be modified - the behaviour is undefined in this case. A different alternative should be found. 
            For example: in order to erase the even keys of a set ``t``, instead of using a loop:

            >>> # Example of badness!
            >>> for e in t:
            ...     if e % 2:        
            ...         t.remove(e)
                
            use comprehension:        
                
            >>> t = SortedSet([k for k in t if k % 2 == 1]) 
                
            which is more efficient computationally anyway.                
        """
                
        return TreeView(self, 0, None, 0, None, 1, 0)       
        
    def copy(self):
        """
        :returns: A shallow copy.
        
        Example:
        
        >>> t = SortedSet([1, 3, 2])
        >>> t1 = t.copy()
        >>> assert t == t1
        """
        
        return SortedSet(        
            self, 
            self._init_info.key_type,
            self._init_info.alg,
            self._init_info.key,
            self._init_info.compare,
            self._init_info.updator)                

