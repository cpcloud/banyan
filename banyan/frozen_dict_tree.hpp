#ifndef BANYAN_FROZEN_DICT_TREE_HPP
#define BANYAN_FROZEN_DICT_TREE_HPP

#include <Python.h>

#include "_dbg.hpp"
#include "tree.hpp"

struct FrozenDictTree :
    public Tree
{
    // Nothing.
};

extern "C" PyTypeObject FrozenDictTreeType;

#endif // #ifndef BANYAN_FROZEN_DICT_TREE_HPP

