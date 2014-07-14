============
Introduction
============

This package provides sorted drop-in versions of Python's `set`_ and `dict`_ (with optional augmentation). Being tree based, they are not as efficient as hash-based containers (such as Python's builtins) for simple lookup and modification. Conversely:

* (Common Case:) They are far more efficient than them for the case where modifications and lookups are interleaved with sorted iterations.
* (Less Common Case:) Through optional `tree augmentation`_, they are far more efficient than them for some other kinds of useful queries exploiting the underlying tree structure (e.g., what is the ordinal position of this item in the set? which geometric intervals overlap this interval?).


--------
Features
--------

* Supports high-performance algorithms for different use-cases:

    * `Red-black trees`_ for normal cases
    * `Splay trees`_ for `temporal locality`_ cases (i.e., when only a small subset of items will actually be accessed in any time period)
    * `Sorted lists`_ for infrequent-update cases
   
* Provides Pythonic interfaces:

    * Collections are almost entirely drop-in sorted replacements for Python's `set`_ and `dict`_ 
    * User defined ``key`` functions (or older style ``compare`` functions) are supported             
    * Methods take slices and ranges where applicable
    * `Pickle`_ is supported
    * Reverse-order `views`_ are provided
   
* Allows optional `tree augmentation`_ with additional node metadata and tree methods:

    * Dynamic order statistics allow queries for the k-th item
    * Interval trees allow geometric querying
    * Any user-defined algorithm can be easily plugged in
    
    .. Note::

        This feature can be almost entirely ignored if all that is needed are efficient sorted drop-in replacemnt containers for Python's sets and dicts.             
    
   
* Optimized implementation (see the `Performance`_ section in the online documentation):

    * C++ templated backend drives the implementation. `C++ template metaprogramming`_ is used to avoid run-time queries along the search path
    * Homogeneous-key trees optimization

.. _`Red-black trees`: http://en.wikipedia.org/wiki/Red%E2%80%93black_tree
.. _`Splay trees`: http://en.wikipedia.org/wiki/Splay_trees
.. _`Sorted lists`: http://www.google.co.il/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&ved=0CDAQFjAA&url=http%3A%2F%2Fwww.lafstern.org%2Fmatt%2Fcol1.pdf&ei=5zMIUY-tHqmP4gSXi4HYBw&usg=AFQjCNHmQS2fP0Gq86UH75QaMwdx0ahnrA&sig2=Ou3LwZJOTZ5VPs-gTZdSzw&bvm=bv.41642243,d.bGE
.. _`tree augmentation`: http://www.cs.cmu.edu/~avrim/451/lectures/lect0927.txt
.. _`augmenting the data structure`: http://www.cs.cmu.edu/~avrim/451/lectures/lect0927.txt
.. _`C++ template metaprogramming`: http://en.wikipedia.org/wiki/Template_metaprogramming
.. _`temporal locality`: http://en.wikipedia.org/wiki/Locality_of_reference
.. _`set`: http://docs.python.org/2/library/stdtypes.html#set-types-set-frozenset
.. _`dict`: http://docs.python.org/2/library/stdtypes.html#mapping-types-dict
.. _`Pickle`: http://docs.python.org/2/library/pickle.html
.. _`views`: http://docs.python.org/2/library/stdtypes.html#dictionary-view-objects
.. _`Performance`: http://packages.python.org/Banyan/performance.html


--------------------
A Few Quick Examples
--------------------

.. Note::

    The following examples assume first typing::
    
        >>> from banyan import *
    
    
