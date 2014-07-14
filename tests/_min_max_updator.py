class MinMaxUpdator(object):
    """
    Updates nodes by the minimum and maximum of their subtrees. 
    
    Example:
    
    >>> t = SortedSet(['hao', 'jiu', 'mei', 'jian'], updator = MinMaxUpdator)
    >>> t
    SortedSet(['hao', 'jian', 'jiu', 'mei'])
    >>>
    >>> t.min_()
    'hao'
    >>>
    >>> t.max_()
    'mei'
    """        

    # Metadata appended to each node.
    class Metadata(object):

        # Updates the minimum and maximum from the the current item
        #   and those of the children.
        def update(self, key, key_fn, l, r):        
            self._min = min(key, l._min, key = key_fn) if l is not None else key
            self._max = max(key, r._max, key = key_fn) if r is not None else key
            
        def __repr__(self):
            return '(' + str(self._min) + ', ' + str(self._max) + ')'       
                                    
    def min_(self):            
        """
        :returns: Smallest key in the container.
        """
        
        assert self.root is not None
        return self.root.metadata._min

    def max_(self):            
        """
        :returns: Largest key in the container.
        """
        
        assert self.root is not None
        return self.root.metadata._max

