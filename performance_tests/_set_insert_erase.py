
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
    for e in es:
        t.insert(e)
    for e in es:
        t.remove(e)        


def _blist(es):
    t = blist.sortedset()
    for e in es:
        t.add(e)
    for e in es:
        t.remove(e)        


def _bintrees(es):
    t = bintrees.FastRBTree()
    for e in es:
        t[e] = None
    for e in es:
        t.remove(e)        


def _set(es):
    t = set([])
    for e in es:
        t.add(e)
    for e in es:
        t.remove(e)        


def _banyan(es, key_type, alg, updator):
    t = banyan.SortedSet(key_type = key_type, alg = alg, updator = updator)
    for e in es:
        t.add(e)
    for e in es:
        t.remove(e)        


def _run_test(fn, type_, num_items, num_its):
    if type_ == int:
        es = _src.random_ints(num_items)
    elif type_ == str:
        es = _src.random_strings(num_items, 8)                
    else:
        print type_
        ff
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
        ('banyan_red_black_tree', 
            lambda es: _banyan(es, type_, banyan.RED_BLACK_TREE, None)),
        ('banyan_red_black_tree_rank_updator', 
            lambda es: _banyan(es, type_, banyan.RED_BLACK_TREE, banyan.RankUpdator)),
        ('banyan_red_black_tree_min_max_updator', 
            lambda es: _banyan(es, type_, banyan.RED_BLACK_TREE, banyan.MinMaxUpdator)),
        ('banyan_splay_tree', 
            lambda es: _banyan(es, type_, banyan.SPLAY_TREE, None)),
        ('banyan_sorted_list', 
            lambda es: _banyan(es, type_, banyan.SORTED_LIST, None)),
        ('banyan_red_black_tree_gen', 
            lambda es: _banyan(es, None, banyan.RED_BLACK_TREE, None)),
        ('banyan_splay_tree_gen', 
            lambda es: _banyan(es, None, banyan.SPLAY_TREE, None)),
        ('banyan_sorted_list_gen', 
            lambda es: _banyan(es, None, banyan.SORTED_LIST, None))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fns[name], type_, num_items, num_its)
    return t

