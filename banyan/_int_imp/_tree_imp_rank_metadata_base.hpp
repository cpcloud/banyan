#ifndef _TREE_IMP_RANK_METADATA_BASE_HPP
#define _TREE_IMP_RANK_METADATA_BASE_HPP

#include <Python.h>

#include <climits>
#include <cfloat>

#include "_dbg.hpp"
#include "_dsa/_rank_updator.hpp"

struct _RankMetadataTag{};

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class LT>
struct _TreeImpMetadataBase<
        Alg_Tag,
        Key_Type,
        Set,
        _RankMetadataTag,
        LT> :
    public _TreeImpValueTypeBase<
        Alg_Tag,
        Key_Type,
        Set,
        _RankMetadata,
        LT>    
{
protected:
    typedef
        _TreeImpValueTypeBase<
            Alg_Tag,
            Key_Type,
            Set,
            _RankMetadata,
            LT>    
        BaseT;
        
    typedef typename BaseT::TreeT TreeT;        

protected: 
    explicit
    _TreeImpMetadataBase(PyObject * seq, PyObject * metadata, const LT & lt) :
        BaseT(seq, _RankMetadata(), lt)
    {
        DBG_ASSERT(seq != NULL);
        DBG_ASSERT(metadata == NULL);
    }   

    virtual PyObject *
    iter_metadata(void * it)
    {                
        const size_t rank = BaseT::iter_internal_metadata(it).rank;
#if PY_MAJOR_VERSION >= 3
        return PyLong_FromLong(rank);
#else // #if PY_MAJOR_VERSION >= 3
        return PyInt_FromLong(rank);            
#endif // #if PY_MAJOR_VERSION >= 3
    }

    virtual PyObject *
    rank_updator_kth(size_t k)
    {
        if (k >= BaseT::tree.size()) {
            PyErr_SetObject(
                PyExc_IndexError, 
#if PY_MAJOR_VERSION >= 3
                PyLong_FromLong(k));
#else // #if PY_MAJOR_VERSION >= 3
                PyInt_FromLong(k));            
#endif // #if PY_MAJOR_VERSION >= 3
            return NULL;
        }            

        const typename TreeT::NodeT * const n = static_cast<const typename TreeT::NodeT *>(
            _rank_updator_kth(	BaseT::tree.node_begin(), k));
        return BaseT::internal_value_to_key_inc(n->val);
    }

    virtual PyObject *
    rank_updator_order(PyObject * key)
    {
        typename TreeT::Iterator b = 
            BaseT::tree.lower_bound(BaseT::key_to_internal_key(key));
        const size_t o = b == BaseT::tree.end()? 
            BaseT::tree.size() :
            _rank_updator_order(b.p);                        
#if PY_MAJOR_VERSION >= 3
        return PyLong_FromLong(o);            
#else // #if PY_MAJOR_VERSION >= 3
        return PyInt_FromLong(o);            
#endif // #if PY_MAJOR_VERSION >= 3
    }
};

template<
    typename Key_Type,
    bool Set,
    class LT>
struct _TreeImpMetadataBase<
        _OVTreeTag,
        Key_Type,
        Set,
        _RankMetadataTag,
        LT> :
    public _TreeImpValueTypeBase<
        _OVTreeTag,
        Key_Type,
        Set,
        _NullMetadata,
        LT>    
{
protected:
    typedef
        _TreeImpValueTypeBase<
            _OVTreeTag,
            Key_Type,
            Set,
            _NullMetadata,
            LT>    
        BaseT;
        
    typedef typename BaseT::TreeT TreeT;        

protected: 
    explicit
    _TreeImpMetadataBase(PyObject * seq, PyObject * metadata, const LT & lt) :
        BaseT(seq, _NullMetadata(), lt)
    {
        DBG_ASSERT(seq != NULL);
    }   

    virtual PyObject *
    iter_metadata(void * it)
    {
        const size_t rank = BaseT::iter_size(it);
#if PY_MAJOR_VERSION >= 3
        return PyLong_FromLong(rank);
#else // #if PY_MAJOR_VERSION >= 3
        return PyInt_FromLong(rank);            
#endif // #if PY_MAJOR_VERSION >= 3
    }

    virtual PyObject *
    rank_updator_kth(size_t k)
    {    
        if (k >= BaseT::tree.size()) {
            PyErr_SetObject(
                PyExc_IndexError, 
#if PY_MAJOR_VERSION >= 3
                PyLong_FromLong(k));
#else // #if PY_MAJOR_VERSION >= 3
                PyInt_FromLong(k));            
#endif // #if PY_MAJOR_VERSION >= 3
            return NULL;
        }            
            
        return BaseT::internal_value_to_key_inc(BaseT::tree.buf[k]);            
    }

    virtual PyObject *
    rank_updator_order(PyObject * key)
    {
        typename TreeT::Iterator b = 
            BaseT::tree.lower_bound(BaseT::key_to_internal_key(key));
        const size_t o = std::distance(BaseT::tree.begin(), b);                      
#if PY_MAJOR_VERSION >= 3
        return PyLong_FromLong(o);            
#else // #if PY_MAJOR_VERSION >= 3
        return PyInt_FromLong(o);            
#endif // #if PY_MAJOR_VERSION >= 3
    }
};

#endif // #ifndef _TREE_IMP_RANK_METADATA_BASE_HPP
    
