import unittest_rand_gen_state
from banyan import *
    

class UnittestBase(metaclass = unittest_rand_gen_state.Saver):
    pass


class StringTestBase:
    def test_00(self):
        t = SortedSet([b'0', b'3', b'1'], key_type = bytearray)
        assert b'0' in t

    def test_01(self):
        with self.assertRaises(TypeError):
            t = SortedSet([b'0', '3', '1'], key_type = str)
        t = SortedSet(['0', '3', '1'], key_type = str)
    

