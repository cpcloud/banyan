#ifndef NODE_HPP
#define NODE_HPP

#include <Python.h>

#include "_dbg.hpp"

PyObject * 
node_new(Tree * tree, void * it);

extern "C" PyTypeObject NodeType;

#endif // #ifndef NODE_HPP

