#include <Python.h>
#include <structmember.h>

#include <iostream>

#include "tree.hpp"
#include "_int_imp/_pyobject_utils.hpp"

struct TreeView
{
    PyObject_HEAD
    
    int start_valid, stop_valid;
    PyObject * start, * stop;

    int forward;
    int type;    
    Tree * tree;
    
    void * it;
};

extern "C" PyTypeObject TreeViewType;

extern "C" int
tree_view_traverse(TreeView * self, visitproc visit, void * arg)
{
    Py_VISIT(self->tree);
    return 0;
}

extern "C" void
tree_view_dealloc(TreeView * self)
{
    BANYAN_PYOBJECT_DECREF((PyObject *)self->tree);
    
    if (self->start_valid)
        BANYAN_PYOBJECT_DECREF(self->start);
    if (self->stop_valid)
        BANYAN_PYOBJECT_DECREF(self->stop);

    PyObject_GC_UnTrack(self);

    PyObject_GC_Del(self);
}

PyObject *
tree_view_new(PyTypeObject * type, PyObject * args, PyObject * keyword_args)
{
    TreeView * const self = PyObject_GC_New(TreeView, &TreeViewType);
    if (self == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    
    self->tree = NULL;
    DBG_VERIFY(PyArg_ParseTuple(
        args, 
        "OiOiOii",
        (PyObject * *)&self->tree,
        &self->start_valid, &self->start,
        &self->stop_valid, &self->stop,
        &self->forward, &self->type));
            
    DBG_ASSERT(self->tree != NULL);
    DBG_ASSERT(self->tree->imp != NULL);
    DBG_ONLY(self->tree->imp->assert_valid();)       
    DBG_ASSERT(self->start_valid == 0 || self->start_valid == 1);
    DBG_ASSERT(self->stop_valid == 0 || self->stop_valid == 1);
    DBG_ASSERT(0 <= self->forward && self->forward <= 1);
    DBG_ASSERT(0 <= self->type && self->type <= 2);

    BANYAN_PYOBJECT_INCREF((PyObject *)self->tree);
    if (self->start_valid)
        BANYAN_PYOBJECT_INCREF(self->start);
    else
        self->start = NULL;        
    if (self->stop_valid)
        BANYAN_PYOBJECT_INCREF(self->stop);
    else 
        self->stop = NULL;        
        
    self->it = self->forward == 0? 
        self->tree->imp->rbegin(self->start, self->stop): 
        self->tree->imp->begin(self->start, self->stop);    
    
    PyObject_GC_Track(self);
    return (PyObject *)self;
}

extern "C" PyObject *
tree_view_next(TreeView * self)
{
    if (self->it == NULL) {
        // Tmp Ami - copy to dagpype
        PyErr_SetNone(PyExc_StopIteration);
        return NULL;
    }
    
    DBG_ASSERT(self->tree != NULL);
    DBG_ASSERT(self->tree->imp != NULL);
    DBG_ONLY(self->tree->imp->assert_valid();)       

    PyObject * ret;
    self->it = self->forward == 0? 
        self->tree->imp->prev(self->it, self->start, self->type, ret):
        self->tree->imp->next(self->it, self->stop, self->type, ret);        
    DBG_ASSERT(ret != NULL);        
    return ret;
}

static PyMethodDef tree_view_methods[] = {
    { NULL, NULL}
};

static PyMemberDef tree_view_memberlist[] = {
    { NULL }
};

PyDoc_STRVAR(TreeViewType_doc, "");

PyTypeObject TreeViewType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/
    "banyan_c.TreeView",               
    /*tp_basicsize*/
    sizeof(TreeView),                   
    /*tp_valuesize*/
    0,                                      
    /* methods */
    /*tp_dealloc*/
    (destructor)tree_view_dealloc,          
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,                     
    /*tp_doc*/
    TreeViewType_doc,                     
    /*tp_traverse*/
    (traverseproc)tree_view_traverse,       
    /*tp_clear*/
    0,               
    /*tp_richless_than*/
    0,                                      
    /*tp_weaklistoffset*/
    0,                                      
    /*tp_iter*/
    PyObject_SelfIter,                  
    /*tp_iternext*/    
    (getiterfunc)tree_view_next,        
    /*tp_methods*/
    tree_view_methods,                      
    /*tp_members*/
    tree_view_memberlist,    
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
    0,      
    /* tp_alloc */
    0,                         
    /* tp_new */
    tree_view_new,                 
};
