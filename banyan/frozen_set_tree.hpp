#ifndef FROZEN_SET_TREE_HPP
#define FROZEN_SET_TREE_HPP

#include <Python.h>

#include "_dbg.hpp"
#include "tree.hpp"

struct FrozenSetTree :
    public Tree
{
    // Nothing.
};

extern "C" PyTypeObject FrozenSetTreeType;

#endif // #ifndef FROZEN_SET_TREE_HPP

