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
    not_in = 0
    for _ in range(num_its):
        for e in es:
            if e not in t:
                not_in += 1
    end = time.time()
    diff = (end - start) / num_its 
    if not_in != 0: 
        raise RuntimeError(not_in)          
    return diff


def run_tests(names, num_items, num_its, type_ = int):
    fns = dict([
        ('btrees', lambda es: BTrees.OOBTree.OOSet(es)),
        ('blist', lambda es: blist.sortedset(es)),
        ('bintrees', lambda es: bintrees.FastRBTree([(e, None) for e in es])),
        ('set', lambda es: set(es)),
        ('banyan_red_black_tree_rank_updator', 
            lambda es: banyan.SortedSet(es, key_type = type_, alg = banyan.RED_BLACK_TREE, 
                updator = banyan.RankUpdator)),
        ('banyan_red_black_tree_min_max_updator', 
            lambda es: banyan.SortedSet(es, key_type = type_, alg = banyan.RED_BLACK_TREE, 
                updator = banyan.MinMaxUpdator)),
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
        t[name] = _run_test(fns[name], type_, num_items, num_its)
    return t

