#ifndef _TREE_IMP_CB_METADATA_BASE_HPP
#define _TREE_IMP_CB_METADATA_BASE_HPP

#include <Python.h>

#include <climits>
#include <cfloat>

#include "_dbg.hpp"

struct _PyObjectCBMetadataTag{};

template<
    class Alg_Tag,
    bool Set,
    class LT>
struct _TreeImpMetadataBase<
        Alg_Tag,
        PyObject *,
        Set,
        _PyObjectCBMetadataTag,
        LT> :
    public _TreeImpValueTypeBase<
        Alg_Tag,
        PyObject *,
        Set,
        _PyObjectCBMetadata,
        LT>        
{
protected:
    typedef
        _TreeImpValueTypeBase<
            Alg_Tag,
            PyObject *,
            Set,
            _PyObjectCBMetadata,
            LT>    
        BaseT;
        
    typedef
        _PyObjectCBMetadata
        MetadataT;        

protected: 
    explicit
    _TreeImpMetadataBase(PyObject * seq, PyObject * metadata, const LT & lt) :
        BaseT(seq, MetadataT(metadata), lt)
    {
        DBG_ASSERT(seq != NULL);
    }   

    int
    traverse(visitproc visit, void * arg)
    {
        const int v = BaseT::traverse(visit, arg);
        if (v)
            return v;
    
        return BaseT::tree.meta().traverse(visit, arg);        
    }

    virtual PyObject *
    iter_metadata(void * it)
    {
        PyObject * const meta = BaseT::iter_internal_metadata(it).meta();
        BANYAN_PYOBJECT_INCREF(meta);
        return meta;
    }
};

#endif // #ifndef _TREE_IMP_CB_METADATA_BASE_HPP
    