* Choose an algorithm suiting the settings, and obtain a drop-in sorted replacement for Python's builtins:

    * A (red-black tree) general drop-in replacement for Python's set:
    
        ::
       
            >>> t = SortedSet([2, 3, 1])
            >>> t
            SortedSet([1, 2, 3])
            >>> assert 2 in t
            >>> t.add(4)
            >>> len(t)
            4
            >>> t.add(4)
            >>> len(t)
            4
            >>> t.remove(4)
            >>> len(t)
            3
            >>> t.remove(4)
            Traceback (most recent call last):
              File "<stdin>", line 1, in <module>
              File "banyan/__init__.py", line 299, in remove
                self._tree.erase(item)
            KeyError: 4

    * A splay-based sorted drop-in replacement for Python's dict, optimized for temporal-locality accesses:
    
        ::
       
            >>> t = SortedDict([(2, 'b'), (3, 'c'), (1, 'a')], alg = SPLAY_TREE)       
            >>> print(list(t))
            [1, 2, 3]
            >>> assert 1 in t
            >>> assert 4 not in t
            >>> # Now access the key 2 for awhile
            >>> t[2] = 'e'
            >>> t[2] = 'f'
            >>> t[2] = 'g'
            >>> t[2] = 'a'
            >>> t[2] = 'b'
            >>> t[2] = 'c'
            >>> t[2] = 'd'
            >>> t[2] = 'e'

    * A (sorted-list based) sorted memory-efficient drop in for Python's frozenset:
    
        ::
       
            >>> t = FrozenSortedSet(['hao', 'jiu', 'mei', 'jian'])
            >>> assert 'hao' in t
            >>> assert 'zaijian' not in t
            >>> t.add('zaijian')
            Traceback (most recent call last):
              File "<stdin>", line 1, in <module>
            AttributeError: 'FrozenSortedSet' object has no attribute 'add'


* Specify the comparison criteria - e.g., use a string dictionary with lowercase comparison:

    * Using the newer-style ``key`` parameter:
    
    ::
    
        >>> t = SortedDict(key = str.lower)
        >>> t['hao'] = 3
        >>> t['Hao'] = 4
        >>> t
        SortedDict({'Hao': 4})

    * Using the older-style (pre-Py3K) ``compare`` parameter:
    
    ::
   
        >>> t = SortedDict(compare = lambda x, y: cmp(str.lower(x), str.lower(y)))
        >>> t['hao'] = 3
        >>> t['Hao'] = 4
        >>> t
        SortedDict({'Hao': 4})
        
* Work with ranges and slices:
    ::
    
        >>> import string
        >>> 
        >>> t = SortedDict(zip(string.ascii_lowercase, string.ascii_uppercase))
        >>> 
        >>> # Erase everything starting at 'e'
        >>> del t['e': ]
        >>> t
        SortedDict({'a': 'A', 'b': 'B', 'c': 'C', 'd': 'D'})
        >>> 
        >>> # View the items between 'b' and 'd'
        >>> t.items('b', 'd')
        ItemsView([('b', 'B'), ('c', 'C')])
        >>> 
        >>> # View the values of the keys between 'a' and 'c', in reverse order
        >>> t.values('a', 'c', reverse = True)
        ValuesView(['B', 'A'])
        >>> 
        >>> # Set the three values of the keys between 'a' and 'd' to 2
        >>> t['a': 'd'] = [2, 2, 2]
        >>> t
        SortedDict({'a': 2, 'b': 2, 'c': 2, 'd': 'D'})
        
* Easily use homogeneous-keys optimization:

    ::
    
        >>> # Simply specify the key type as key_type - no other changes are needed.
        >>> t = SortedSet([1, 2, 3], key_type = int)
        >>> assert 2 in t
        >>> t = SortedSet(['hao', 'jiu', 'mei', 'jian'], key_type = str)
        >>> assert 'hola' not in t
        
* Use Pythonic versions of `C++/STL's lower_bound/upper_bound`_:        
    ::

        >>> from itertools import *
        >>>
        >>> t = SortedSet(['hao', 'jiu', 'mei', 'jian'])
        >>> t
        SortedSet(['hao', 'jian', 'jiu', 'mei'])
        >>> assert 'hao' in t
        >>> 
        >>> # Find the key after 'hao'
        >>> keys = t.keys('hao', None)
        >>> next(islice(keys, 1, None))
        'jian'

.. _`C++/STL's lower_bound/upper_bound`: http://www.cplusplus.com/reference/map/map/lower_bound/

