from __future__ import print_function

import banyan_c


class RankUpdator(object):
    """
    Updates nodes by the size of their subtree. Allows trees employing this to efficiently answer 
        queries such as "what is the k-th key?" and "what is the order of key 'a' in the
        collection?".
        
    Example:
    
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
    """        

    # Metadata appended to each node.
    class Metadata(banyan_c.RankMetadata):
        # Overwritten by C implementation
        pass
            
    def kth(self, k):
        """
        :returns: k-th key
        :raises: :py:exc:`IndexError` if k is too small (negative) or too large (exceeds the 
            order of the largest item).
        """
        
        return self._rank_updator_kth(k)
                
    def order(self, key):
        """
        :returns: The order of key in the keys of the container.        
        """
        
        return self._rank_updator_order(key)

