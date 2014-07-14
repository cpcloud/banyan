#ifndef _TREE_IMP_PYOBJECT_CACHED_KEY_VALUE_TYPE_SET_BASE_HPP
#define _TREE_IMP_PYOBJECT_CACHED_KEY_VALUE_TYPE_SET_BASE_HPP

#include <Python.h>

#include <algorithm>
#include <exception>

#include "_dsa/_key_extractor.hpp"
#include "_pyobject_utils.hpp"
#include "_tree_imp_alg_base.hpp"
#include "_dbg.hpp"

template<>
class _PyObjectCachedKeyUniqueSorterIncer<
    true>
{
public:
    _PyObjectCachedKeyUniqueSorterIncer(
        PyObject * fast_seq, 
        const _CachedKeyPyObjectCacheGeneratorLT & lt) 
    {           
        if (fast_seq == Py_None)
            return;
    
        vals.reserve(PySequence_Fast_GET_SIZE(fast_seq));            
        for (size_t i = 0; i < static_cast<size_t>(PySequence_Fast_GET_SIZE(fast_seq)); ++i) {    
            PyObject * const val = PySequence_Fast_GET_ITEM(fast_seq, i);    
            vals.push_back(lt(val));
        }            
            
        std::sort(vals.begin(), vals.end(), lt);
        typename VecT::iterator end_it = 
            unique(vals.begin(), vals.end(), not2(lt));
        for (typename VecT::iterator it = end_it; it != vals.end(); ++it)
            it->dec();                     
        vals.erase(end_it, vals.end());
    }
    
    _CachedKeyPyObject *
    sorted_begin() 
    {
        return vals.empty()? NULL: &vals[0];
    }
    
    _CachedKeyPyObject *
    sorted_end()
    {
        return vals.empty()? NULL: &vals[0] + vals.size();
    }
    
    void 
    clear()
    {
        vals.clear();
    }

private:    
    typedef std::vector<_CachedKeyPyObject, PyMemMallocAllocator<_CachedKeyPyObject> > VecT;    
    
private:
    VecT vals;
};

template<
    class Alg_Tag>
class _TreeImpValueTypeBase<
        Alg_Tag,
        _CachedKeyPyObject,
        true,
        _NullMetadata,
        _CachedKeyPyObjectCacheGeneratorLT> : 
    private _PyObjectCachedKeyUniqueSorterIncer<true>,
    public _TreeImpAlgBase<
        Alg_Tag,
        _CachedKeyPyObject,
        true,
        _KeyExtractor<_CachedKeyPyObject>,
        _NullMetadata,
        _CachedKeyPyObjectCacheGeneratorLT>
{
protected:
    typedef 
        _CachedKeyPyObject
        InternalKeyType;

    typedef 
        _CachedKeyPyObject
        InternalValueType;

    typedef
        _PyObjectCachedKeyUniqueSorterIncer<true>        
        SortingBaseT;

    typedef        
        _TreeImpAlgBase<
            Alg_Tag,
            _CachedKeyPyObject,
            true,
            _KeyExtractor<_CachedKeyPyObject>,
            _NullMetadata,
            _CachedKeyPyObjectCacheGeneratorLT>
        BaseT;            
        
    typedef typename BaseT::TreeT TreeT;        

protected:        
    explicit 
    _TreeImpValueTypeBase(
            PyObject * fast_seq, 
            const _NullMetadata &, 
            const _CachedKeyPyObjectCacheGeneratorLT & lt) : 
        SortingBaseT(fast_seq, lt),
        BaseT(
            SortingBaseT::sorted_begin(), 
            SortingBaseT::sorted_end(), 
            _NullMetadata(), 
            lt)
    {
        SortingBaseT::clear();
    }

    int
    traverse(visitproc visit, void * arg)
    {
        for (typename TreeT::Iterator it = BaseT::tree.begin(); it != BaseT::end(); ++it) {
            const int v = visit_internal_value(*it, visit, arg);
            if (v != 0)
                return v;
        }            
    
        return 0;
    }

    inline static void
    dec_internal_value(InternalValueType & val)
    {
        val.dec();
    }

    inline static int
    visit_internal_value(InternalValueType & val, visitproc visit, void * arg)
    {
        return val.traverse(visit, arg);
    }

    inline _CachedKeyPyObject
    key_to_internal_key(PyObject * key) 
    {
        return BaseT::tree.less_than()(key);
    }    

    inline static PyObject *
    value_to_key(PyObject * val)
    {
        return val;
    }

    inline PyObject *
    internal_value_to_key_inc(const InternalValueType & val)
    {
        return val.key_inc();
    }

    inline static PyObject *
    internal_value_to_value_inc(const InternalValueType & val)
    {
        return val.key_inc();
    }

    std::vector<InternalValueType, PyMemMallocAllocator<InternalValueType> > 
    sort_inc_unique_seq(PyObject * fast_seq)
    {
        SortingBaseT so(fast_seq, BaseT::tree.less_than());
        return std::vector<InternalValueType, PyMemMallocAllocator<InternalValueType> >(
            so.sorted_begin(), so.sorted_end());
    }

#ifdef BANYAN_DEBUG
    static void 
    assert_valid(const InternalValueType & val)
    {
        val.assert_valid();
    }
#endif // #ifdef BANYAN_DEBUG
};    

#endif // #ifndef _TREE_IMP_PYOBJECT_CACHED_KEY_VALUE_TYPE_SET_BASE_HPP

