#ifndef _TREE_IMP_NULL_METADATA_BASE_HPP
#define _TREE_IMP_NULL_METADATA_BASE_HPP

#include <Python.h>

#include "_dbg.hpp"

struct _NullMetadataTag{};

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class LT>
struct _TreeImpMetadataBase<
        Alg_Tag,
        Key_Type,
        Set,
        _NullMetadataTag,
        LT> :
    public _TreeImpValueTypeBase<
        Alg_Tag,
        Key_Type,
        Set,
        _NullMetadata,
        LT>    
{
protected:
    typedef
        _TreeImpValueTypeBase<
            Alg_Tag,
            Key_Type,
            Set,
            _NullMetadata,
            LT>    
        BaseT;

protected: 
    explicit
    _TreeImpMetadataBase(PyObject * seq, PyObject * metadata, const LT & lt) :
        BaseT(seq, _NullMetadata(), lt)
    {
        DBG_ASSERT(seq != NULL);
        DBG_ASSERT(metadata == NULL);
    }   

    virtual PyObject *
    iter_metadata(void * it)
    {
        Py_RETURN_NONE;
    }
};

#endif // #ifndef _TREE_IMP_NULL_METADATA_BASE_HPP
    
