#include <Python.h>
#include <structmember.h>

#include <iostream>

#include "dict_tree.hpp"
#include "_int_int/_dict_tree_imp_base.hpp"
#include "_int_imp/_pyobject_utils.hpp"

using namespace std;

extern "C" PyTypeObject FrozenDictTreeType;

inline static _DictTreeImpBase *
_imp(FrozenDictTree * self)
{
    DBG_ASSERT(dynamic_cast<_DictTreeImpBase *>(self->imp) != NULL);
    return static_cast<_DictTreeImpBase *>(self->imp);
}

extern "C" int
frozen_dict_tree_init(FrozenDictTree * self, PyObject * args, PyObject * keyword_args)
{
    return TreeType.tp_init((PyObject *)self, args, keyword_args);
}

extern "C" PyObject *
frozen_dict_tree_sub(FrozenDictTree * self, PyObject * key)
{
    try {
        DBG_ASSERT(self->imp != NULL);
        DBG_ONLY(_imp(self)->assert_valid();)
        if (!PySlice_Check(key))    
            return BANYAN_VERIFY_EXCEPTION((PyObject *)_imp(self)->find(key));
            
        if (reinterpret_cast<PySliceObject *>(key)->step != Py_None) {
            PyErr_SetObject(PyExc_TypeError, key);            
            return NULL;
        }
        
        return BANYAN_VERIFY_EXCEPTION((PyObject *)_imp(self)->find_slice(
            reinterpret_cast<PySliceObject *>(key)->start, 
            reinterpret_cast<PySliceObject *>(key)->stop));
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

extern "C" PyObject *
frozen_dict_tree_get_default(DictTree * self, PyObject * args)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        PyObject * key, * data = Py_None;
        if (!PyArg_ParseTuple(
                args, 
                "O|O",
                &key, &data))
            return NULL;
            
        return BANYAN_VERIFY_EXCEPTION((PyObject *)_imp(self)->get(key, data));
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

PyDoc_STRVAR(
    get_doc, 
    "\
    :param key: Key\n\
    :param default: default value\n\
    :returns: default if key is not in the dict, otherwise value mapped by key.\n\
    \n\
    Examples:\n\
    \n\
    >>> t = FrozenSortedDict([(2, 'b'), (3, 'c')])\n\
    >>> assert t.get(2, 'a') == 'b'\n\
    >>> assert t.get(0, 'a') == 'a'");

static PyMappingMethods frozen_dict_tree_mapping_methods = {
    /*mp_length*/
    0, 
    /*mp_subscript*/
    (binaryfunc)frozen_dict_tree_sub, 
    /*mp_ass_subscript*/
    0, 
};

static PyMethodDef frozen_dict_tree_methods[] = {
    { "get", (PyCFunction)frozen_dict_tree_get_default, METH_VARARGS, get_doc },
    { NULL, NULL}
};

static PyMemberDef frozen_dict_tree_members[] = {
    { NULL }
};

PyDoc_STRVAR(frozen_dict_tree_type_doc, "");

PyTypeObject FrozenDictTreeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/
    "banyan_c.FrozenDictTree",                                                        
    /*tp_basicsize*/
    sizeof(FrozenDictTree),                                                       
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
    &frozen_dict_tree_mapping_methods,   
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
    frozen_dict_tree_type_doc,                     
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
    frozen_dict_tree_methods,                   
    /*tp_members*/   
    frozen_dict_tree_members,       
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
    (initproc)frozen_dict_tree_init,  
    /* tp_alloc */    
    0,                 
    /* tp_new */        
    0,                 
};

