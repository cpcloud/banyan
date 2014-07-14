#include <Python.h>
#include <structmember.h>

#include <iostream>

#include "tree.hpp"
#include "node.hpp"
#include "_int_imp/_pyobject_utils.hpp"
#include "_int_imp/_build_tree_imp.hpp"

using namespace std;

extern "C" PyTypeObject TreeType;

extern "C" int
tree_init(Tree * self, PyObject * args, PyObject * keyword_args)
{
    int alg;
    int mapping;
    PyObject * key_type, * key, * cmp, * metadata, * seq;
    int clear_existing;
    DBG_ONLY(const int parse = )
    PyArg_ParseTuple(
        args, 
        "iOOiOOOi",
        &alg, 
        &seq,
        &key_type, 
        &mapping,
        &metadata,
        &key,
        &cmp, 
        &clear_existing);
#ifdef BANYAN_DEBUG       
    if (
            !parse || 
            seq == NULL || 
            key_type == NULL || 
            (mapping != 0 && mapping != 1) || 
            (clear_existing != 0 && clear_existing != 1) || 
            metadata == NULL ||
            key == NULL ||
            cmp == NULL) {
        WARN(parse);            
        WARN(alg);
        WARN(seq);
        WARN(mapping);
        WARN(metadata);
        WARN(key);
        WARN(cmp);            
        WARN(clear_existing);
            
        return BANYAN_VERIFY_EXCEPTION(-1);
    }  
#endif // #ifdef BANYAN_DEBUG

    if (alg < 0 || alg > 2) {
        PyErr_Format(PyExc_TypeError, "Invalid alg %d", alg);
        
        return BANYAN_VERIFY_EXCEPTION(-1);
    }
     
    PyObject * const fast_seq = seq_to_fast_seq(seq, mapping == 0);
    if (fast_seq == NULL)
        return BANYAN_VERIFY_EXCEPTION(-1);
        
    if (!clear_existing) 
        self->imp = NULL;
    try {
        _TreeImpBase * const imp = _build_tree_imp(
            alg, 
            fast_seq,
            key_type,
            mapping,
            metadata,
            key,
            cmp);
        if (imp == NULL) {
            PyErr_NoMemory();
            BANYAN_PYOBJECT_DECREF(fast_seq);
            return BANYAN_VERIFY_EXCEPTION(-1);
        }
        
        if (clear_existing) 
            delete self->imp;
        self->imp = imp;            
    }
    catch(...) {
        BANYAN_PYOBJECT_DECREF(fast_seq);
        return BANYAN_VERIFY_EXCEPTION(-1);
    }
            
    BANYAN_PYOBJECT_DECREF(fast_seq);
    
    return 0;
}

extern int
tree_gc_clear(Tree * self)
{
    _TreeImpBase * imp = NULL;
    swap(imp, self->imp);
    
    delete imp;
    
    return 0;
}

extern "C" void
tree_dealloc(Tree * self)
{
    tree_gc_clear(self);

    PyObject_GC_UnTrack(self);

    PyObject_GC_Del(self);
}

extern "C" int
tree_traverse(Tree * self, visitproc visit, void * arg)
{
    if (self->imp == NULL) 
        return 0;

    // Tmp Ami - consider next line.
    // DBG_ONLY(self->imp->assert_valid();)
    return self->imp->traverse(visit, arg);
}

extern int
tree_contains(Tree * self, PyObject * key)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        return self->imp->contains(key);
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION(-1);
    }        
}

static Py_ssize_t
tree_size(Tree * self)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    return self->imp->size();
}

extern "C" PyObject *
tree_clear(Tree * self)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    return BANYAN_VERIFY_EXCEPTION(self->imp->clear());
}

extern "C" PyObject *
tree_root(Tree * self)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    return BANYAN_VERIFY_EXCEPTION(node_new(self, self->imp->root_iter()));
}

extern "C" PyObject * 
tree_rank_updator_kth(Tree * self, PyObject * k_)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)    
#if PY_MAJOR_VERSION >= 3
    if (!PyLong_CheckExact(k_)) {
#else // #if PY_MAJOR_VERSION >= 3
    if (!PyInt_CheckExact(k_)) {
#endif // #if PY_MAJOR_VERSION >= 3
        PyErr_SetObject(PyExc_TypeError, k_);            
        return NULL;
    }

#if PY_MAJOR_VERSION >= 3
    const size_t k = PyLong_AsLong(k_);
#else // #if PY_MAJOR_VERSION >= 3
    const size_t k = PyInt_AsLong(k_);
#endif // #if PY_MAJOR_VERSION >= 3        

    if (k < 0) {    
        PyErr_SetObject(
            PyExc_IndexError, 
#if PY_MAJOR_VERSION >= 3
            PyLong_FromLong(k));
#else // #if PY_MAJOR_VERSION >= 3
            PyInt_FromLong(k));            
#endif // #if PY_MAJOR_VERSION >= 3
        return NULL;
    }
    
    return BANYAN_VERIFY_EXCEPTION(self->imp->rank_updator_kth(k));
}

extern "C" PyObject * 
tree_rank_updator_order(Tree * self, PyObject * key)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)    
    return BANYAN_VERIFY_EXCEPTION(self->imp->rank_updator_order(key));
}

extern "C" PyObject * 
tree_min_gap_updator_min_gap(Tree * self)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)    
    return BANYAN_VERIFY_EXCEPTION(self->imp->min_gap_updator_min_gap());
}

