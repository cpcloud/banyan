#!/usr/bin/env python

from __future__ import print_function

import os
from fnmatch import fnmatch

from setuptools import setup, Extension


def find_recursive(pattern, package='banyan'):
    for r, _, fs in os.walk(package):
        for f in fs:
            if fnmatch(f, pattern):
                yield os.path.join(r, f)


banyan_c = Extension('banyan_c',
                     include_dirs=['/usr/local/include', 'banyan'],
                     language='c++',
                     depends=list(find_recursive('*.hpp')),
                     sources=list(find_recursive('*.cpp')))


def read_text(filename):
    with open(filename, 'rt') as f:
        return f.read()


setup(
    name='Banyan',
    version='0.1.5',
    author='Ami Tavory',
    author_email='atavory at gmail.com',
    maintainer='Phillip Cloud',
    maintainer_email='cpcloud at gmail.com',
    packages=['banyan'],
    url='https://github.com/cpcloud/banyan',
    license='BSD',
    description=('Highly-optimized search trees (red-black, splay, and '
                 'sorted-list) with optional augmentation '
                 '(dynamic order statistics, interval trees, etc.)'),
    long_description=read_text('README.rst'),
    requires=['UnittestRandGenState'],
    ext_modules=[banyan_c],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        'Topic :: Software Development :: Libraries :: Python Modules'])
