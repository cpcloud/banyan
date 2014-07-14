#include <Python.h>
#include <structmember.h>

#include <iostream>

struct MinGapMetadata
{
    PyObject_HEAD
};

extern "C" PyTypeObject MinGapMetadataType;

extern "C" void
min_gap_metadata_dealloc(MinGapMetadata * self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

extern "C" PyObject *
min_gap_metadata_new(PyTypeObject * type, PyObject *, PyObject *)
{
    MinGapMetadata * const self = (MinGapMetadata *)type->tp_alloc(type, 0);
    if (self == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    return (PyObject *)self;
}

static PyMethodDef min_gap_metadata_methods[] = {
    { NULL, NULL}
};

static PyMemberDef min_gap_metadata_memberlist[] = {
    { NULL }
};

PyDoc_STRVAR(min_gap_metadata_type_doc, "");

PyTypeObject MinGapMetadataType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/
    "banyan_c.MinGapMetadata",               
    /*tp_basicsize*/
    sizeof(MinGapMetadata),                   
    /*tp_valuesize*/
    0,                                      
    /* methods */
    /*tp_dealloc*/
    (destructor)min_gap_metadata_dealloc,          
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
    min_gap_metadata_type_doc,                     
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
    min_gap_metadata_methods,                      
    /*tp_members*/
    min_gap_metadata_memberlist,         
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
    min_gap_metadata_new,                 
};
