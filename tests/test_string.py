from six import u, text_type, binary_type
from banyan import SortedSet


class TestString(object):
    def test_construct_bytes(self):
        with self.assertRaises(TypeError):
            t = SortedSet([u('0'), u('3'), u('1')], key_type=binary_type)
        t = SortedSet([b'0', b'3', b'1'], key_type=binary_type)
        assert b'0' in t

    def test_construct_unicode(self):
        with self.assertRaises(TypeError):
            t = SortedSet([b'0', b'3', b'1'], key_type=text_type)
        t = SortedSet([u('0'), u('3'), u('1')], key_type=text_type)
        assert u('0') in t
