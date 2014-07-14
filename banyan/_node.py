import sys

from ._dummy_key import _LTDummyKey as _LTDummyKey


class Node(object):
    """
    Encapsulation of a tree node. Node objects can be used to iterate over the tree in flexible ways.     
    This is useful primarily (and perhaps exclusively) in conjunction with tree node metadata updators. 
    Once a useful metadata update scheme has been conceived, it is possible to implement addtional tree
    methods using ``Node`` objects to iterate over the tree in some manner that depends on the metadata.
    
    A ``Node`` object is obtained from a tree object through the latter's ``root`` method. 
    Given a ``Node`` object, it is possible to obtain its content itme, metadata, and left and right 
    children, via properties.    
    For example, here is a toy ufnction that takes a node, and prints the subtree structure
    (rotated 90 degrees counter-clockwise):
    
    >>> def trace(node, indent = 0):
    ...     if node is None:
    ...         return
    ... 
    ...     trace(node.right, indent + 1)   
    ...         
    ...     node_content = ' ' * indent + str(node.key)
    ...     if node.metadata is not None:
    ...         node_content += ' [ ' + str(node.metadata) + ' ]'
    ...     print(node_content)
    ...         
    ...     trace(node.left, indent + 1)   
    ... 
    
    Here are two examples of its output (with different updators):
    
    >>> t = SortedSet(range(25), updator = RankUpdator)
    >>> trace(t.root)    
       24 [ 2 ]
        23 [ 1 ]
      22 [ 5 ]
       21 [ 2 ]
        20 [ 1 ]
     19 [ 12 ]
       18 [ 2 ]
        17 [ 1 ]
      16 [ 6 ]
        15 [ 1 ]
       14 [ 3 ]
        13 [ 1 ]
    12 [ 25 ]
       11 [ 2 ]
        10 [ 1 ]
      9 [ 5 ]
       8 [ 2 ]
        7 [ 1 ]
     6 [ 12 ]
       5 [ 2 ]
        4 [ 1 ]
      3 [ 6 ]
        2 [ 1 ]
       1 [ 3 ]
        0 [ 1 ]
    >>> t = SortedSet(range(25))
    >>> trace(t.root)    
       24
        23
      22
       21
        20
     19
       18
        17
      16
        15
       14
        13
    12
       11
        10
      9
       8
        7
     6
       5
        4
      3
        2
       1
        0
    """

    def __init__(self, c_node):        
        self._c_node = c_node
        
    @property        
    def left(self):
        """
        :returns: Left child node, or ``None`` if there isn't any.
        """
        
        c_node = self._c_node.left()
        return Node(c_node) if c_node is not None else None

    @property        
    def right(self):
        """
        :returns: Right child node, or ``None`` if there isn't any.
        """

        c_node = self._c_node.right()
        return Node(c_node) if c_node is not None else None

    @property        
    def key(self):
        """
        :returns: Key stored in this node.
        """

        return self._c_node.key()

    @property        
    def metadata(self):
        """
        :returns: Metadata stored in this node, or ``None`` if there isn't any.
        """

        return self._c_node.metadata()
        
    @property      
    def key_fn(self):
        """
        :returns: A function mapping items' keys to comparable keys.
        
        This function is synthesized from the parameters passed to the container (either a key 
            function, a comparison function, or none), and is
            guaranteed to produce the same ordering used by the tree.
        """

        return lambda key: _LTDummyKey(self._c_node, key)

