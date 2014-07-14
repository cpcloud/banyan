from __future__ import print_function
import sys
import types
import unittest
import random
import os
import itertools
import warnings
import doctest
try:
   import cPickle as pickle_
except:
   import pickle as pickle_
if sys.version_info < (3, 0):
    from itertools import izip as zip_
else:
    zip_ = zip           

from banyan import *
if sys.version_info < (3, 0):    
    from _base2 import UnittestBase
    from _base2 import StringTestBase
else:     
    from _base3 import UnittestBase 
    from _base3 import StringTestBase
import _leak_detector
import banyan._trace
import _min_max_updator


# Tmp Ami
_key_types = [None, int, float]

_set_types = [FrozenSortedSet, SortedSet]
_dict_types = [FrozenSortedDict, SortedDict]

_algs = [RED_BLACK_TREE, SPLAY_TREE, SORTED_LIST]
#_algs = [RED_BLACK_TREE]

# Tmp Ami
#_updators = [None, RankUpdator]
_updators = [None, RankUpdator, MinGapUpdator, _min_max_updator.MinMaxUpdator]

# Tmp Ami
_keys = [None, lambda x: x]
#_keys = [None]


# Tmp Ami - not enough None/reverseviews in stress
# Tmp Ami - add also mismatched key examples.


class _Test00Basics(unittest.TestCase, UnittestBase):    
    def test_00(self):
        self.assertEqual(os.system('scons'), 0)
        
        seed = random.randint(0, 999999)

        # Tmp Ami
        # self.assertEqual(os.system('./debug_test %d' % seed), 0)
        self.assertEqual(os.system('./test %d' % seed), 0)
        
    def test_01(self):     
        try:            
            os.chdir('../docs')

            os.system('make text')
            num_fails, num_tests = doctest.testfile('build/text/reference.txt')
        finally:        
            os.chdir('..')
        self.assertGreater(num_tests, 0)            
        self.assertEqual(num_fails, 0)

        num_fails, num_tests = doctest.testfile('README.txt')
        self.assertGreater(num_tests, 0)            
        self.assertEqual(num_fails, 0)


