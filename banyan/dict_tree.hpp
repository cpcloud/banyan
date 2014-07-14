#ifndef BANYAN_DICT_TREE_HPP
#define BANYAN_DICT_TREE_HPP

#include <Python.h>

#include "_dbg.hpp"
#include "frozen_dict_tree.hpp"

struct DictTree :
    public FrozenDictTree
{
    // Nothing.
};

extern "C" PyTypeObject DictTreeType;

#endif // #ifndef BANYAN_DICT_TREE_HPP

