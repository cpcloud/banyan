import os
import sys
import math
import time
import random
if sys.version_info < (3, 0):
    from itertools import izip as zip_
else:
    zip_ = zip           

sys.path.extend(['..', '../..'])
import banyan
from bx.intervals.intersection import IntervalTree
import _src


def _banyan(es, key_type):
    t = banyan.SortedSet(updator = banyan.OverlappingIntervalsUpdator, key_type = key_type)
    for e in es:
        t.add(e)
        c = len(t.overlap(e))


def _bx(es):
    t = IntervalTree()
    for e in es:
        t.add(e[0], e[1], e)
        c = len(t.find(e[0], e[1]))


def _run_test(fn, num_items, num_its):
    es = [(min(b, e), max(b, e)) for (b, e) in zip(_src.random_ints(num_items), _src.random_ints(num_items))]
    start = time.time()
    for _ in range(num_its):
        fn(es)            
    end = time.time()
    diff = (end - start) / num_its
    return diff


def run_tests(names, num_items, num_its):
    fns = dict([
        ('bx', lambda es: _bx(es)),
        ('banyan_red_black_tree', lambda es: _banyan(es, key_type = (int, int))),
        ('banyan_red_black_tree_float', lambda es: _banyan(es, key_type = (float, float))),
        ('banyan_red_black_tree_gen', lambda es: _banyan(es, key_type = None))])
    t = dict([])        
    for name in names:        
        t[name] = _run_test(fns[name], num_items, num_its)
    return t