* Exploit the tree structure for additional efficient functionality:

    * Use an overlapping-interval updator for creating a data structure that can efficiently answer overlapping queries:

    ::    

        >>> t = SortedSet([(1, 3), (2, 4), (-2, 9)], updator = OverlappingIntervalsUpdator)
        >>> 
        >>> print(t.overlap_point(-5))
        []
        >>> print(t.overlap_point(5))
        [(-2, 9)]
        >>> print(t.overlap_point(3.5))
        [(-2, 9), (2, 4)]
        >>>  
        >>> print(t.overlap((-10, 10)))
        [(-2, 9), (1, 3), (2, 4)]
        
    For high performance, combine this with homogeneous-keys optimization:
    
    ::
    
        >>> t = SortedSet(key_type = (int, int), updator = OverlappingIntervalsUpdator)
        >>> t.add((1, 3))
        >>> t.overlap_point(2)
        [(1, 3)]
        >>>
        >>> t = SortedSet(key_type = (float, float), updator = OverlappingIntervalsUpdator)
        >>> t.add((1.0, 3.0))
        >>> t.overlap_point(2.5)
        [(1.0, 3.0)]
        
    * Use a rank updator for creating a data structure that can efficiently answer order queries:

    ::
   
        >>> t = SortedSet(['hao', 'jiu', 'mei', 'jian'], updator = RankUpdator)
        >>> t
        SortedSet(['hao', 'jian', 'jiu', 'mei'])
        >>>
        >>> # 'hao' is item no. 0
        >>> t.kth(0)
        'hao'
        >>> t.order('hao')
        0
        >>>
        >>> # 'mei' is item no. 3
        >>> t.kth(3)
        'mei'
        >>> t.order('mei')
        3
        
    * Use a min-gap updator for creating a data structur that can efficiently answer smallest-gap queries:
    
    ::
        
        >>> t = SortedSet([1, 3, 2], updator = MinGapUpdator)
        >>> t
        SortedSet([1, 2, 3])
        >>> t.min_gap()
        1
        >>> t.remove(2)
        >>> t
        SortedSet([1, 3])
        >>> t.min_gap()
        2


------------------------------------------------------
Download, Installation, Documentation, And Bugtracking
------------------------------------------------------


* The package is at PyPI_.

    .. _PyPI: http://pypi.python.org/pypi/Banyan

* The usual setup for Python libraries is used. Type:
   
    ``$ pip install banyan``
   
    or

    ``$ sudo pip install banyan``

        .. Note::
       
            To install this package from the source distribution, the system must have a C++ compiler installed. The setup script will invoke this compiler.
           
            Using Python 2.x on Windows will attempt to invoke Visual Studio 2008. If you are using a later version, download and extract the archive; then, from within the Banyan directory, use
           
            ``> SET VS90COMNTOOLS=%VS100COMNTOOLS%``
           
            or
           
            ``> SET VS90COMNTOOLS=%VS110COMNTOOLS%``
           
            (for Visual Studio 2010 and 2012, respectively), followed by
           
            ``> python setup.py install``

* The documentation is hosted at `PyPI Docs`_ and can also be found in the 'docs' directory of the distribution.

    .. _`PyPI Docs`: http://packages.python.org/Banyan/

* Bugtracking is on `Google Code`_.

    .. _`Google Code`: http://code.google.com/p/banyan/issues/list?can=1&q=
	
	
-------
Changes
-------

.. csv-table:: Changes
    :header: "Version", "Date", "Description"
    :widths: 15, 15, 70
    :delim: $

    0.1.5 $ 05/04/2013 $ Faster red-black tree iteration; minor documentation bugfixes
    0.1.4 $ 01/04/2013 $ User key-function specification optimization; performance tests for dictionary types; better warnings for user mismatched policies
    0.1.3.5 $ 30/3/2013 $ OverlappingIntervalsUpdator: more regression tests + performance improvements +  performance comparison tests
    0.1.3 $ 28/03/2013 $ OverlappingIntervalsUpdator implemented; minor documentation bugfixes
    0.1.2 $ 26/03/2013 $ Efficient C++ RankUpdator and MinGapUpdator; MinMaxUpdator out; major internal simplification
    0.1.0 $ 17/03/2013 $ Initial release    
    
    
----------------
Planned Features
----------------

* Improve documentation and performance documentation w.r.t. key-type and policy specification.

* Give better yet warnings for user mismatched policies    

* Implement priority search-tree updators


