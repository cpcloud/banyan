#ifndef _TREE_IMP_LT_BASE_HPP
#define _TREE_IMP_LT_BASE_HPP

#include <Python.h>

#include "_tree_imp_metadata_base.hpp"
#include "_tree_imp_cb_metadata_base.hpp"
#include "_tree_imp_null_metadata_base.hpp"
#include "_tree_imp_rank_metadata_base.hpp"
#include "_tree_imp_min_gap_metadata_base.hpp"
#include "_tree_imp_interval_max_metadata_base.hpp"
#include "_dbg.hpp"

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class MetadataTag,
    class LT>
struct _TreeImpLTBase
{
    // Nothing.
};

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class MetadataTag>
struct _TreeImpLTBase<
        Alg_Tag,
        Key_Type,
        Set,
        MetadataTag,
        std::less<Key_Type> > : 
    public _TreeImpMetadataBase<
        Alg_Tag,
        Key_Type,
        Set,
        MetadataTag,
        std::less<Key_Type> >
{
protected:
    typedef
        _TreeImpMetadataBase<
            Alg_Tag,
            Key_Type,
            Set,
            MetadataTag,
            std::less<Key_Type> >    
    BaseT;

protected: 
    explicit
    _TreeImpLTBase(PyObject * seq, PyObject * metadata, PyObject * lt) :
        BaseT(seq, metadata, std::less<Key_Type>())
    {
        DBG_ASSERT(lt == NULL);
        DBG_ASSERT(seq != NULL);
    }   
};

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class MetadataTag>
struct _TreeImpLTBase<
        Alg_Tag,
        Key_Type,
        Set,
        MetadataTag,
        _PyObjectStdLT> : 
    public _TreeImpMetadataBase<
        Alg_Tag,
        Key_Type,
        Set,
        MetadataTag,
        _PyObjectStdLT>
{
protected:
    typedef
        _TreeImpMetadataBase<
            Alg_Tag,
            Key_Type,
            Set,
            MetadataTag,
            _PyObjectStdLT>    
        BaseT;

protected: 
    explicit
    _TreeImpLTBase(PyObject * seq, PyObject * metadata, PyObject * lt) :
        BaseT(seq, metadata, _PyObjectStdLT())
    {
        DBG_ASSERT(lt == NULL);
        DBG_ASSERT(seq != NULL);
    }   
};

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class MetadataTag>
struct _TreeImpLTBase<
        Alg_Tag,
        Key_Type,
        Set,
        MetadataTag,
        _PyObjectCmpCBLT> : 
    public _TreeImpMetadataBase<
        Alg_Tag,
        Key_Type,
        Set,
        MetadataTag,
        _PyObjectCmpCBLT>
{
protected:
    typedef
        _TreeImpMetadataBase<
            Alg_Tag,
            Key_Type,
            Set,
            MetadataTag,
            _PyObjectCmpCBLT>    
        BaseT;

protected: 
    explicit
    _TreeImpLTBase(PyObject * seq, PyObject * metadata, PyObject * cmp) :
        BaseT(seq, metadata, _PyObjectCmpCBLT(cmp))
    {
        DBG_ASSERT(seq != NULL);
    }   

    int
    traverse(visitproc visit, void * arg)
    {
        return BaseT::tree.less_than().traverse(visit, arg);        
    }
};

template<
    class Alg_Tag,
    bool Set,
    class MetadataTag>
struct _TreeImpLTBase<
        Alg_Tag,
        PyObject *,
        Set,
        MetadataTag,
        _PyObjectKeyCBLT> : 
    public _TreeImpMetadataBase<
        Alg_Tag,
        PyObject *,
        Set,
        MetadataTag,
        _PyObjectKeyCBLT>
{
protected:
    typedef
        _TreeImpMetadataBase<
            Alg_Tag,
            PyObject *,
            Set,
            MetadataTag,
            _PyObjectKeyCBLT>    
        BaseT;

protected: 
    explicit
    _TreeImpLTBase(PyObject * seq, PyObject * metadata, PyObject * key) :
        BaseT(seq, metadata, _PyObjectKeyCBLT(key))
    {
        DBG_ASSERT(seq != NULL);
    }   

    int
    traverse(visitproc visit, void * arg)
    {
        return BaseT::tree.less_than().traverse(visit, arg);        
    }
};

template<
    class Alg_Tag,
    bool Set>
struct _TreeImpLTBase<
        Alg_Tag,
        PyObject *,
        Set,
        _NullMetadataTag,
        _PyObjectKeyCBLT> : 
    public _TreeImpMetadataBase<
        Alg_Tag,
        _CachedKeyPyObject,
        Set,
        _NullMetadataTag,
        _CachedKeyPyObjectCacheGeneratorLT>
{
protected:
    typedef
        _TreeImpMetadataBase<
            Alg_Tag,
            _CachedKeyPyObject,
            Set,
            _NullMetadataTag,
            _CachedKeyPyObjectCacheGeneratorLT>    
        BaseT;

protected: 
    explicit
    _TreeImpLTBase(PyObject * seq, PyObject * metadata, PyObject * key) :
        BaseT(seq, metadata, _CachedKeyPyObjectCacheGeneratorLT(key))
    {
        DBG_ASSERT(seq != NULL);
    }   

    int
    traverse(visitproc visit, void * arg)
    {
        return BaseT::tree.less_than().traverse(visit, arg);        
    }
};

#endif // #ifndef _TREE_IMP_LT_BASE_HPP

