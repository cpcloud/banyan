#ifndef _DICT_TREE_IMP_BASE_HPP
#define _DICT_TREE_IMP_BASE_HPP

#include <Python.h>

#include <functional>
#include <utility>

#include "_tree_imp_base.hpp"
#include "_dbg.hpp"

class _DictTreeImpBase : 
    public _TreeImpBase
{
public:
    virtual 
    ~_DictTreeImpBase() = 0;
        
    virtual PyObject *
    insert(PyObject * key, PyObject * data, bool overwrite) = 0;

    virtual PyObject *
    find(PyObject * key) = 0;

    virtual PyObject *
    find_slice(PyObject * start, PyObject * stop) = 0;

    virtual PyObject *
    get(PyObject * key, PyObject * data) = 0;

    virtual PyObject *
    pop(PyObject * key, PyObject * data) = 0;

    virtual int
    update_slice_data(PyObject * start, PyObject * stop, PyObject * data) = 0;
};

#endif // #ifndef _DICT_TREE_IMP_BASE_HPP

