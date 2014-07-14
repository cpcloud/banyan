import os
import sys
import math
import time

import bintrees

import _src
sys.path.extend(['..', '../..'])
import banyan


def _run_test(fn, type_, num_items, num_its):
    diff = 0
    for _ in range(num_its):
        if type_ == int:
            es = _src.random_ints(num_items)
        elif type_ == str:
            es = _src.random_strings(num_items, 8)                
        else:
            assert False        
        t = fn(es)
        e = es[0]    
        start = time.time()
        if isinstance(t, bintrees.FastRBTree):        
            del t[e: e + 10]
        elif isinstance(t, banyan.SortedSet):        
            t.remove(e, e + 10)
        elif isinstance(t, set):
            t = set([i for i in t if i < e or e + 10 >= i])
        else:
            fff        
        end = time.time()        
        diff += (end - start) / num_its 
    # if len(t) != len(set(es)):
    #    raise RunteimError(len(t))              
    return diff


def run_tests(names, num_items, num_its):
    fns = dict([
        ('bintrees', lambda es: bintrees.FastRBTree([(e, None) for e in es])),
        ('set', lambda es: set(es)),
        ('banyan_red_black_tree', 
            lambda es: banyan.SortedSet(es, alg = banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree', 
            lambda es: banyan.SortedSet(es, alg = banyan.SPLAY_TREE)),
        ('banyan_sorted_list', 
            lambda es: banyan.SortedSet(es, alg = banyan.SORTED_LIST)),
        ('banyan_red_black_tree_gen', 
            lambda es: banyan.SortedSet(es, key_type = int, alg = banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree_gen', 
            lambda es: banyan.SortedSet(es, key_type = int, alg = banyan.SPLAY_TREE)),
        ('banyan_sorted_list_gen', 
        lambda es: banyan.SortedSet(es, key_type = int, alg = banyan.SORTED_LIST))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fns[name], int, num_items, num_its)
    return t

