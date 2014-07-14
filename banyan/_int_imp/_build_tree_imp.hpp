#ifndef _BUILD_TREE_IMP_HPP
#define _BUILD_TREE_IMP_HPP

#include <Python.h>

#include "_int_int/_tree_imp_base.hpp"

_TreeImpBase *
_build_tree_imp(
    int alg, 
    PyObject * seq,
    PyObject * KeyType, 
    int mapping,
    PyObject * metadata, 
    PyObject * key,
    PyObject * cmp);
    
#endif // #ifndef _BUILD_TREE_IMP_HPP