class _Test01BasicSet(unittest.TestCase, UnittestBase):    
    def test_00(self):
        for key_type, cnt, alg, key, in itertools.product(_key_types, _set_types, _algs, _keys):
            r = cnt(alg = alg, key_type = key_type, key = key)
            self.assertEqual(len(r), 0)

    def test_01(self):
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            r = cnt(alg = alg, key_type = key_type, key = key)
            self.assertEqual(len(r), 0)
            try:
                r.add(2)
            except AttributeError:                
                if cnt == FrozenSortedSet:
                    continue
            r.add(3)
            self.assertEqual(len(r), 2)

    def test_02(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            r = SortedSet(alg = alg, key_type = key_type, key = key)
            self.assertEqual(len(r), 0)
            r.add(2)
            r.add(3)
            self.assertIn(2, r)
            self.assertEqual(len(r), 2)
            self.assertNotIn(4, r)
            r.remove(2)
            self.assertNotIn(2, r)
            
    def test_03(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            r = SortedSet(alg = alg, key_type = key_type, key = key)
            self.assertEqual(len(r), 0)
            r.add(2)
            r.add(3)
            self.assertEqual(len(r), 2)
            self.assertNotIn(4, r)
            r.remove(2)
            self.assertNotIn(2, r)
            try:
                r.remove(4)
                self.assertEqual(1, 0)
                pass
            except KeyError:
                pass
                
    def test_04(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            r = SortedSet(alg = alg, key_type = key_type,  key = key)
            self.assertEqual(len(r), 0)
            r.add(2)
            r.add(3)
            self.assertEqual(list(r), [2, 3])

    def test_05(self):
        for key_type, alg in itertools.product(_key_types, _algs):
            r = SortedSet(alg = alg, key_type = key_type, compare = lambda x, y: y - x)
            self.assertEqual(len(r), 0)
            r.add(2)
            r.add(3)
            self.assertEqual(list(r), [3, 2])
            
            r = SortedSet(alg = alg, key_type = key_type, key = lambda x: - x)
            self.assertEqual(len(r), 0)
            r.add(2)
            r.add(3)
            self.assertEqual(list(r), [3, 2])

    def test_06(self):
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            r = cnt([4, 3, 5], key_type = key_type, alg = alg, key = key)
            self.assertEqual(len(r), 3)
            self.assertEqual(list(r), [3, 4, 5])

    def test_07(self):
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertTrue(cnt([2, 3], key_type = key_type, alg = alg, key = key).issubset(
                cnt([2, 3], alg = alg, key = key)))
            self.assertFalse(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issubset(
                cnt([2, 3], alg = alg, key = key)))
            self.assertTrue(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issubset(
                cnt([2, 3, 4, 10], alg = alg, key = key)))
            self.assertTrue(cnt([], key_type = key_type, alg = alg, key = key).issubset(
                cnt([2, 3, 4, 10], alg = alg, key = key)))
            self.assertFalse(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issubset(
                cnt([], alg = alg, key = key)))
            self.assertFalse(cnt([2, 4, 6], key_type = key_type, alg = alg, key = key).issubset(
                cnt([2, 3], alg = alg, key = key)))

            self.assertTrue(cnt([2, 3], key_type = key_type, alg = alg, key = key).issubset([2, 3]))
            self.assertFalse(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issubset([2, 3]))
            self.assertTrue(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issubset([2, 3, 4, 10]))
            self.assertTrue(cnt([], key_type = key_type, alg = alg, key = key).issubset([2, 3, 4, 10]))
            self.assertFalse(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issubset([]))
            self.assertFalse(cnt([2, 4, 6], key_type = key_type, alg = alg, key = key).issubset([2, 3]))
            
            self.assertTrue(cnt([2, 3], key_type = key_type, alg = alg, key = key) <= [3, 2])

            self.assertLess(SortedSet([1, 3, 2], key_type = key_type, alg = alg, key = key), [1, 2, 3, 4])
            self.assertLessEqual(SortedSet([1, 3, 2], key_type = key_type, alg = alg, key = key), [1, 2, 3])

    def test_08(self):
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertTrue(cnt([2, 3], key_type = key_type, alg = alg, key = key).issuperset(
                cnt([2, 3], key_type = key_type, alg = alg, key = key)))
            self.assertTrue(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issuperset(
                cnt([2, 3], key_type = key_type, alg = alg, key = key)))
            self.assertFalse(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issuperset(
                cnt([2, 3, 4, 10], key_type = key_type, alg = alg, key = key)))
            self.assertFalse(cnt([], key_type = key_type, alg = alg, key = key).issuperset(
                cnt([2, 3, 4, 10], key_type = key_type, alg = alg, key = key)))
            self.assertTrue(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issuperset(
                cnt([], key_type = key_type, alg = alg, key = key)))
            self.assertFalse(cnt([2, 4, 6], key_type = key_type, alg = alg, key = key).issuperset(
                cnt([2, 3], key_type = key_type, alg = alg, key = key)))

            self.assertTrue(cnt([2, 3], key_type = key_type, alg = alg, key = key).issuperset([2, 3]))
            self.assertTrue(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issuperset([2, 3]))
            self.assertFalse(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issuperset([2, 3, 4, 10]))
            self.assertFalse(cnt([], key_type = key_type, alg = alg, key = key).issuperset([2, 3, 4, 10]))
            self.assertTrue(cnt([2, 3, 4], key_type = key_type, alg = alg, key = key).issuperset([]))
            self.assertFalse(cnt([2, 4, 6], key_type = key_type, alg = alg, key = key).issuperset([2, 3]))
            
            self.assertTrue(cnt([2, 3], key_type = key_type, alg = alg, key = key) >= [3, 2])

            self.assertGreater(SortedSet([1, 3, 2, 4], key_type = key_type, key = key), [1, 2, 3])
            self.assertLessEqual(SortedSet([1, 3, 2], key_type = key_type, key = key), [1, 2, 3])

    def test_09(self):
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertTrue(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint(
                cnt([4, 5], key_type = key_type, alg = alg, key = key)))
            self.assertTrue(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint(
                cnt([-1, 5], key_type = key_type, alg = alg, key = key)))
            self.assertTrue(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint(
                cnt([-1, -1], key_type = key_type, alg = alg, key = key)))
            self.assertFalse(cnt([1, 2, 3], alg = alg, key = key).isdisjoint(
                cnt([-1, 1, 3], key_type = key_type, alg = alg, key = key)))
            self.assertTrue(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint(
                cnt([], key_type = key_type, alg = alg, key = key)))
            self.assertFalse(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint(
                cnt([1, 2], key_type = key_type, alg = alg, key = key)))

            self.assertTrue(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint([4, 5]))
            self.assertTrue(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint([-1, 5]))
            self.assertTrue(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint([-1, -1]))
            self.assertFalse(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint([-1, 1, 3]))
            self.assertTrue(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint([]))
            self.assertFalse(cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).isdisjoint([1, 2]))

    def test_10(self):
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertTrue(cnt([1, 3, 2], key_type = key_type, alg = alg, key = key) == [1, 2, 3])
            self.assertFalse(cnt([1, 3, 2], key_type = key_type, alg = alg, key = key) == [1, 2, 2])
            self.assertTrue(cnt([], key_type = key_type, alg = alg, key = key) == [])
            self.assertFalse(cnt([1], key_type = key_type, alg = alg, key = key) == [])
            self.assertFalse(cnt([], key_type = key_type, alg = alg, key = key) == [1])            

            self.assertFalse(cnt([1, 3, 2], key_type = key_type, alg = alg, key = key) != [1, 2, 3])
            self.assertTrue(cnt([1, 3, 2], key_type = key_type, alg = alg, key = key) != [1, 2, 2])
            self.assertFalse(cnt([], key_type = key_type, alg = alg, key = key) != [])
            self.assertTrue(cnt([1], key_type = key_type, alg = alg, key = key) != [])
            self.assertTrue(cnt([], key_type = key_type, alg = alg, key = key) != [1])            

    def test_11(self):            
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertTrue(
                cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).__repr__().find('([1, 2, 3])') != -1 or \
                cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).__repr__().find('([1.0, 2.0, 3.0])') != -1)
            
    def test_12(self):
        for key_type, cnt, alg in itertools.product(_key_types, _set_types, _algs):
            w = cnt([4, 3, 5], key_type = key_type, alg = alg)
            s = pickle_.dumps(w)
            r = pickle_.loads(s)
            self.assertEqual(list(r), [3, 4, 5], r)   
            
    def test_13(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedSet(alg = alg, key_type = key_type, key = key)
            self.assertEqual(len(t), 0)
            t.add(0)
            self.assertEqual(len(t), 1)
            t.clear()
            self.assertEqual(len(t), 0)
            t.add(0)
            self.assertEqual(len(t), 1)
            
    def test_14(self):            
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertEqual(
                cnt([1, 2, 3], key_type = key_type, alg = alg, key = key) | \
                    cnt([4, 3], key_type = key_type, key = key), 
                cnt([1, 2, 3, 4], key_type = key_type, key = key))
            self.assertEqual(
                cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).union(
                    cnt([4], key_type = key_type, key = key), cnt([3], key_type = key_type, key = key)), 
                cnt([1, 2, 3, 4]))
            
    def test_15(self):            
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertEqual(
                cnt([1, 2, 3], key_type = key_type, alg = alg, key = key) & \
                    cnt([4, 3], key_type = key_type, key = key), 
                cnt([3], key_type = key_type, key = key))
            self.assertEqual(
                cnt([1, 2, 3], key_type = key_type, alg = alg, key = key).intersection(
                    cnt([1, 2], key_type = key_type, key = key), cnt([3, 2], key_type = key_type, key = key)), 
                cnt([2], key_type = key_type, key = key))

    def test_16(self):            
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertEqual(cnt([1, 2, 3], alg = alg, key = key) - cnt([4, 3]), cnt([1, 2]))
            self.assertEqual(cnt([1, 2, 3], alg = alg, key = key).difference(cnt([1, 2]), cnt([3, 2])), cnt([]))

    def test_17(self):            
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertEqual(cnt([1, 2, 3], alg = alg, key = key) ^ cnt([4, 3]), cnt([1, 2, 4]))
            self.assertEqual(
                cnt([1, 2, 3], alg = alg, key = key).symmetric_difference(
                    cnt([4, 5]), cnt([1, 2, 3, 4])), 
                cnt([5]))

    def test_18(self):            
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            self.assertEqual(cnt([1, 2, 3], alg = alg, key = key).copy(), cnt([1, 3, 2], alg = alg, key = key))

    def test_19(self):            
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedSet([1, 2, 3], key_type = key_type, alg = alg, key = key)
            t.update([20])
            self.assertEqual(t, [1, 2, 3, 20])
            
            t = SortedSet([1, 2, 3], key_type = key_type, alg = alg, key = key)
            t.intersection_update([20])
            self.assertEqual(t, [])
            
            t = SortedSet([1, 2, 3], key_type = key_type, alg = alg, key = key)
            t.difference_update([20])
            self.assertEqual(t, [1, 2, 3])
            
            t = SortedSet([1, 2, 3], key_type = key_type, alg = alg, key = key)
            t.symmetric_difference_update([2, 5])
            self.assertEqual(t, [1, 3, 5])

    def test_20(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedSet([1, 2, 3], key_type = key_type, alg = alg, key = key)
            t.pop()
            self.assertEqual(len(t), 2)
            while len(t) > 0:
                t.pop()
                
    def test_21(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = FrozenSortedSet([1, 2, 3], key_type = key_type, alg = alg, key = key)            
            h = set(t)                

    def test_22(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedSet([1, 2, 3], key_type = key_type, alg = alg, key = key)            
            with self.assertRaises(TypeError):
                h = set([t])
                
    def test_23(self):                
        for key_type, cnt, alg, key in itertools.product(_key_types, _set_types, _algs, _keys):
            t = cnt([1, 2, 3, 4], key_type = key_type, alg = alg, key = key)
            
            v = t.keys()
            self.assertEqual(v, cnt([1, 2, 3, 4], key_type = key_type, alg = alg, key = key).keys())
            
            v = t.keys(3)
            self.assertEqual(v, cnt([1, 2], key_type = key_type, alg = alg, key = key).keys())
            
            v = t.keys(3, reverse = True)
            self.assertEqual(v, cnt([1, 2], key_type = key_type, alg = alg, key = key).keys())
            
            v = t.keys(0, 3)
            self.assertEqual(v, cnt([1, 2], key_type = key_type, alg = alg, key = key).keys())
            
            v = t.keys(0, 23)
            self.assertEqual(v, cnt([1, 2, 3, 4], key_type = key_type, alg = alg, key = key).keys())
                                                        
    def test_24(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedSet(range(1, 7), key_type = key_type, alg = alg, key = key)            
            t.remove(2, 5)
            self.assertEqual(t, SortedSet([1, 5, 6], key_type = key_type, alg = alg, key = key))

            t = SortedSet(range(1, 7), key_type = key_type, alg = alg, key = key)            
            t.remove(None, 5)
            self.assertEqual(t, SortedSet([5, 6], key_type = key_type, alg = alg, key = key))

            t = SortedSet(range(1, 7), key_type = key_type, alg = alg, key = key)            
            t.remove(2, None)
            self.assertEqual(t, SortedSet([1], key_type = key_type, alg = alg, key = key))

            t = SortedSet(range(1, 7), key_type = key_type, alg = alg, key = key)            
            t.remove(None, None)
            self.assertEqual(t, SortedSet([], key_type = key_type, alg = alg, key = key))

            
# From http://eli.thegreenplace.net/2010/01/22/weighted-random-generation-in-python            
def _weighted_choice(weights):                                     
    total, winner = 0, 0
    for i, w in enumerate(weights):
        total += w
        if random.random() * total < w:
            winner = i
    return winner                        
    
    
class _StressTest:
    _updators = _updators + [OverlappingIntervalsUpdator]
    
    @classmethod
    def updator_key_type(cls, updator, key_type):
        if updator is not OverlappingIntervalsUpdator:
            return key_type
            
        return None if key_type is None else (key_type, key_type)

    @classmethod
    def updator_r(cls, updator, num_distinct_items):
        if updator is not OverlappingIntervalsUpdator:
            def r_():
                return random.randrange(0, num_distinct_items)
        else:
            def r_():
                return (random.randrange(0, num_distinct_items), random.randrange(0, num_distinct_items))
                        
        return r_                

    def _check_tree_ok(self, r_, tree, builtin_):
        self.assertEqual(len(tree), len(builtin_)) 
        
        for e in tree:
            self.assertIn(e, builtin_)        
            
        self.assertEqual(len(tree), len(builtin_))            
           
        self.assertEqual(list(tree), sorted(list(tree)))   
        
        def _check_node_ok(node):
            if node is None:
                return
                
            _check_node_ok(node.left)
            
            if node.left is not None:
                self.assertGreaterEqual(node.key_fn(node.key), node.key_fn(node.left.key))
            if node.right is not None:
                self.assertLessEqual(node.key_fn(node.key), node.key_fn(node.right.key))
            
            _check_node_ok(node.right)
            
        _check_node_ok(tree.root)            
                                                                         
        def _count_nodes(node):
            if node is None:
                return 0
            
            count = 1    
            if node.left is not None:
                count += _count_nodes(node.left)
            if node.right is not None:
                count += _count_nodes(node.right)
            
            return count
            
        self.assertEqual(_count_nodes(tree.root), len(builtin_))
        
        if len(builtin_) > 0:
            if hasattr(tree, 'min_'):
                self.assertEqual(tree.min_(), min(builtin_))
                self.assertEqual(tree.max_(), max(builtin_))

            if hasattr(tree, 'kth'):
                j = random.randint(0, len(builtin_) - 1)
                key = list(sorted(builtin_))[j]
                self.assertEqual(tree.kth(j), key)
                self.assertEqual(tree.order(key), j)
    
            if hasattr(tree, 'min_gap'):
                keys = list(sorted(builtin_))
                if len(keys) > 1:
                    builtin_min_gap = \
                        min([abs(x - y) for (x, y) in zip_(keys[1: ], keys[: -1])])
                    self.assertEqual(
                        tree.min_gap(), 
                        builtin_min_gap, 
                        (tree.min_gap(), list(tree)))

            if hasattr(tree, 'overlap'):
                keys = list(sorted(builtin_))
                if len(keys) > 0:
                    self.assertEqual(
                        tree.overlap(keys[0]),
                        [(b, e) for (b, e) in keys if b <= keys[0][1] and keys[0][0] <= e])
                        
                    self.assertEqual(
                        tree.overlap_point(keys[0][0]),
                        [(b, e) for (b, e) in keys if b <= keys[0][0] and keys[0][0] <= e])

    
class _Test02SetStress(unittest.TestCase, UnittestBase, _StressTest):    
    def _check_tree_ok(self, r_, tree, builtin_, num_distinct_items, new_tree_ave_len):
        _StressTest._check_tree_ok(self, r_, tree, builtin_)

        len_ = int(random.expovariate(1. / new_tree_ave_len))
        items = [r_() for _ in range(len_)]
        other_tree = SortedSet(items)
        other_set = set(items)
        
        self.assertEqual(tree < other_tree, builtin_ < other_set)
        self.assertEqual(tree <= other_tree, builtin_ <= other_set)
        self.assertEqual(tree.issubset(other_tree), builtin_.issubset(other_set))        
        self.assertEqual(tree > other_tree, builtin_ > other_set)
        self.assertEqual(tree >= other_tree, builtin_ >= other_set)
        self.assertEqual(tree.issuperset(other_tree), builtin_.issuperset(other_set))        
        self.assertEqual(tree.isdisjoint(other_tree), builtin_.isdisjoint(other_set))        
        self.assertEqual(tree == other_tree, builtin_ == other_set)
        self.assertEqual(tree != other_tree, builtin_ != other_set)
        
    def _generic_test(self, num_trees, num_its, num_distinct_items, new_tree_ave_len, weights):
        for key_type, alg, updator, key in itertools.product(_key_types, _algs, _StressTest._updators, _keys):
            key_type = _StressTest.updator_key_type(updator, key_type)
            r_ = _StressTest.updator_r(updator, num_distinct_items)
        
            print(key_type, alg, key, updator)

            with _leak_detector.check_obj_count(100):
                trees = [SortedSet(key_type = key_type, alg = alg, key = key, updator = updator) \
                    for _ in range(num_trees)]
                sets = [set() for _ in range(num_trees)]
                
                for _ in range(num_its):            
                    i = random.randrange(0, num_trees)
                    choice = _weighted_choice(weights)
                    
                    if choice == 0:
                        len_ = int(random.expovariate(1. / new_tree_ave_len))
                        items = [r_() for _ in range(len_)]
                        trees[i] = SortedSet(items, key_type = key_type, alg = alg, key = key, updator = updator)
                        sets[i] = set(items)
                        
                        self._check_tree_ok(r_, trees[i], sets[i], num_distinct_items, new_tree_ave_len)
                        
                        continue
                                                                
                    tree, set_ = trees[i], sets[i]
                    item = r_()
                    
                    if choice == 1:
                        self.assertEqual(item in tree, item in set_)                    
                    elif choice == 2:
                        tree.add(item)
                        set_.add(item)
                    elif choice == 3:
                        erase_choice = _weighted_choice([1] * 5)
                        if erase_choice == 0:
                            if item in tree:
                                self.assertIn(item, set_)
                                tree.remove(item)
                                set_.remove(item)
                            else:
                                self.assertNotIn(item, set_) 
                        elif erase_choice == 1:
                            stop = r_()
                            for i in tree:
                                if item <= i < stop:
                                    set_.remove(i)
                            tree.remove(item, stop)                                
                        elif erase_choice == 2:
                            for i in tree:
                                if item <= i:
                                    set_.remove(i)
                            tree.remove(item, None)                                
                        elif erase_choice == 3:
                            stop = r_()
                            for i in tree:
                                if i < stop:
                                    set_.remove(i)
                            tree.remove(None, stop)                                
                        else:
                            set_.clear()
                            tree.remove(None, None)                        
                    elif choice == 4:                    
                        len_ = int(random.expovariate(1. / new_tree_ave_len))
                        items = [r_() for _ in range(len_)]
                        tree = SortedSet(items, key_type = key_type, alg = alg, key = key, updator = updator)
                        set_ = set(items)
                        
                        ext_merge_choice = _weighted_choice([1] * 4)
                        
                        if ext_merge_choice == 0:
                            trees[i] |= tree
                            sets[i] |= set_
                        elif ext_merge_choice == 1:                        
                            trees[i] &= tree
                            sets[i] &= set_
                        elif ext_merge_choice == 2:
                            trees[i] -= tree
                            sets[i] -= set_
                        elif ext_merge_choice == 3:                        
                            trees[i] ^= tree
                            sets[i] ^= set_
                        else:
                            self.assertTrue(False, ext_merge_choice)                        
                        
                        self._check_tree_ok(r_, trees[i], sets[i], num_distinct_items, new_tree_ave_len)
                    elif choice == 5:
                        tree = trees[i].copy()
                        self.assertEqual(tree, trees[i])
                        trees[i] = tree                    
                    else:
                        self.assertTrue(False, choice)                    
                
                    for tree, set_ in zip_(trees, sets):
                        self._check_tree_ok(r_, tree, set_, num_distinct_items, new_tree_ave_len)
              
            tree = set_ = trees = sets = item = items = None

    def test_00(self):
        num_trees = 5
        num_its = 1000
        num_distinct_items = 10000
        
        new_tree_ave_len = 10
        
        new_tree_weight = 1
        find_weight = 150
        insert_weight = 70
        remove_weight = 20
        ext_merge_weight = 30
        copy_weight = 10
        
        weights = [new_tree_weight, find_weight, insert_weight, remove_weight, ext_merge_weight, copy_weight]
        
        self._generic_test(num_trees, num_its, num_distinct_items, new_tree_ave_len, weights)


class _Test03BasicDict(unittest.TestCase, UnittestBase):    
    def test_00(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            r = SortedDict(alg = alg, key_type = key_type, key = key)
            
            r[2] = 'b'
            r[3] = 'c'
            self.assertIn(3, r)
            try:
                r[4]
                self.assertTrue(False)
            except KeyError:
                pass
            del r[2]
            self.assertNotIn(2, r)
            
    def test_01(self):            
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            r = SortedDict(alg = alg, key_type = key_type, key = key)
            
            r[2] = 'b'
            r[3] = 'c'
            self.assertIn(3, r)
            try:
                r[4]
                self.assertTrue(False)
            except KeyError:
                pass
            del r[2]
            self.assertNotIn(2, r)
        
    def test_02(self):            
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            r = SortedDict(key_type = key_type, alg = alg, key = key)
            
            r[2] = 'b'
            r[3] = 'c'
            self.assertEqual(r.setdefault(3, 'd'), 'c')
            self.assertEqual(r[3], 'c')
            r.setdefault(4, 'e')
            self.assertEqual(r[4], 'e')            

    def test_03(self):
        for key_type, cnt, alg, key in itertools.product(_key_types, _dict_types, _algs, _keys):
            cnt([(1, 'a')], key_type = key_type, alg = alg, key = key)
            try:
                cnt([1, 'a'], key_type = key_type, alg = alg, key = key)
                self.assertTrue(False)
            except TypeError:
                pass                

    def test_04(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            r = SortedDict(key_type = key_type, alg = alg, key = key)
            r[2] = 'b'
            r[3] = 'c'
            self.assertIn(2, r)
            self.assertEqual(r[2], 'b')
            r[2] = 'f'
            self.assertIn(2, r)
            self.assertEqual(r[2], 'f')
            del r[2]
            self.assertNotIn(2, r)
            try:                       
                del r[2]
                self.assertTrue(False)
            except KeyError:
                pass                

    def test_05(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            r = SortedDict([(2, 'b'), (3, 'c')], key_type = key_type, alg = alg, key = key)
            del r[3: 8]
            self.assertEqual(len(r), 1)

    def test_06(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedDict([(1, 'a'), (2, 'b')], key_type = key_type, alg = alg, key = key)
                        
            self.assertEqual(len(t.keys()), 2)
            self.assertEqual(len(t.values()), 2)
            self.assertEqual(len(t.items()), 2)
            self.assertEqual(len(t.keys(reverse = True)), 2)
            self.assertEqual(len(t.values(reverse = True)), 2)
            self.assertEqual(len(t.items(reverse = True)), 2)
            
            self.assertIn(1, t.keys())
            self.assertIn((1, 'a'), t.items())
            self.assertIn(1, t.keys(reverse = True))
            self.assertIn((1, 'a'), t.items(reverse = True))
            
            del t[1]
            
            self.assertEqual(len(t.keys()), 1)
            self.assertEqual(len(t.values()), 1)
            self.assertEqual(len(t.items()), 1)
            self.assertEqual(len(t.keys(reverse = True)), 1)
            self.assertEqual(len(t.values(reverse = True)), 1)
            self.assertEqual(len(t.items(reverse = True)), 1)
            
            self.assertFalse(1 in t.keys())
            self.assertFalse((1, 'a') in t.items())
            self.assertFalse(1 in t.keys(reverse = True))
            self.assertFalse((1, 'a') in t.items(reverse = True))

    def test_07(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = FrozenSortedDict([(1, 'a')], key_type = key_type, alg = alg, key = key)            
            h = set(t)                

    def test_08(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedDict([(1, 'a')], key_type = key_type, alg = alg, key = key)            
            with self.assertRaises(TypeError):
                h = set([t])

    def test_09(self):                
        for key_type, cnt, alg, key in itertools.product(_key_types, _dict_types, _algs, _keys):
            t = cnt([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, alg = alg, key = key)
            
            v = t.keys()
            self.assertEqual(
                v, 
                SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, key = key).keys())
            
            v = t.keys(3)
            self.assertEqual(
                v, 
                SortedDict([(1, 'a'), (2, 'b')], key_type = key_type, key = key).keys())
            
            v = t.keys(3, reverse = True)
            self.assertEqual(
                v, 
                SortedDict([(2, 'b'), (1, 'a')], key_type = key_type, key = key).keys())
            
            v = t.keys(0, 3)
            self.assertEqual(
                v, 
                SortedDict([(1, 'a'), (2, 'b')], key_type = key_type, key = key).keys())
            
            v = t.keys(0, 23)
            self.assertEqual(
                v, 
                SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, key = key).keys())
            
            t = SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, key = key)
            
            v = t.items()
            self.assertEqual(
                v, 
                SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, key = key).items())
            
            v = t.items(3)
            self.assertEqual(v, SortedDict([(1, 'a'), (2, 'b')], key_type = key_type, key = key).items())
            
            v = t.items(3, reverse = True)
            self.assertEqual(v, SortedDict([(1, 'a'), (2, 'b')], key_type = key_type, key = key).items())
            
            v = t.items(0, 3)
            self.assertEqual(v, SortedDict([(1, 'a'), (2, 'b')], key_type = key_type, key = key).items())
            
            v = t.items(0, 23)
            self.assertEqual(
                v, 
                SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, key = key).items())

    def test_10(self):                
        for key_type, cnt, alg, key in itertools.product(_key_types, _dict_types, _algs, _keys):
            t = cnt({1 : 'a', 2 : 'b'}, key_type = key_type, alg = alg, key = key)
            self.assertEqual(t[1], 'a')                
            self.assertEqual(t[2], 'b')                

    def test_11(self):                
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedDict({1 : 'a', 2 : 'b'}, key_type = key_type, alg = alg, key = key)
            for i in range(2):
                t.popitem()
            self.assertEqual(len(t), 0, len(t))                

    def test_12(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedDict([(1, 'a'), (2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, alg = alg, key = key)
            self.assertEqual(t[1: 2], ('a', ))
            self.assertEqual(t[1: 3], ('a', 'b'))            
            self.assertEqual(t[-1: 888], ('a', 'b', 'c', 'd'))            

    def test_13(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedDict([(2, 'b'), (3, 'c')], key_type = key_type, alg = alg, key = key)
            assert t[2] == 'b'
            del t[2]
            assert 2 not in t
            
            t = SortedDict([(2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, alg = alg, key = key)
            assert t[2] == 'b'
            assert t[3] == 'c'
            assert t[4] == 'd'
            del t[2: 4]
            assert 2 not in t
            assert 3 not in t
            assert 4 in t

            t = SortedDict([(2, 'b'), (3, 'c'), (4, 'd')], key_type = key_type, alg = alg, key = key)
            assert t[2] == 'b'
            assert t[3] == 'c'
            assert t[4] == 'd'
            del t[4: ]
            assert 2 in t
            assert 3 in t
            assert 4 not in t

    def test_15(self):
        for key_type, alg, key in itertools.product(_key_types, _algs, _keys):
            t = SortedDict([(2, 'b'), (3, 'c')], key_type = key_type, alg = alg, key = key)
            self.assertEqual(t.pop(2), 'b')
            self.assertFalse(2 in t)
            self.assertEqual(t.pop(2, 'a'), 'a')
            with self.assertRaises(KeyError):
                t.pop(2)
                
    def test_16(self):                
        for cnt, alg, key in itertools.product(_dict_types, _algs, _keys):
            t = cnt([('d', 4), ('c', 3), ('e', 5)], alg = alg, key = key)                
            self.assertEqual(next(itertools.islice(t.values('c', 'e'), 1, None)), 4)                


class _Test04DictStress(unittest.TestCase, UnittestBase, _StressTest):    
    def _check_tree_ok(self, r_, tree, builtin_):
        _StressTest._check_tree_ok(self, r_, tree, builtin_)
        
        for k, v in tree.items():   
            self.assertEqual(v, builtin_[k])

    def _generic_test(self, num_trees, num_its, num_distinct_items, new_tree_ave_len, weights):
        for key_type, alg, updator, key in itertools.product(_key_types, _algs, _StressTest._updators, _keys):
            key_type = _StressTest.updator_key_type(updator, key_type)
            r_ = _StressTest.updator_r(updator, num_distinct_items)

            print(key_type, alg, key, updator)
            
            with _leak_detector.check_obj_count(100):
                trees = [SortedDict(key_type = key_type, alg = alg, key = key, updator = updator) \
                    for _ in range(num_trees)]
                dicts = [dict() for _ in range(num_trees)]
                
                for _ in range(num_its):     
                    i = random.randrange(0, num_trees)
                    choice = _weighted_choice(weights)                         
                    
                    if choice == 0:
                        len_ = int(random.expovariate(1. / new_tree_ave_len))

                        create_choice = _weighted_choice([1] * 2)
                        if create_choice == 0:
                            items = [(r_(), r_()) for _ in range(len_)]
                            dicts[i] = dict(items)
                            trees[i] = SortedDict(
                                dicts[i].items(), 
                                key_type = key_type, 
                                alg = alg, 
                                key = key,
                                updator = updator)                        
                        elif create_choice == 1:
                            keys = [r_() for _ in range(len_)]
                            value = r_()
                            dicts[i] = SortedDict.fromkeys(keys, value)
                            trees[i] = SortedDict.fromkeys(
                                keys, 
                                value, 
                                key_type = key_type, 
                                alg = alg, 
                                updator = updator)
                        else:
                            self.assertTrue(False, create_choice)                        
                        
                        self._check_tree_ok(r_, trees[i], dicts[i])                    
                        continue
                        
                    tree, dict_ = trees[i], dicts[i]

                    for k in tree:
                        self.assertIn(k, dict_)                                        
                        self.assertEqual(tree[k], dict_[k])

                    key_ = r_()
                    
                    if choice == 1:
                        default = r_()
                        self.assertEqual(
                            tree.get(key_, default), 
                            dict_.get(key_, default), 
                            (tree.get(key_, default), dict_.get(key_, default)))
                        self.assertEqual(key_ in tree, key_ in dict_, (key, tree, dict_))                    
                        if key_ in tree:
                            self.assertIn(key_, dict_)
                            self.assertEqual(tree[key_], dict_[key_], (tree[key_], dict_[key_]))
                            
                        b, e = r_(), r_()
                        self.assertEqual(
                            tree[b: e], 
                            tuple((v for (k, v) in tree.items() if b <= k < e)))
                    elif choice == 2:
                        val = r_()
                        tree[key_] = val
                        dict_[key_] = val
                        self.assertIn(key_, tree)
                    elif choice == 3:
                        if key_ in tree:
                            self.assertIn(key_, dict_)
                            del tree[key_]
                            del dict_[key_]
                        else:
                            self.assertFalse(key in dict_) 
                    else:
                        self.assertTrue(False, choice)
                        
                    for i, tree, dict_ in zip_(range(num_trees), trees, dicts):
                        self._check_tree_ok(r_, tree, dict_)

    def test_00(self):
        num_trees = 5
        num_its = 500
        num_distinct_items = 1000
        
        new_tree_ave_len = 100
        
        new_tree_weight = 10
        find_weight = 150
        insert_weight = 70
        erase_weight = 20
    
        weights = [new_tree_weight, find_weight, insert_weight, erase_weight]

        self._generic_test(num_trees, num_its, num_distinct_items, new_tree_ave_len, weights)
        
        
class Test05KeyTypePathologies(unittest.TestCase, UnittestBase):
    def test_00(self):        
        class Koko(object):
            pass
          
        with warnings.catch_warnings(record=True) as w:            
            SortedSet(key_type = Koko)                    

    def test_01(self):
        with warnings.catch_warnings(record=True) as w:            
            SortedSet(key_type = float, compare = lambda x, y: x - y)
        
    def test_02(self):
        with warnings.catch_warnings(record=True) as w:            
            SortedSet(key_type = float, key = lambda x: x)

    def test_03(self):
        with self.assertRaises(TypeError):
            SortedSet(['1', '3', '2'], updator = MinGapUpdator)        
            
    def test_04(self):
        with self.assertRaises(TypeError):
            SortedSet([1, 3, 2], updator = OverlappingIntervalsUpdator)
            
    def test_05(self):
        with self.assertRaises(TypeError):
            SortedSet(key_type = int, updator = OverlappingIntervalsUpdator)


if __name__ == '__main__':
    unittest.main()




