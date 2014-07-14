#include <Python.h>
#include <structmember.h>

#include <iostream>

#include "set_tree.hpp"
#include "_int_int/_set_tree_imp_base.hpp"
#include "_int_imp/_pyobject_utils.hpp"

using namespace std;

extern "C" PyTypeObject SetTreeType;

inline static _SetTreeImpBase *
_imp(SetTree * self)
{
    DBG_ASSERT(dynamic_cast<_SetTreeImpBase *>(self->imp) != NULL);
    return static_cast<_SetTreeImpBase *>(self->imp);
}

extern "C" int
set_tree_init(SetTree * self, PyObject * args, PyObject * keyword_args)
{
    return TreeType.tp_init((PyObject *)self, args, keyword_args);
}

extern "C" PyObject *
set_tree_insert(SetTree * self, PyObject * key)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)_imp(self)->insert(key));
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

extern "C" PyObject *
set_tree_pop(Tree * self)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)self->imp->pop());
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

extern "C" PyObject *
set_tree_discard(SetTree * self, PyObject * key)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)_imp(self)->discard(key));
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

PyDoc_STRVAR(
    discard_doc, 
    "\
    \n\
    :param item: Item which should be removed.\n\
    \n\
    Removes item from the set if it is present.\n\
    \n\
    Example:\n\
    \n\
    >>> t = SortedSet()\n\
    >>> t.remove(3)\n\
    Traceback (most recent call last):\n\
        ...\n\
    KeyError: 3\n\
    >>> t.discard(3)\n\
    ");

extern "C" PyObject *
set_tree_erase(SetTree * self, PyObject * args, PyObject * keyword_args)
{
    DBG_ASSERT(self->imp != NULL);
    DBG_ONLY(self->imp->assert_valid();)
    try {
        PyObject * key;    
        PyObject * stop = NULL;    
        if (!PyArg_ParseTuple(
                args, 
                "O|O", 
                &key, &stop))
            return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);            
            
        if (stop != NULL)        
            return BANYAN_VERIFY_EXCEPTION((PyObject *)self->imp->erase_slice(key, stop));

        return BANYAN_VERIFY_EXCEPTION((PyObject *)self->imp->erase(key));
    }
    catch(...) {
        return BANYAN_VERIFY_EXCEPTION((PyObject *)NULL);
    }        
}

PyDoc_STRVAR(
    remove_doc, 
    "\n\
    Removes an item or items.\n\
    \n\
    :param item: If only a single parameter is given, then this the item which should be removed. \n\
        If two parameters are given, then all items at least as large as this one should be removed. If two \n\
        parameters are given, and this one is None, then all items smaller than the second parameter \n\
        will be removed\n\
    \n\
    :param stop: optional parameter, indicating, the smallest element that should \n\
        be retained. If this parameter is None, then all items at least as large as the first parameter\n\
        will be removed.\n\
    \n\
    :raises: :py:exc:`KeyError` if only a single parameter is given the set contains no such key.\n\
    \n\
    Remove Single Item Example:\n\
    \n\
    >>> t = SortedSet()\n\
    >>> assert 1 not in t\n\
    >>> t.add(1)\n\
    >>> assert 1 in t\n\
    >>> t.remove(1)\n\
    >>> assert 1 not in t\n\
    >>> t.remove(1)\n\
    Traceback (most recent call last):\n\
        ...\n\
    KeyError: 1\n\
    \n\
    Remove Range Examples:\n\
    \n\
    >>> t = SortedSet([1, 3, 2, 5, 4, 6])\n\
    >>> t\n\
    SortedSet([1, 2, 3, 4, 5, 6])\n\
    >>> # Remove everything in the range [2, 5)\n\
    >>> t.remove(2, 5)\n\
    >>> t\n\
    SortedSet([1, 5, 6])\n\
    >>>\n\
    >>> t = SortedSet([1, 3, 2, 5, 4, 6])\n\
    >>> t\n\
    SortedSet([1, 2, 3, 4, 5, 6])\n\
    >>> # Remove everything in the range [2, inf)\n\
    >>> t.remove(2, None)\n\
    >>> t\n\
    SortedSet([1])\n\
    >>>\n\
    >>> t = SortedSet([1, 3, 2, 5, 4, 6])\n\
    >>> t\n\
    SortedSet([1, 2, 3, 4, 5, 6])\n\
    >>> # Remove everything in the range [-inf, 5)\n\
    >>> t.remove(None, 5)\n\
    >>> t\n\
    SortedSet([5, 6])\n\
    >>>\n\
    >>> t = SortedSet([1, 3, 2, 5, 4, 6])\n\
    >>> t\n\
    SortedSet([1, 2, 3, 4, 5, 6])\n\
    >>> # Remove everything in the range [-inf, inf)\n\
    >>> t.remove(None, None)\n\
    >>> t\n\
    SortedSet([])\n\
    ");
    
PyDoc_STRVAR(
    clear_doc, 
    "\
    Clears all items.\n\
    \n\
    Example:\n\
    \n\
    >>> t = SortedSet([1, 2, 3])\n\
    >>> t.clear()\n\
    >>> t\n\
    SortedSet([])");

static PyMethodDef set_tree_methods[] = {
    { "clear", (PyCFunction)tree_clear, METH_NOARGS, clear_doc },
    { "remove", (PyCFunction)set_tree_erase, METH_VARARGS, remove_doc },
    { "discard", (PyCFunction)set_tree_discard, METH_O, discard_doc },
    { "add", (PyCFunction)set_tree_insert, METH_O, "" },
    { "pop", (PyCFunction)set_tree_pop, METH_NOARGS, "" },
    { NULL, NULL}
};

static PyMemberDef set_tree_members[] = {
    { NULL }
};

PyDoc_STRVAR(set_tree_type_doc, "");

PyTypeObject SetTreeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/
    "banyan_c.SetTree",                                                        
    /*tp_basicsize*/
    sizeof(SetTree),                                                        
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
    set_tree_type_doc,                     
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
    set_tree_methods,                   
    /*tp_members*/   
    set_tree_members,        
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
    (initproc)set_tree_init,   
    /* tp_alloc */   
    0,                 
    /* tp_new */        
    0,                 
};
