#include <Python.h>
#include <structmember.h>

#include <iostream>

#include "frozen_set_tree.hpp"
#include "_int_int/_set_tree_imp_base.hpp"
#include "_int_imp/_pyobject_utils.hpp"

using namespace std;

extern "C" PyTypeObject FrozenSetTreeType;

inline static _SetTreeImpBase *
_imp(FrozenSetTree * self)
{
    DBG_ASSERT(dynamic_cast<_SetTreeImpBase *>(self->imp) != NULL);
    return static_cast<_SetTreeImpBase *>(self->imp);
}

extern "C" int
frozen_set_tree_init(FrozenSetTree * self, PyObject * args, PyObject * keyword_args)
{
    return TreeType.tp_init((PyObject *)self, args, keyword_args);
}

extern "C" PyObject *
frozen_set_tree_ext_union(FrozenSetTree * self, PyObject * args)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        PyObject * o;
        int type;
        DBG_VERIFY(PyArg_ParseTuple(
            args, 
            "Oi",
            &o, &type));
        DBG_ASSERT(o != NULL);
        DBG_ASSERT(0 <= type && type <= 3);        
        
        PyObject * const fast_seq = seq_to_fast_seq(o, false);
        if (fast_seq == NULL)
            return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);

        PyObject * const ret = BANYAN_VERIFY_EXCEPTION((PyObject *)_imp(self)->ext_union(fast_seq, type));
        BANYAN_PYOBJECT_DECREF(fast_seq);
        return ret;
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

extern "C" PyObject *
frozen_set_tree_ext_cmp(FrozenSetTree * self, PyObject * args)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        PyObject * o;
        int type;
        DBG_VERIFY(PyArg_ParseTuple(
            args, 
            "Oi",
            &o, &type));
        DBG_ASSERT(o != NULL);
        DBG_ASSERT(0 <= type && type <= 3);        
        
        PyObject * const fast_seq = seq_to_fast_seq(o, false);
        if (fast_seq == NULL)
            return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);

        PyObject * const ret = BANYAN_VERIFY_EXCEPTION(_imp(self)->ext_cmp(fast_seq, type));
        BANYAN_PYOBJECT_DECREF(fast_seq);
        return ret;
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

static PyMethodDef frozen_set_tree_methods[] = {
    { "_ext_union", (PyCFunction)frozen_set_tree_ext_union, METH_VARARGS, "" },
    { "_ext_cmp", (PyCFunction)frozen_set_tree_ext_cmp, METH_VARARGS, "" },
    { NULL, NULL}
};

static PyMemberDef frozen_set_tree_members[] = {
    { NULL }
};

PyDoc_STRVAR(frozen_set_tree_type_doc, "");

PyTypeObject FrozenSetTreeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/
    "banyan_c.FrozenSetTree",                                                        
    /*tp_basicsize*/
    sizeof(FrozenSetTree),                                                        
    /*tp_valuesize*/   
    0,                                                                      
    /* methods */
    /*tp_dealloc*/
    0,          
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
    0,                                      
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                     
    /*tp_doc*/
    frozen_set_tree_type_doc,                     
    /*tp_traverse*/
    0,       
    /*tp_clear*/
    0,               
    /*tp_richless_than*/
    0,                                      
    /*tp_weaklistoffset*/
    0,                      
    /*tp_iter*/                
    0,                      
    /*tp_iternext*/
    0,                              
    /*tp_methods*/
    frozen_set_tree_methods,                   
    /*tp_members*/   
    frozen_set_tree_members,        
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
    (initproc)frozen_set_tree_init,   
    /* tp_alloc */   
    0,                 
    /* tp_new */        
    0,                 
};
