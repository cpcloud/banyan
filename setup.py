#!/usr/bin/env python


from __future__ import print_function
from distutils.core import setup, Extension, Command
from distutils.command.build_ext import build_ext
import os
import os.path
import glob
import sys
import shutil


_python_str = 'python3' if sys.version_info >= (3, 0) else 'python'


class _DocumentCommand(Command):
    user_options = [
        ('clean', 'c', 'clean'),
        ('aspell', 'a', 'run aspell')]

    def initialize_options(self):
        self._dir = os.getcwd()
        self.aspell = False
        self.clean = False

    def finalize_options(self):
        pass

    def run(self):
        try:            
            shutil.copy('README.txt', 'docs/')
            shutil.copy('README.txt', 'docs/source')
            
            for f_name in glob.glob(os.path.join('performance_tests', '*.py')):
                shutil.copy(f_name, 'docs/source')

            os.chdir('docs')
            
            if self.clean:
                os.system('make clean')
            os.system('make html')
                    
            for f_name in glob.iglob('*.html'):
                os.system("perl -p -i -e 's/pypi\/banyan/pypi\/Banyan/g' %s" % f_name)

            if self.aspell:
                for f_name in glob.iglob('*.html'):
                    os.system('aspell check %s' % f_name)

            os.system('cp -r build/html/* .')
            
            if os.path.exists('docs.zip'):
                os.remove('docs.zip')
            os.system('zip -r docs.zip *')
        finally:        
            os.chdir('..')


class _TestCommand(Command):
    user_options = []

    def initialize_options(self):
        self._dir = os.getcwd()

    def finalize_options(self):
        pass

    def run(self):
        try:
            os.chdir('tests')
            run_str = '%s __init__.py' % _python_str
            os.system(run_str)
        finally:        
            os.chdir('..')


class _PerformanceTestCommand(Command):
    user_options = [
        ('no-clean', 'n', 'no clean')]

    def initialize_options(self):
        self._dir = os.getcwd()
        self.no_clean = False

    def finalize_options(self):
        pass

    def run(self):
        try:
            os.chdir('performance_tests')

            os.system('%s -OO __init__.py' % _python_str)

            os.system('mv *.png ../docs/source')
            
            if not self.no_clean:
                os.system('rm *.so')
                os.system('rm *.o')
                os.system('rm *_wrap.c')
                os.system('rm *.csv')
                os.system('rm *.txt')
                os.system('rm *.dat')
        finally:        
            os.chdir('..')


banyan_c = Extension('banyan_c',
    include_dirs = ['/usr/local/include', 'banyan'],
    language = 'c++',
    depends = [os.path.join(r, f) for r, d, fs in os.walk('banyan') for f in fs if f.endswith('.hpp')],
    sources = [os.path.join(r, f) for r, d, fs in os.walk('banyan') for f in fs if f.endswith('.cpp')])


setup(
    name = 'Banyan',
    version = '0.1.5',
    author = 'Ami Tavory',
    author_email = 'atavory at gmail.com',
    packages = ['banyan'],
    url = 'http://pypi.python.org/pypi/banyan',
    license = 'BSD',
    description = 'Highly-optimized search trees (red-black, splay, and sorted-list) with optional augmentation (dynamic order statistics, interval trees, etc.)',
    long_description = open('README.txt').read(),
    requires = ['UnittestRandGenState'],
    ext_modules = [banyan_c],
    cmdclass = { 
        'test': _TestCommand, 
        'performance_test': _PerformanceTestCommand,
        'document': _DocumentCommand},
    classifiers = [
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',   
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        'Topic :: Software Development :: Libraries :: Python Modules'])





