#ifndef _TREE_IMP_MAPPING_BASE_HPP
#define _TREE_IMP_MAPPING_BASE_HPP

#include <Python.h>

#include "_int_int/_set_tree_imp_base.hpp"
#include "_int_int/_dict_tree_imp_base.hpp"

template<bool Set>
class _TreeImpMappingBase :
    public _DictTreeImpBase
{
    // Nothing.
};

template<>
class _TreeImpMappingBase<
        true> :
    public _SetTreeImpBase
{
    // Nothing.
};
    
#endif // #ifndef _TREE_IMP_MAPPING_BASE_HPP

