import os
import sys
import math
import time
import random
import string

import bintrees
sys.path.extend(['..', '../..'])
import banyan


def random_ints(num_items):
    return list(set([random.randint(-999999, 999999) for _ in range(num_items)]))
    
    
def random_strings(num_items, len_):
    return list(set([''.join(random.choice(string.letters) for _ in range(len_)) for _ in range(num_items)]))

