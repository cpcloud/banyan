#ifndef _TREE_IMP_PYOBJECT_KEY_VALUE_TYPE_DICT_BASE_HPP
#define _TREE_IMP_PYOBJECT_KEY_VALUE_TYPE_DICT_BASE_HPP

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
    class LT,
    bool Set>
class _PyObjectUniqueSorterIncer
{
private:
    typedef
        std::pair<PyObject *, PyObject *>
        ValueType;

public:
    _PyObjectUniqueSorterIncer(PyObject * fast_seq, const LT & lt) 
    {           
        if (fast_seq == Py_None)
            return;
    
        vals.reserve(PySequence_Fast_GET_SIZE(fast_seq));            
        for (size_t i = 0; i < static_cast<size_t>(PySequence_Fast_GET_SIZE(fast_seq)); ++i) {    
            PyObject * const val = PySequence_Fast_GET_ITEM(fast_seq, i);    
            DBG_ASSERT(PyTuple_Check(val));
            DBG_ASSERT(PyTuple_Size(val) == 2);
            vals.push_back(std::make_pair(
                PyTuple_GET_ITEM(val, 0), 
                PyTuple_GET_ITEM(val, 1)));
        }            

        _FirstLT<LT> lt_(lt);            
        std::sort(vals.begin(), vals.end(), lt_);
        vals.erase(unique(vals.begin(), vals.end(), not2(lt_)), vals.end());
        
        for (size_t i = 0; i < vals.size(); ++i) {
            BANYAN_PYOBJECT_INCREF(vals[i].first);            
            BANYAN_PYOBJECT_INCREF(vals[i].second);            
        }            
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
    class Alg_Tag,
    class Metadata,
    class LT>
class _TreeImpValueTypeBase<
        Alg_Tag,
        PyObject *,
        false,
        Metadata,
        LT> :  
    private _PyObjectUniqueSorterIncer<TupleLT<LT>, true>,
    public _TreeImpAlgBase<
        Alg_Tag,
        PyObject *,
        false,
        _TupleKeyExtractor,
        Metadata,
        LT> 
{
protected:
    typedef 
        PyObject *
        InternalKeyType;

    typedef 
        PyObject *
        InternalDataType;

    typedef 
        PyObject *
        InternalValueType;

    typedef        
        _TreeImpAlgBase<
            Alg_Tag,
            PyObject *,
            false,
            _TupleKeyExtractor,
            Metadata,
            LT> 
        BaseT;            
        
    typedef typename BaseT::TreeT TreeT;        

    typedef
        _PyObjectUniqueSorterIncer<TupleLT<LT>, true>        
        SortingBaseT;
        
#ifdef BANYAN_DEBUG
    static void 
    assert_valid(const InternalValueType & val)
    {
        BANYAN_PYOBJECT_VERIFY(val);
        DBG_ASSERT(PyTuple_Check(val));
        DBG_ASSERT(PyTuple_Size(val) == 2);
        BANYAN_PYOBJECT_VERIFY(PyTuple_GET_ITEM(val, 0));
        BANYAN_PYOBJECT_VERIFY(PyTuple_GET_ITEM(val, 1));
    }
#endif // #ifdef BANYAN_DEBUG

protected:        
    inline explicit 
    _TreeImpValueTypeBase(PyObject * fast_seq, const Metadata & metadata, const LT & lt) : 
        SortingBaseT(fast_seq, TupleLT<LT>(lt)),
        BaseT(
            SortingBaseT::sorted_begin(), 
            SortingBaseT::sorted_end(), 
            metadata,
            lt)
    {
        SortingBaseT::clear();
    }

    int
    traverse(visitproc visit, void * arg)
    {
        for (typename TreeT::Iterator it = BaseT::tree.begin(); it != BaseT::end(); ++it)
            Py_VISIT(*it);
    
        return 0;
    }

    inline static void
    dec_internal_value(InternalValueType & val)
    {
        BANYAN_PYOBJECT_DECREF(val);
    }

    inline static int
    visit_internal_value(InternalValueType & val, visitproc visit, void * arg)
    {
        Py_VISIT(val);
        return 0;
    }
    
    inline static PyObject *
    key_to_internal_key(PyObject * key) 
    {
        return key;
    }    

    inline static PyObject *
    value_to_key(PyObject * val)
    {
        DBG_ASSERT(PyTuple_Check(val));
        DBG_ASSERT(PyTuple_Size(val) == 2);
        return PyTuple_GET_ITEM(val, 0);
    }

    inline static PyObject *
    internal_value_to_key_inc(InternalValueType val)
    {
        DBG_ASSERT(PyTuple_Check(val));
        DBG_ASSERT(PyTuple_Size(val) == 2);
        PyObject * const key = PyTuple_GET_ITEM(val, 0);
        BANYAN_PYOBJECT_INCREF(key); 
        return key;
    }

    inline static PyObject *
    internal_value_to_data_inc(InternalValueType val)
    {
        DBG_ASSERT(PyTuple_Check(val));
        DBG_ASSERT(PyTuple_Size(val) == 2);
        PyObject * const data = PyTuple_GET_ITEM(val, 1);
        BANYAN_PYOBJECT_INCREF(data);
        return data;
    }

    inline static PyObject *
    internal_value_to_value_inc(InternalValueType val)
    {
        BANYAN_PYOBJECT_INCREF(val);
        return val;
    }
    
    inline static void
    update_data(PyObject * val, PyObject * data)
    {
        DBG_ONLY(assert_valid(val));
        PyTuple_SetItem(val, 1, data);
    }

    static PyObject *
    key_data_to_internal_value_inc(PyObject * key, PyObject * data)
    {
        PyObject * val = PyTuple_Pack(2, key, data);
        if (val == NULL) {
            PyErr_NoMemory();
            return NULL;
        }    
        BANYAN_PYOBJECT_DUMMY_INCREF(val);
        BANYAN_PYOBJECT_INCREF(key);
        BANYAN_PYOBJECT_INCREF(data);
        return val;
    }        
};    

#endif // #ifndef _TREE_IMP_PYOBJECT_KEY_VALUE_TYPE_DICT_BASE_HPP

