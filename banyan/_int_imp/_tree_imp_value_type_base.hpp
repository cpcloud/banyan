#ifndef _TREE_IMP_VALUE_TYPE_BASE_HPP
#define _TREE_IMP_VALUE_TYPE_BASE_HPP

#include <Python.h>

#include <algorithm>
#include <exception>

#include "_pyobject_utils.hpp"
#include "_dsa/_key_extractor.hpp"
#include "_dbg.hpp"

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class Metadata,
    class LT>
class _TreeImpValueTypeBase
{
protected:
    struct Bad{};
    
private:
    _TreeImpValueTypeBase();        
};    

template<typename Key_Type>
struct _PairKeyExtractor :
    public std::unary_function<Key_Type, PyObject *>
{
    typedef Key_Type KeyType;
    typedef std::pair<KeyType, PyObject *> ValueType;

    inline static const KeyType & 
    extract(const ValueType & p)
    {
        return p.first;
    }
};

#endif // #ifndef _TREE_IMP_VALUE_TYPE_BASE_HPP

