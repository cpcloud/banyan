#ifndef BANYAN_SET_TREE_HPP
#define BANYAN_SET_TREE_HPP

#include <Python.h>

#include "_dbg.hpp"
#include "frozen_set_tree.hpp"

struct SetTree :
    public FrozenSetTree
{
    // Nothing.
};

extern "C" PyTypeObject SetTreeType;

#endif // #ifndef BANYAN_SET_TREE_HPP

