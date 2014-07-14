import os
import sys
import math
import time

import bintrees
import blist
import BTrees.OOBTree 

import _src
sys.path.extend(['..', '../..'])
import banyan


def _run_test(fn, type_, num_items, num_its):
    if type_ == int:
        es = _src.random_ints(num_items)
    elif type_ == str:
        es = _src.random_strings(num_items, 8)                
    else:
        assert False        
    t = fn(es)
    start = time.time()
    if isinstance(t, set):
        for _ in range(num_its):
            in_ = 0
            for e in sorted(t):
                in_ += 1                
    else:            
        for _ in range(num_its):
            in_ = 0
            for e in t:
                in_ += 1                
    end = time.time()
    diff = (end - start) / num_its 
    if in_ != len(t): 
        raise RuntimeError(not_in)          
    # if len(t) != len(set(es)):
    #    raise RunteimError(len(t))              
    return diff


def run_tests(names, num_items, num_its):
    # Tmp Ami - make key-type separate labels below
    fns = dict([
        ('btrees', lambda es: BTrees.OOBTree.OOSet(es)),
        ('blist', lambda es: blist.sortedset(es)),
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

