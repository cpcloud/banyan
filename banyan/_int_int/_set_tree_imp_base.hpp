#ifndef _SET_TREE_IMP_BASE_HPP
#define _SET_TREE_IMP_BASE_HPP

#include <Python.h>

#include <functional>
#include <utility>

#include "_tree_imp_base.hpp"
#include "_dbg.hpp"

class _SetTreeImpBase : 
    public _TreeImpBase
{
public:
    virtual 
    ~_SetTreeImpBase() = 0;

    virtual PyObject *
    insert(PyObject * key) = 0;

    virtual PyObject *
    ext_union(PyObject * o, int type) = 0;

    virtual PyObject *
    ext_cmp(PyObject * o, int type) = 0;

    virtual PyObject *
    erase(PyObject * key) = 0;

    virtual PyObject *
    discard(PyObject * key) = 0;
};

#endif // #ifndef _SET_TREE_IMP_BASE_HPP

