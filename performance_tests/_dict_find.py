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
    sum_ = 1
    for _ in range(num_its):
        for e in es:                                    
            sum_ += t[e]
    end = time.time()
    diff = (end - start) / num_its 
    if sum_ == 0: 
        raise RuntimeError(not_in)          
    return diff


def run_tests(names, num_items, num_its, type_ = int):
    fns = dict([
        ('btrees', lambda es: BTrees.OOBTree.OOBTree([(e, 1) for e in es])),
        ('blist', lambda es: blist.sorteddict.fromkeys(es, 1)),
        ('bintrees', lambda es: bintrees.FastRBTree([(e, 1) for e in es])),
        ('dict', lambda es: dict.fromkeys(es, 1)),
        ('banyan_red_black_tree', 
            lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = type_, alg = banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree', 
            lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = type_, alg = banyan.SPLAY_TREE)),
        ('banyan_sorted_list', 
            lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = type_, alg = banyan.SORTED_LIST)),
        ('banyan_red_black_tree_gen', 
            lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = None, alg = banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree_gen', 
            lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = None, alg = banyan.SPLAY_TREE)),
        ('banyan_sorted_list_gen', 
            lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = None, alg = banyan.SORTED_LIST))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fns[name], type_, num_items, num_its)
    return t

