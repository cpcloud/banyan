import unittest_rand_gen_state
from banyan import *
    

class UnittestBase(object):
    __metaclass__ = unittest_rand_gen_state.Saver
    
    
class StringTestBase:
    def test_00(self):
        t = SortedSet(['0', '3', '1'], key_type = str)
        assert '0' in t

    def test_01(self):
        with self.assertRaises(TypeError):
            t = SortedSet(['0', '3', '1'], key_type = unicode)
        t = SortedSet([u'0', u'3', u'1'], key_type = unicode)
    

