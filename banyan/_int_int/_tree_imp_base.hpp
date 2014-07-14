#ifndef _TREE_IMP_BASE_HPP
#define _TREE_IMP_BASE_HPP

#include <Python.h>

#include <functional>
#include <utility>

#include "_dbg.hpp"

class _TreeImpBase
{
public:
    virtual 
    ~_TreeImpBase() = 0;
    
    virtual int
    traverse(visitproc visit, void * arg) = 0;

    virtual int
    contains(PyObject * key) = 0;

    virtual PyObject *
    erase(PyObject * key) = 0;
    
    virtual PyObject *
    erase_slice(PyObject * start, PyObject * stop) = 0;

    virtual PyObject *
    erase_return(PyObject * key) = 0;

    virtual PyObject *
    clear() = 0;

    virtual PyObject *
    pop() = 0;

    virtual void *
    begin(PyObject * start, PyObject * stop) = 0;

    virtual void *
    rbegin(PyObject * start, PyObject * stop) = 0;

    virtual void *
    next(void * cur, PyObject * stop, int type, PyObject * & cur_val) = 0;

    virtual void *
    prev(void * cur, PyObject * start, int type, PyObject * & cur_val) = 0;

    virtual Py_ssize_t
    size() = 0;

    virtual void *
    root_iter() = 0;

    virtual void *
    left_iter(void * iter) = 0;
    
    virtual void *
    right_iter(void * iter) = 0;
    
    virtual PyObject *
    iter_key(void * it) = 0;
    
    virtual PyObject *
    iter_metadata(void * it) = 0;

    virtual void 
    delete_node_iter(void * node) = 0;        
    
    virtual PyObject *
    lt_keys(PyObject * lhs, PyObject * rhs) = 0;
    
    virtual PyObject *
    rank_updator_kth(size_t k);

    virtual PyObject *
    rank_updator_order(PyObject * key);

    virtual PyObject *
    min_gap_updator_min_gap();

    virtual PyObject *
    interval_max_updator_overlapping(PyObject * b, PyObject * e);

    virtual PyObject *
    interval_max_updator_overlapping(PyObject * p);

#ifdef BANYAN_DEBUG
    virtual void
    assert_valid() const = 0;
#endif // #ifdef BANYAN_DEBUG    
};

#endif // #ifndef _TREE_IMP_BASE_HPP

