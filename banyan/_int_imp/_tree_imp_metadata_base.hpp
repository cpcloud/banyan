#ifndef _TREE_IMP_METADATA_BASE_HPP
#define _TREE_IMP_METADATA_BASE_HPP

#include <Python.h>

#include <climits>
#include <cfloat>

#include "_dbg.hpp"
#include "_tree_imp_value_type_base.hpp"
#include "_tree_imp_pyobject_key_value_type_dict_base.hpp"
#include "_tree_imp_pyobject_key_value_type_set_base.hpp"
#include "_tree_imp_pyobject_cached_key_value_type_dict_base.hpp"
#include "_tree_imp_pyobject_cached_key_value_type_set_base.hpp"
#include "_tree_imp_non_pyobject_key_value_type_dict_base.hpp"
#include "_tree_imp_non_pyobject_key_value_type_set_base.hpp"

template<
    class Alg_Tag,    
    typename Key_Type,
    bool Set,
    class MetadataTag,
    class LT>
struct _TreeImpMetadataBase
{
    // Nothing.
};

#endif // #ifndef _TREE_IMP_METADATA_BASE_HPP
    
