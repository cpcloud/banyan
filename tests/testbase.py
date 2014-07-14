import six
from six import u
from banyan import SortedSet
import unittest_rand_gen_state


@six.with_metaclass(unittest_rand_gen_state.Saver)
class UnittestBase(object):
    pass


class StringTestBase(object):
    def test_construct_bytes(self):
        with self.assertRaises(TypeError):
            t = SortedSet([u('0'), u('3'), u('1')], key_type=six.binary_type)
        t = SortedSet([b'0', b'3', b'1'], key_type=six.binary_type)
        assert b'0' in t

    def test_construct_unicode(self):
        with self.assertRaises(TypeError):
            t = SortedSet([b'0', b'3', b'1'], key_type=six.text_type)
        t = SortedSet([u('0'), u('3'), u('1')], key_type=six.text_type)
        assert u('0') in t
