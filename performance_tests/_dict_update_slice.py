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


def _run_test(fn_types, type_, num_items, num_its):
    if type_ == int:
        es = _src.random_ints(num_items)
    elif type_ == str:
        es = _src.random_strings(num_items, 8)                
    else:
        assert False        
    e = es[0]    
    t = fn_types[0](es)
    start = time.time()
    for _ in range(num_its):
        end = time.time()        
        if fn_types[1] is None:
            t[e: e + 10] = [v + 1 for v in t[e: e + 10]]
        else:
            t = fn_types[1]([(k, v + 1)  if e <= k < e + 10 else (k, v) for (k, v) in t.items()])
    end = time.time()        
    return (end - start) / num_its 


def run_tests(names, num_items, num_its):
    fn_types = dict([
        ('btrees', (lambda es: BTrees.OOBTree.OOBTree([(e, 1) for e in es]), BTrees.OOBTree.OOBTree)),
        ('blist', (lambda es: blist.sorteddict.fromkeys(es, 1), blist.sorteddict)),
        ('bintrees', (lambda es: bintrees.FastRBTree([(e, 1) for e in es]), bintrees.FastRBTree)),
        ('dict', (lambda es: dict.fromkeys(es, 1), dict)),
        ('banyan_red_black_tree', 
            (lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = int, alg = banyan.RED_BLACK_TREE), None)),
        ('banyan_splay_tree', 
            (lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = int, alg = banyan.SPLAY_TREE), None)),
        ('banyan_sorted_list', 
            (lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = int, alg = banyan.SORTED_LIST), None)),
        ('banyan_red_black_tree_gen', 
            (lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = None, alg = banyan.RED_BLACK_TREE), None)),
        ('banyan_splay_tree_gen', 
            (lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = None, alg = banyan.SPLAY_TREE), None)),
        ('banyan_sorted_list_gen', 
            (lambda es: banyan.SortedDict.fromkeys(es, 1, key_type = None, alg = banyan.SORTED_LIST), None))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fn_types[name], int, num_items, num_its)
    return t

