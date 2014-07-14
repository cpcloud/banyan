#ifndef BANYAN_TREE_HPP
#define BANYAN_TREE_HPP

#include <Python.h>

#include "_dbg.hpp"
#include "_int_int/_tree_imp_base.hpp"

struct Tree
{
    PyObject_HEAD
    
    _TreeImpBase * imp;
};

extern "C" PyTypeObject TreeType;

extern "C" PyObject *
tree_clear(Tree * self);

PyObject * 
seq_to_fast_seq(PyObject * seq, bool pair);

#endif // #ifndef BANYAN_TREE_HPP

