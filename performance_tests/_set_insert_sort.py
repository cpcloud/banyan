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


def _btrees(es):
    t = BTrees.OOBTree.OOSet()
    for i, e in enumerate(es):
        t.insert(e)
        if i % 1 == 0:
            for ee in t:
                pass


def _blist(es):
    t = blist.sortedset()
    for i, e in enumerate(es):
        t.add(e)
        if i % 1 == 0:
            for ee in t:
                pass


def _bintrees(es):
    t = bintrees.FastRBTree()
    for i, e in enumerate(es):
        t[e] = None
        if i % 1 == 0:
            for ee in t:
                pass


def _set(es):
    t = set([])
    for i, e in enumerate(es):
        t.add(e)
        if i % 1 == 0:
            for ee in sorted(t):
                pass


def _banyan(es, key_type, alg):
    t = banyan.SortedSet(key_type = key_type, alg = alg)
    for i, e in enumerate(es):
        t.add(e)
        if i % 1 == 0:
            for ee in t:
                pass


def _run_test(fn, type_, num_items, num_its):
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


def run_tests(names, num_items, num_its, type_ = int):
    fns = dict([
        ('btrees', lambda es: _btrees(es)),
        ('blist', lambda es: _blist(es)),
        ('bintrees', lambda es: _bintrees(es)),
        ('set', lambda es: _set(es)),
        ('banyan_red_black_tree', lambda es: _banyan(es, type_, banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree', lambda es: _banyan(es, type_, banyan.SPLAY_TREE)),
        ('banyan_sorted_list', lambda es: _banyan(es, type_, banyan.SORTED_LIST)),
        ('banyan_red_black_tree_gen', lambda es: _banyan(es, None, banyan.RED_BLACK_TREE)),
        ('banyan_splay_tree_gen', lambda es: _banyan(es, None, banyan.SPLAY_TREE)),
        ('banyan_sorted_list_gen', lambda es: _banyan(es, None, banyan.SORTED_LIST))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fns[name], type_, num_items, num_its)
    return t

