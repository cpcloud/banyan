from __future__ import print_function

import banyan_c


class OverlappingIntervalsUpdator(object):
    """
    Updates nodes by the maximum of the intervals specified by their keys. Allows trees employing this to           
        efficiently answer which intervals overlap a point or interval.
        
    Example:
    
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
    
    .. Note::
        
        The keys used with this updator must support the `sequence protocol`_ and be `rich comparable`_, otherwise an exception will be thrown. The begining and end of an interval k are k[0] and k[1].
        
        .. _`sequence protocol`: http://docs.python.org/2/c-api/sequence.html
        .. _`rich comparable`: http://docs.python.org/2/c-api/object.html?highlight=pyobject_richcomparebool#PyObject_RichCompareBool
        
        Examples:
        
        >>> SortedSet([1, 3, 2], updator = OverlappingIntervalsUpdator)
        Traceback (most recent call last):
          ...
        TypeError: 2
        >>> 
        >>> t = SortedSet([(1, 2, 3), (4, 5, 6, 7)], updator = OverlappingIntervalsUpdator)       
        >>> # As far as this tree is concerened, the intervals are (1, 2) and (4, 5).
        >>> print(t.overlap_point(1.5))
        [(1, 2, 3)]
    """        

    # Metadata appended to each node.
    class Metadata(banyan_c.OverlappingIntervalsMetadata):
        # Overwritten by C implementation
        pass
        
    def overlap(self, range_):        
        """
        :returns: Keys overlapping range_.
        
        Example:
        
        >>> t = SortedDict.fromkeys([(1, 3), (2, 4), (-2, 9)], updator = OverlappingIntervalsUpdator)
        >>> print(t.overlap((-10, 10)))
        [(-2, 9), (1, 3), (2, 4)]
        """

        return self._interval_max_updator_overlapping_range(range_)
        
    def overlap_point(self, point):
        """
        :returns: Keys overlapping point.
        
        Example:
        
        >>> t = SortedDict.fromkeys([(1, 3), (2, 4), (-2, 9)], updator = OverlappingIntervalsUpdator)
        >>> print(t.overlap_point(-5))
        []
        >>> print(t.overlap_point(5))
        [(-2, 9)]
        >>> print(t.overlap_point(3.5))
        [(-2, 9), (2, 4)]
        """
        
        return self._interval_max_updator_overlapping(point)
                

