import os
import sys
import math
import time
import itertools

import bintrees
import blist
import btree
import BTrees.OOBTree 

import _src
sys.path.extend(['..', '../..'])
import banyan


def _run_test(fn, type_, num_items, num_its, mini_num_items):
    if type_ == int:
        es = _src.random_ints(num_items)
    elif type_ == str:
        es = _src.random_strings(num_items, 8)                
    else:
        assert False        
    t = fn(es)
    start = time.time()
    for i in range(max(num_items - mini_num_items - 1, 0)):         
        for e in es[i: i + mini_num_items]:        
            not_in = 0
            for _ in range(num_its):
                if e not in t:
                    not_in += 1
            if not_in != 0:
                raise RuntimeError(not_in)    
    end = time.time()
    diff = (end - start) / num_its 
    return diff


def run_tests(names, num_items, num_its, mini_num_items = 1, type_ = int):
    fns = dict([
        ('btrees', lambda es: BTrees.OOBTree.OOSet(es)),
        ('btree', lambda es: btree.sorted_btree.bulkload(sorted(es), 128)),
        ('blist', lambda es: blist.sortedset(es)),
        ('bintrees', lambda es: bintrees.FastRBTree([(e, None) for e in es])),
        ('set', lambda es: set(es)),
        ('banyan_red_black_tree', 
            lambda es: banyan.SortedSet(es, key_type = type_, alg = banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree', 
            lambda es: banyan.SortedSet(es, key_type = type_, alg = banyan.SPLAY_TREE)),
        ('banyan_sorted_list', 
            lambda es: banyan.SortedSet(es, key_type = type_, alg = banyan.SORTED_LIST)),
        ('banyan_red_black_tree_gen', 
            lambda es: banyan.SortedSet(es, alg = banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree_gen', 
            lambda es: banyan.SortedSet(es, alg = banyan.SPLAY_TREE)),
        ('banyan_sorted_list_gen', 
            lambda es: banyan.SortedSet(es, alg = banyan.SORTED_LIST))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fns[name], type_, num_items, num_its, mini_num_items)
    return t

