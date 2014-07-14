from __future__ import print_function
import itertools
import random

import banyan


def simple_trace(node, indent = 0):
    if node is None:
        return

    simple_trace(node.right, indent + 1)   
        
    node_content = ' ' * indent + str(node.key)
    if node.metadata is not None:
        node_content += ' [ ' + str(node.metadata) + ' ]'
    print(node_content)
        
    simple_trace(node.left, indent + 1)   
    
    
def fancy_trace(node):
    def _fancy_trace(node, path):
        if node is None:
            return
            
        _fancy_trace(node.right, path + 'r')            
        
        node_content = ''            
        if path:        
            for d, nd in itertools.izip(path[: -1], path[1: ]):
                node_content += '  ' if d == nd else '| '
            node_content += '/-' if path[-1] == 'r' else '\-'
        node_content += str(node.key)
        if node.metadata is not None:
            node_content += ' [ ' + str(node.metadata) + ' ]'
        print(node_content)    
            
        _fancy_trace(node.left, path + 'l')   

    _fancy_trace(node, '')            
    
    
    
if __name__ == '__main__':
    keys = [random.randint(1, 100) for i in range(18)]
    
    simple_trace(banyan.SortedSet(keys).root)

    fancy_trace(banyan.SortedSet(keys).root)
    
    fancy_trace(banyan.SortedSet(keys, updator = banyan.RankUpdator).root)
    
    fancy_trace(banyan.SortedSet(keys, updator = banyan.MinMaxUpdator).root)

