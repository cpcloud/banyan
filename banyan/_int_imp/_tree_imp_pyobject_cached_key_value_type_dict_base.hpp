#ifndef _TREE_IMP_PYOBJECT_CACHED_KEY_VALUE_TYPE_DICT_BASE_HPP
#define _TREE_IMP_PYOBJECT_CACHED_KEY_VALUE_TYPE_DICT_BASE_HPP

#include <Python.h>

#include <algorithm>
#include <exception>

#include "_dsa/_functional.hpp"
#include "_dsa/_key_extractor.hpp"
#include "_pyobject_utils.hpp"
#include "_tree_imp_alg_base.hpp"
#include "_tree_imp_value_type_base.hpp"
#include "_dbg.hpp"

template<
    bool Set>
class _PyObjectCachedKeyUniqueSorterIncer
{
private:
    typedef
        std::pair<_CachedKeyPyObject, PyObject *>
        ValueType;

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
            DBG_ASSERT(PyTuple_Check(val));
            DBG_ASSERT(PyTuple_Size(val) == 2);
            vals.push_back(std::make_pair(
                lt(PyTuple_GET_ITEM(val, 0)), 
                PyTuple_GET_ITEM(val, 1)));
        }            

        _FirstLT<_CachedKeyPyObjectCacheGeneratorLT> lt_(lt);            
        std::sort(vals.begin(), vals.end(), lt_);
        typename VecT::iterator end_it = 
            unique(vals.begin(), vals.end(), not2(lt_));
        for (typename VecT::iterator it = end_it; it != vals.end(); ++it)
            it->first.dec();                     
        vals.erase(end_it, vals.end());        
        for (size_t i = 0; i < vals.size(); ++i) 
            BANYAN_PYOBJECT_INCREF(vals[i].second);            
    }
    
    ValueType *
    sorted_begin() 
    {
        return vals.empty()? NULL: &vals[0];
    }
    
    ValueType *
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
    typedef 
        std::vector<ValueType, PyMemMallocAllocator<ValueType> > 
        VecT;    
    
private:
    VecT vals;
};

template<
    class Alg_Tag>
class _TreeImpValueTypeBase<
        Alg_Tag,
        _CachedKeyPyObject,
        false,
        _NullMetadata,
        _CachedKeyPyObjectCacheGeneratorLT> :  
    private _PyObjectCachedKeyUniqueSorterIncer<false>,
    public _TreeImpAlgBase<
        Alg_Tag,
        std::pair<_CachedKeyPyObject, PyObject *>,
        false,
        _PairKeyExtractor<_CachedKeyPyObject>,
        _NullMetadata,
        _CachedKeyPyObjectCacheGeneratorLT> 
{
protected:
    typedef 
        _CachedKeyPyObject
        InternalKeyType;

    typedef 
        PyObject *
        InternalDataType;

    typedef 
        std::pair<InternalKeyType, PyObject *>
        InternalValueType;

    typedef        
        _TreeImpAlgBase<
            Alg_Tag,
            std::pair<_CachedKeyPyObject, PyObject *>,
            false,
            _PairKeyExtractor<_CachedKeyPyObject>,
            _NullMetadata,
            _CachedKeyPyObjectCacheGeneratorLT> 
        BaseT;            
        
    typedef typename BaseT::TreeT TreeT;        

    typedef
        _PyObjectCachedKeyUniqueSorterIncer<false>
        SortingBaseT;
        
#ifdef BANYAN_DEBUG
    static void 
    assert_valid(const InternalValueType & val)
    {
        val.first.assert_valid();
        BANYAN_PYOBJECT_VERIFY(val.second);
    }
#endif // #ifdef BANYAN_DEBUG

protected:        
    inline explicit 
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
        val.first.dec();
        BANYAN_PYOBJECT_DECREF(val.second);
    }

    inline static int
    visit_internal_value(InternalValueType & val, visitproc visit, void * arg)
    {
        const int v = val.first.traverse(visit, arg);
        if (v != 0)
            return v;
        Py_VISIT(val.second);
        return 0;
    }
    
    inline _CachedKeyPyObject
    key_to_internal_key(PyObject * key) 
    {
        return BaseT::tree.less_than()(key);
    }    

    inline static PyObject *
    value_to_key(PyObject * val)
    {
        DBG_ASSERT(PyTuple_Check(val));
        DBG_ASSERT(PyTuple_Size(val) == 2);
        return PyTuple_GET_ITEM(val, 0);
    }

    inline static PyObject *
    internal_value_to_key_inc(const InternalValueType & val)
    {
        return val.first.key_inc();
    }

    inline static PyObject *
    internal_value_to_data_inc(const InternalValueType & val)
    {
        BANYAN_PYOBJECT_INCREF(val.second);
        return val.second;
    }

    inline static PyObject *
    internal_value_to_value_inc(const InternalValueType & val)
    {
        PyObject * ext_val = PyTuple_Pack(2, internal_value_to_key_inc(val), internal_value_to_data_inc(val));
        if (ext_val == NULL) {
            PyErr_NoMemory();
            return NULL;
        }    
        BANYAN_PYOBJECT_DUMMY_INCREF(ext_val);
        return ext_val;
    }
    
    inline static void
    update_data(InternalValueType & val, PyObject * data)
    {
        BANYAN_PYOBJECT_DECREF(val.second);
        val.second = data;
    }

    inline InternalValueType
    key_data_to_internal_value_inc(PyObject * key, PyObject * data)
    {
        InternalValueType val = std::make_pair(BaseT::tree.less_than()(key), data);
        BANYAN_PYOBJECT_INCREF(data);
        return val;
    }        
};    

#endif // #ifndef _TREE_IMP_PYOBJECT_CACHED_KEY_VALUE_TYPE_DICT_BASE_HPP

