#include <Python.h>
#include <structmember.h>

#include <iostream>

#include "dict_tree.hpp"
#include "_int_int/_dict_tree_imp_base.hpp"
#include "_int_imp/_pyobject_utils.hpp"
#include "_int_imp/_pyobject_utils.hpp"

using namespace std;

extern "C" PyTypeObject DictTreeType;

inline static _DictTreeImpBase *
_imp(DictTree * self)
{
    DBG_ASSERT(dynamic_cast<_DictTreeImpBase *>(self->imp) != NULL);
    return static_cast<_DictTreeImpBase *>(self->imp);
}

extern "C" int
dict_tree_init(DictTree * self, PyObject * args, PyObject * keyword_args)
{
    return TreeType.tp_init((PyObject *)self, args, keyword_args);
}

extern "C" int
dict_tree_update_sub(DictTree * self, PyObject * key, PyObject * data)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    
    try {
        if (data == NULL) {
            if (!PySlice_Check(key))    
                return _imp(self)->erase(key) == NULL;
                
            if (reinterpret_cast<PySliceObject *>(key)->step != Py_None) {
                PyErr_SetObject(PyExc_TypeError, key);            
                return BANYAN_VERIFY_EXCEPTION(-1);
            }
            
            return _imp(self)->erase_slice(
                reinterpret_cast<PySliceObject *>(key)->start, 
                reinterpret_cast<PySliceObject *>(key)->stop) == NULL;
        }        
        
        if (!PySlice_Check(key))    
            return _imp(self)->insert(key, data, true) == NULL;

        if (reinterpret_cast<PySliceObject *>(key)->step != Py_None) {
            PyErr_SetObject(PyExc_TypeError, key);            
            return BANYAN_VERIFY_EXCEPTION(-1);
        }
        
        PyObject * const data_fast_seq = seq_to_fast_seq(data, false);
        if (data_fast_seq == NULL)
            return BANYAN_VERIFY_EXCEPTION(-1);
        // Tmp Ami - check update of empty range - this might fail.       
        DBG_ASSERT(data_fast_seq != Py_None);        
        
        const int ret = _imp(self)->update_slice_data(
            reinterpret_cast<PySliceObject *>(key)->start, 
            reinterpret_cast<PySliceObject *>(key)->stop,
            data);
            
        BANYAN_PYOBJECT_DECREF(data_fast_seq);
        return ret;
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION(-1);
    }        
}

extern "C" PyObject *
dict_tree_insert_non_new(DictTree * self, PyObject * args)
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
            
        return BANYAN_VERIFY_EXCEPTION(_imp(self)->insert(key, data, false));
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

PyDoc_STRVAR(
    setdefault_doc, 
    "\
    :param key: Key to be mapped\n\
    :param default: Data to be mapped to key, if key is not in the dict (default ``None``).\n\
    \n\
    Sets the value mapped by key to default, unless there is such a key already.\n\
    \n\
    :returns: value mapped by key after this operation completes.\n\
    \n\
    Example:\n\
    \n\
    >>> t = SortedDict([(1, 'a')])\n\
    >>> t.setdefault(1, 'b')\n\
    'a'\n\
    >>> t.setdefault(2, 'b')\n\
    'b'");

extern "C" PyObject *
dict_tree_get_default(DictTree * self, PyObject * args)
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
            
        return BANYAN_VERIFY_EXCEPTION(_imp(self)->get(key, data));
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
    >>> t = SortedDict([(2, 'b'), (3, 'c')])\n\
    >>> assert t.get(2, 'a') == 'b'\n\
    >>> assert t.get(0, 'a') == 'a'");

extern "C" PyObject *
dict_tree_pop(DictTree * self, PyObject * args)
{
    DBG_ASSERT(self->imp != NULL);
    PyObject * key, * data = NULL;
    try {
        DBG_ONLY(self->imp->assert_valid();)
        if (!PyArg_ParseTuple(
                args, 
                "O|O",
                &key, &data))
            return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
            
        return BANYAN_VERIFY_EXCEPTION(_imp(self)->pop(key, data));
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

PyDoc_STRVAR(
    pop_doc, 
    "\
    :param key: Key.\n\
    :param default: optional default value\n\
    :raises: :py:exc:`KeyError` if default is not given, and the dict contains no such key.\n\
    \n\
    Example:\n\
    \n\
    >>> t = SortedDict([(2, 'b'), (3, 'c')])\n\
    >>> assert t.pop(2) == 'b'\n\
    >>> assert 2 not in t\n\
    >>> assert t.pop(2, 'a') == 'a'\n\
    >>> t.pop(2)\n\
    Traceback (most recent call last):\n\
      ...\n\
    KeyError: 2");

extern "C" PyObject *
dict_tree_pop_val(DictTree * self)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        return BANYAN_VERIFY_EXCEPTION(self->imp->pop());
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

PyDoc_STRVAR(
    popitem_doc, 
    " \
    Removes an arbitrary item from the dict.\n\
     \n\
    :returns: item removed.\n\
    \n\
    :raises: :py:exc:`KeyError` if the dict is empty.\n\
    \n\
    Example:\n\
    \n\
    >>> t = SortedDict([(1, 'a'), (2, 'b'), (3, 'c')])\n\
    >>> len(t)\n\
    3\n\
    >>> t.popitem()\n\
    (1, 'a')\n\
    >>> assert len(t) == 2");

static PyMappingMethods dict_tree_mapping_methods = {
    /*mp_length*/
    0, 
    /*mp_subscript*/
    0, 
    /*mp_ass_subscript*/
    (objobjargproc)dict_tree_update_sub, 
};

PyDoc_STRVAR(
    clear_doc, 
    "\
    Clears all items.\n\
    \n\
    Example:\n\
    \n\
    >>> t = SortedDict([(1, 'a')])\n\
    >>> t.clear()\n\
    >>> t\n\
    SortedDict({})\n\
    ");

static PyMethodDef dict_tree_methods[] = {
    { "clear", (PyCFunction)tree_clear, METH_NOARGS, clear_doc },
    { "setdefault", (PyCFunction)dict_tree_insert_non_new, METH_VARARGS, setdefault_doc },
    { "get", (PyCFunction)dict_tree_get_default, METH_VARARGS, get_doc },
    { "pop", (PyCFunction)dict_tree_pop, METH_VARARGS, pop_doc },
    { "popitem", (PyCFunction)dict_tree_pop_val, METH_NOARGS, popitem_doc },
    { NULL, NULL}
};

static PyMemberDef dict_tree_members[] = {
    { NULL }
};

PyDoc_STRVAR(dict_tree_type_doc, "");

PyTypeObject DictTreeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/
    "banyan_c.DictTree",                                                        
    /*tp_basicsize*/
    sizeof(DictTree),                                                       
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
    &dict_tree_mapping_methods,   
    /*tp_hash*/                                   
    PyObject_HashNotImplemented,                         
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
    dict_tree_type_doc,                     
    /*tp_traverse*/
    0,       
+    /*tp_clear*/
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
    dict_tree_methods,                   
    /*tp_members*/   
    dict_tree_members,       
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
    (initproc)dict_tree_init,  
    /* tp_alloc */    
    0,                 
    /* tp_new */        
    0,                 
};
