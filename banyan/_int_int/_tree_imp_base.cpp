#include <Python.h>

#include "_tree_imp_base.hpp"
#include "_dbg.hpp"

_TreeImpBase::~_TreeImpBase()
{
    // Do nothing.
}

PyObject *
_TreeImpBase::rank_updator_kth(size_t)
{
    DBG_VERIFY(false);
    Py_RETURN_NONE;
}

PyObject *
_TreeImpBase::rank_updator_order(PyObject *)
{
    DBG_VERIFY(false);
    Py_RETURN_NONE;
}

PyObject *
_TreeImpBase::min_gap_updator_min_gap()
{
    DBG_VERIFY(false);
    Py_RETURN_NONE;
}

PyObject *
_TreeImpBase::interval_max_updator_overlapping(PyObject * b, PyObject * e)
{
    DBG_VERIFY(false);
    Py_RETURN_NONE;
}

PyObject *
_TreeImpBase::interval_max_updator_overlapping(PyObject * p)
{
    DBG_VERIFY(false);
    Py_RETURN_NONE;
}

