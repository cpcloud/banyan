import os
import sys
import math
import time
import random
if sys.version_info < (3, 0):
    from itertools import izip as zip_
else:
    zip_ = zip           

import bintrees
import blist
import BTrees.OOBTree 

sys.path.extend(['..', '../..'])
import banyan
import _src


def _btrees(es):
    t = BTrees.OOBTree.OOSet(es)
    for e in es:
        t.insert(e)
        if len(t) > 1:
            keys = list(t)
            c = min([abs(x - y) for (x, y) in zip_(keys[1: ], keys[: -1])])            


def _blist(es):
    t = blist.sortedset()
    for e in es:
        t.add(e)
        if len(t) > 1:
            keys = list(t)
            c = min([abs(x - y) for (x, y) in zip_(keys[1: ], keys[: -1])])            


def _bintrees(es):
    t = bintrees.FastRBTree()
    for e in es:
        t[e] = None
        if len(t) > 1:
            keys = list(t)
            c = min([abs(x - y) for (x, y) in zip_(keys[1: ], keys[: -1])])            


def _set(es):
    t = set([])
    for e in es:
        t.add(e)
        if len(t) > 1:
            keys = sorted(list(t))
            c = min([abs(x - y) for (x, y) in zip_(keys[1: ], keys[: -1])])            


def _banyan(es, key_type, alg, updator):
    t = banyan.SortedSet(key_type = key_type, alg = alg, updator = updator)
    if updator is None:
        for e in es:
            t.add(e)
            if len(t) > 1:
                keys = list(t)
                c = min([abs(x - y) for (x, y) in zip_(keys[1: ], keys[: -1])])            
    else:                
        for e in es:            
            t.add(e)
            if len(t) > 1:
                c = t.min_gap()


def _run_test(fn, type_, num_items, num_its):
    # Tmp Ami
    type_ = int
    if type_ == int:
        es = _src.random_ints(num_items)
    elif type_ == str:
        es = _src.random_strings(num_items, 8)                
    else:
        assert False        
    start = time.time()
    for _ in range(num_its):
        fn(es)            
    end = time.time()
    diff = (end - start) / num_its
    return diff


def run_tests(names, num_items, num_its):
    fns = dict([
        ('btrees', lambda es: _btrees(es)),
        ('blist', lambda es: _blist(es)),
        ('bintrees', lambda es: _bintrees(es)),
        ('set', lambda es: _set(es)),
        ('banyan_red_black_tree', 
            lambda es: _banyan(es, int, banyan.RED_BLACK_TREE, None)),
        ('banyan_red_black_tree_min_gap_updator', 
            lambda es: _banyan(es, int, banyan.RED_BLACK_TREE, banyan.MinGapUpdator)),
        ('banyan_splay_tree', 
            lambda es: _banyan(es, int, banyan.SPLAY_TREE, None)),
        ('banyan_sorted_list', 
            lambda es: _banyan(es, int, banyan.SORTED_LIST, None)),
        ('banyan_red_black_tree_gen', 
            lambda es: _banyan(es, None, banyan.RED_BLACK_TREE, None)),
        ('banyan_splay_tree_gen', 
            lambda es: _banyan(es, None, banyan.SPLAY_TREE, None)),
        ('banyan_sorted_list_gen', 
            lambda es: _banyan(es, None, banyan.SORTED_LIST, None))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fns[name], int, num_items, num_its)
    return t

