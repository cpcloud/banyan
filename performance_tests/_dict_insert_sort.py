import os
import sys
import math
import time
import random

import bintrees
import blist
import BTrees.OOBTree 

sys.path.extend(['..', '../..'])
import banyan
import _src


def _run_test(fn, type_, num_items, num_its):
    if type_ == int:
        es = _src.random_ints(num_items)
    elif type_ == str:
        es = _src.random_strings(num_items, 8)                
    else:
        assert False        
    start = time.time()
    for _ in range(num_its):
        t = fn()
        if isinstance(t, dict):
            for i, e in enumerate(es):
                t[e] = 1
                if i % 1 == 0:
                    for ee in sorted(t):
                        pass
        else:                    
            for i, e in enumerate(es):
                t[e] = 1
                if i % 1 == 0:
                    for ee in t:
                        pass
    end = time.time()
    diff = (end - start) / num_its
    return diff


def run_tests(names, num_items, num_its, type_ = int):
    fns = dict([
        ('btrees', lambda: BTrees.OOBTree.OOBTree()),
        ('blist', lambda: blist.sorteddict()),
        ('bintrees', lambda: bintrees.FastRBTree()),
        ('dict', lambda: dict()),
        ('banyan_red_black_tree', 
            lambda: banyan.SortedDict(key_type = type_, alg = banyan.RED_BLACK_TREE)),
        ('banyan_red_black_tree_rank_updator', 
            lambda: banyan.SortedDict(key_type = type_, alg = banyan.RED_BLACK_TREE, updator = banyan.RankUpdator)),
        ('banyan_red_black_tree_min_max_updator', 
            lambda: banyan.SortedDict(key_type = type_, alg = banyan.RED_BLACK_TREE, updator = banyan.MinMaxUpdator)),
        ('banyan_splay_tree', 
            lambda: banyan.SortedDict(key_type = type_, alg = banyan.SPLAY_TREE)),
        ('banyan_sorted_list', 
            lambda: banyan.SortedDict(key_type = type_, alg = banyan.SORTED_LIST)),
        ('banyan_red_black_tree_gen', 
            lambda: banyan.SortedDict(alg = banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree_gen', 
            lambda: banyan.SortedDict(alg = banyan.SPLAY_TREE)),
        ('banyan_sorted_list_gen', 
            lambda: banyan.SortedDict(alg = banyan.SORTED_LIST))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fns[name], type_, num_items, num_its)
    return t

