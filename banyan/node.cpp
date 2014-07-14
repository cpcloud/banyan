#include <Python.h>
#include <structmember.h>

#include <iostream>

#include "tree.hpp"
#include "_int_imp/_pyobject_utils.hpp"

struct Node
{
    PyObject_HEAD

    Tree * tree;
    
    void * it;
};

extern "C" PyTypeObject NodeType;

extern "C" int
node_traverse(Node * self, visitproc visit, void * arg)
{
    Py_VISIT(self->tree);
    return 0;
}

extern "C" void
node_dealloc(Node * self)
{
    DBG_ASSERT(self->tree != NULL);
    DBG_ASSERT(self->tree->imp != NULL);
    DBG_ONLY(self->tree->imp->assert_valid();)
    self->tree->imp->delete_node_iter(self->it);
    DBG_ONLY(self->it = NULL);

    BANYAN_PYOBJECT_DECREF((PyObject *)self->tree);

    PyObject_GC_UnTrack(self);

    PyObject_GC_Del(self);
}

PyObject *
node_new(Tree * tree, void * it)
{
    if (it == NULL)
        Py_RETURN_NONE;

    Node * const self = PyObject_GC_New(Node, &NodeType);
    if (self == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    
    self->tree = tree;
    BANYAN_PYOBJECT_INCREF((PyObject *)self->tree);

    self->it = it;
    
    PyObject_GC_Track(self);
    return (PyObject *)self;
}

extern "C" PyObject *
node_left(Node * self)
{
    DBG_ASSERT(self->tree != NULL);
    DBG_ASSERT(self->tree->imp != NULL);
    DBG_ONLY(self->tree->imp->assert_valid();)

    return node_new(self->tree, self->tree->imp->left_iter(self->it));
}

extern "C" PyObject *
node_right(Node * self)
{
    DBG_ASSERT(self->tree != NULL);
    DBG_ASSERT(self->tree->imp != NULL);
    DBG_ONLY(self->tree->imp->assert_valid();)

    return node_new(self->tree, self->tree->imp->right_iter(self->it));
}

extern "C" PyObject *
node_key(Node * self)
{
    DBG_ASSERT(self->tree != NULL);
    DBG_ASSERT(self->tree->imp != NULL);
    DBG_ONLY(self->tree->imp->assert_valid();)
    
    return BANYAN_VERIFY_EXCEPTION(self->tree->imp->iter_key(self->it));
}

extern "C" PyObject *
node_metadata(Node * self)
{
    DBG_ASSERT(self->tree != NULL);
    DBG_ASSERT(self->tree->imp != NULL);
    DBG_ONLY(self->tree->imp->assert_valid();)
    
    return BANYAN_VERIFY_EXCEPTION(self->tree->imp->iter_metadata(self->it));
}

extern "C" PyObject *
node_lt_keys(Node * self, PyObject * args)
{
    DBG_ASSERT(self->tree != NULL);
    DBG_ASSERT(self->tree->imp != NULL);
    DBG_ONLY(self->tree->imp->assert_valid();)

    PyObject * lhs, * rhs;
    DBG_VERIFY(PyArg_ParseTuple(
        args, 
        "OO",
        &lhs, &rhs));

    return BANYAN_VERIFY_EXCEPTION(self->tree->imp->lt_keys(lhs, rhs));
}

static PyMethodDef node_methods[] = {
    { "left", (PyCFunction)node_left, METH_NOARGS, "" },
    { "right", (PyCFunction)node_right, METH_NOARGS, "" },
    { "key", (PyCFunction)node_key, METH_NOARGS, "" },
    { "metadata", (PyCFunction)node_metadata, METH_NOARGS, "" },
    { "lt_keys", (PyCFunction)node_lt_keys, METH_VARARGS, "" },
    { NULL, NULL}
};

static PyMemberDef node_memberlist[] = {
    { NULL }
};

PyDoc_STRVAR(node_type_doc, "");

PyTypeObject NodeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/
    "banyan_c.Node",               
    /*tp_basicsize*/
    sizeof(Node),                   
    /*tp_valuesize*/
    0,                                      
    /* methods */
    /*tp_dealloc*/
    (destructor)node_dealloc,          
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE,                     
    /*tp_doc*/
    node_type_doc,                     
    /*tp_traverse*/
    (traverseproc)node_traverse,       
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
    node_methods,                      
    /*tp_members*/
    node_memberlist,         
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
    0,                 
};