extern "C" PyObject * 
tree_interval_max_updator_overlapping_range(Tree * self, PyObject * interval)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)    
    PyObject * const b = PySequence_GetItem(interval, 0);
    PyObject * const e = PySequence_GetItem(interval, 1);
    if (b == NULL || e == NULL) {
        PyErr_SetObject(PyExc_TypeError, interval);                    
        return NULL;
    }
    return BANYAN_VERIFY_EXCEPTION(self->imp->interval_max_updator_overlapping(b, e));
}

extern "C" PyObject * 
tree_interval_max_updator_overlapping(Tree * self, PyObject * p)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)    
    return BANYAN_VERIFY_EXCEPTION(self->imp->interval_max_updator_overlapping(p));
}

static PySequenceMethods tree_seq_methods = {
    // sq_length
    (lenfunc)tree_size,
    // sq_concat
    0,                                  
    // sq_repeat
    0,                                  
    // sq_item
    0,                                  
    // sq_slice
    0,                                  
    // sq_ass_item
    0,                                  
    // sq_ass_slice
    0,                                  
    // sq_contains
    (objobjproc)tree_contains,           
};

// Tmp Ami - go over all of these and see which can user fail. Add to reg.
static PyMethodDef tree_methods[] = {
    { "root", (PyCFunction)tree_root, METH_NOARGS, "" },
    { "_rank_updator_kth", (PyCFunction)tree_rank_updator_kth, METH_O, "" },
    { "_rank_updator_order", (PyCFunction)tree_rank_updator_order, METH_O, "" },
    { "_min_gap_updator_min_gap", (PyCFunction)tree_min_gap_updator_min_gap, METH_NOARGS, "" },
    { "_interval_max_updator_overlapping", (PyCFunction)tree_interval_max_updator_overlapping, METH_O, "" },
    { "_interval_max_updator_overlapping_range", 
        (PyCFunction)tree_interval_max_updator_overlapping_range, METH_O, "" },
    { NULL, NULL}
};

static PyMemberDef tree_members[] = {
    { NULL }
};

PyDoc_STRVAR(tree_type_doc, "");

PyTypeObject TreeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/
    "banyan_c.Tree",                                                        
    /*tp_basicsize*/
    sizeof(Tree),                                                           
    /*tp_valuesize*/
    0,                                                                      
    /* methods */
    /*tp_dealloc*/
    (destructor)tree_dealloc,          
    /*tp_print*/
    0,                                         
    /*tp_getattr*/
    0,                                         
    /*tp_setattr*/
    0,                                         
    /*tp_less_than*/
    0,                                        
    /*tp_repr*/  
    0,                                      
    /*tp_as_number*/  
    0,                                      
    /*tp_as_sequence*/
    &tree_seq_methods,                      
    /*tp_as_mapping*/                
    0,                            
    /*tp_hash*/          
    0,                         
    /*tp_call*/   
    0,                         
    /*tp_str*/
    0,                            
    /*tp_getattro*/
    0,                                      
    /*tp_setattro*/
    0,                                      
    /*tp_as_buffer*/
    0,                                      
    /*tp_flags*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE,                     
    /*tp_doc*/
    tree_type_doc,                     
    /*tp_traverse*/
    (traverseproc)tree_traverse,       
    /*tp_clear*/
    (inquiry)tree_gc_clear,               
    /*tp_richless_than*/
    0,                                      
    /*tp_weaklistoffset*/
    0,                      
    /*tp_iter*/                
    0,                      
    /*tp_iternext*/
    0,                              
    /*tp_methods*/
    tree_methods,                   
    /*tp_members*/   
    tree_members,            
    /* tp_getset */       
    0,                       
    /* tp_base */
    0,                       
    /* tp_dict */
    0,                       
    /* tp_descr_get */
    0,                       
    /* tp_descr_set */
    0,                       
    /* tp_dictoffset */
    0,                       
    /* tp_init */
    (initproc)tree_init,      
    /* tp_alloc */
    0,                         
    /* tp_new */
    PyType_GenericNew,                 
};

PyObject * 
seq_to_fast_seq(PyObject * seq, bool pair)
{
    if (seq == Py_None) {
        BANYAN_PYOBJECT_DUMMY_INCREF(Py_None);
        return Py_None;
    }        
        
    PyObject * const fast_seq = PySequence_Fast(seq, "Sequence expected");
    if (fast_seq == NULL) { 
        PyErr_NoMemory();
        return NULL;
    }        
    BANYAN_PYOBJECT_DUMMY_INCREF(fast_seq);                        
    
    if (PySequence_Fast_GET_SIZE(fast_seq) == 0) {
        BANYAN_PYOBJECT_DECREF(fast_seq);
        BANYAN_PYOBJECT_DUMMY_INCREF(Py_None);
        return Py_None;
    }

    if (!pair)             
        return fast_seq;
        
    for (size_t i = 0; i < static_cast<size_t>(PySequence_Fast_GET_SIZE(fast_seq)); ++i) {
        PyObject * const p = PySequence_Fast_GET_ITEM(fast_seq, i);    
        if (PyTuple_Check(BANYAN_PYOBJECT_VERIFY(p)) && PyTuple_Size(p) == 2) 
            continue;

        PyErr_SetObject(PyExc_TypeError, PySequence_Fast_GET_ITEM(fast_seq, i));
        BANYAN_PYOBJECT_DECREF(fast_seq);
        return NULL;
    }               
        
    return fast_seq;             
}
