from functools import wraps
import inspect
import collections    
import warnings
import types
import sys

from ._dummy_key import _CmpDummyKey as _CmpDummyKey


RED_BLACK_TREE = 0
"""
Red-black tree algorithm indicator; good for general use.
"""
SPLAY_TREE = 1
"""
Splay tree algorithm indicator; good for temporal locality cases.
"""
SORTED_LIST = 2
"""
Sorted list algorithm indicator; good for infrequent updates.
"""
    
    
def _updator_metadata(set_, init_info):    
    if init_info.updator is None:
        return None

    name_clss = [(name, cls) for \
        (name, cls) in inspect.getmembers(init_info.updator, predicate = inspect.isclass) if name == 'Metadata']
    assert len(name_clss) == 1
    cls = name_clss[0][1]
    
    compare = init_info.compare if init_info.compare is not None else \
        lambda x, y: -1 if x < y else (0 if x == y else 1)
    
    def update(m, key, l, r):
        m.update(
            key,
            init_info.key if init_info.key is not None else lambda k: _CmpDummyKey(compare, k),
            l, 
            r)

    return (cls, update)
    
    
def _adopt_updator_methods(self, updator):
    def method_wrapper(f):
        def wrapper(*args, **kwargs):
            return f(self, *args, **kwargs)
        return wraps(f)(wrapper)            

    if updator is None:
        return

    for name, method in inspect.getmembers(updator()):
        if name.find('_') == 0 or name in self.__dict__ or name == 'Metadata':
            continue
        try:
            method_ = method.__func__ if sys.version_info >= (3, 0) else method.im_func
            self.__dict__[name] = method_wrapper(method_)
        except AttributeError:
            warnings.warn(name, RuntimeWarning)

            
_CommonInitInfo = collections.namedtuple(
    '_CommonInitInfo', 
    ['key_type', 'alg', 'key', 'compare', 'updator'], 
    verbose = False)          




