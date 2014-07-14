from __future__ import print_function
import types
import sys
import warnings
import gc
import pprint
import os
import weakref
import gc
import inspect
from contextlib import contextmanager


def _breakdown():
    gc.collect()
    
    break_ = {}
    for o in gc.get_objects():
        break_[type(o)] = (0 if type(o) not in break_ else break_[type(o)]) + 1
    return break_ 


@contextmanager
def check_obj_count(to_worry):
    break_ = _breakdown()
    
    yield
    
    new_break = _breakdown()
    
    for k in new_break.keys():  
        if new_break[k] - break_.setdefault(k, 0) >= to_worry:
            warnings.warn(','.join([str(o) for o in [k, new_break[k], break_[k]]]), RuntimeWarning)























