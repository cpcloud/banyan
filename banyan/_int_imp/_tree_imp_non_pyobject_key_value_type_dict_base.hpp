#ifndef _TREE_IMP_NON_PYOBJECT_KEY_VALUE_TYPE_DICT_BASE_HPP
#define _TREE_IMP_NON_PYOBJECT_KEY_VALUE_TYPE_DICT_BASE_HPP

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
    typename Key_Type,
    bool Set>
class _NonPyObjectUniqueSorterIncer
{
private:
    typedef
        std::pair<std::pair<Key_Type, PyObject *>, PyObject *>
        ValueType;

public:
    _NonPyObjectUniqueSorterIncer(PyObject * fast_seq) 
    {           
        if (fast_seq == Py_None)
            return;
    
        vals.reserve(PySequence_Fast_GET_SIZE(fast_seq));            
        for (size_t i = 0; i < static_cast<size_t>(PySequence_Fast_GET_SIZE(fast_seq)); ++i) {    
            PyObject * const val = PySequence_Fast_GET_ITEM(fast_seq, i);    
            DBG_ASSERT(PyTuple_Check(val));
            DBG_ASSERT(PyTuple_Size(val) == 2);
            PyObject * const key = PyTuple_GET_ITEM(val, 0);
            BANYAN_PYOBJECT_INCREF(key);
            vals.push_back(std::make_pair(
                std::make_pair(                    
                    _KeyFactory<Key_Type>::convert(key), 
                    key),
                PyTuple_GET_ITEM(val, 1)));
        }            
            
        _FirstLT<_FirstLT<std::less<Key_Type> > > lt;            
        std::sort(vals.begin(), vals.end(), lt);
        vals.erase(unique(vals.begin(), vals.end(), not2(lt)), vals.end());
        
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
    class Alg_Tag,
    class Key_Type, 
    class Metadata>
class _TreeImpValueTypeBase<
        Alg_Tag,
        Key_Type,
        false,
        Metadata,   
        std::less<Key_Type> > :  
    private _NonPyObjectUniqueSorterIncer<Key_Type, false>,
    public _TreeImpAlgBase<
        Alg_Tag,
        std::pair<std::pair<Key_Type, PyObject *>, PyObject *>,
        false,
        _PairKeyExtractor<std::pair<Key_Type, PyObject *> >,
        Metadata,   
        _FirstLT<std::less<Key_Type> > >
{
protected:
    typedef 
        std::pair<Key_Type, PyObject *>
        InternalKeyType;

    typedef 
        PyObject *
        InternalDataType;

    typedef 
        std::pair<InternalKeyType, InternalDataType>
        InternalValueType;

    typedef        
        _TreeImpAlgBase<
            Alg_Tag,
            std::pair<std::pair<Key_Type, PyObject *>, PyObject *>,
            false,
            _PairKeyExtractor<std::pair<Key_Type, PyObject *> >,
            Metadata,   
            _FirstLT<std::less<Key_Type> > > 
        BaseT;            
        
    typedef typename BaseT::TreeT TreeT;        

    typedef
        _NonPyObjectUniqueSorterIncer<Key_Type, false>        
        SortingBaseT;
        
#ifdef BANYAN_DEBUG
    static void 
    assert_valid(const InternalValueType & val)
    {
        BANYAN_PYOBJECT_VERIFY(val.second);
    }
#endif // #ifdef BANYAN_DEBUG

protected:        
    inline explicit 
    _TreeImpValueTypeBase(PyObject * fast_seq, const Metadata &  metadata, const std::less<Key_Type> &) : 
        SortingBaseT(fast_seq),
        BaseT(
            SortingBaseT::sorted_begin(), 
            SortingBaseT::sorted_end(), 
             metadata,
            _FirstLT<std::less<Key_Type> >())
    {
        SortingBaseT::clear();
    }

    int
    traverse(visitproc visit, void * arg)
    {
        for (typename TreeT::Iterator it = BaseT::tree.begin(); it != BaseT::end(); ++it) {
            Py_VISIT(it->first.second);            
            Py_VISIT(it->second);
    }            
    
        return 0;
    }

    inline static void
    dec_internal_value(InternalValueType & val)
    {
        BANYAN_PYOBJECT_DECREF(val.first.second);
        BANYAN_PYOBJECT_DECREF(val.second);
    }

    inline static int
    visit_internal_value(InternalValueType & val, visitproc visit, void * arg)
    {
        Py_VISIT(val.first.second);
        Py_VISIT(val.second);
        return 0;
    }

    inline static std::pair<Key_Type, PyObject *>
    key_to_internal_key(PyObject * key) 
    {
        return std::make_pair(_KeyFactory<Key_Type>::convert(key), key);
    }    

    inline static PyObject *
    value_to_key(PyObject * val)
    {
        DBG_ASSERT(PyTuple_Check(val));
        DBG_ASSERT(PyTuple_Size(val) == 2);
        return PyTuple_GET_ITEM(val, 0);
    }

    inline static InternalKeyType
    internal_value_to_internal_key(const InternalValueType & val)
    {
        return val.first;
    }

    inline static PyObject *
    internal_value_to_key_inc(const InternalValueType & val)
    {
        BANYAN_PYOBJECT_INCREF(val.first.second);
        return val.first.second;
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
        PyObject * const ext_val = PyTuple_New(2);
        if (ext_val == NULL) {
             PyErr_NoMemory();             
             throw std::bad_alloc();
        }             
        BANYAN_PYOBJECT_INCREF(val.first.second);
        PyTuple_SET_ITEM(ext_val, 0, val.first.second);
        BANYAN_PYOBJECT_INCREF(val.second);
        PyTuple_SET_ITEM(ext_val, 1, val.second);
        return ext_val;
    }

    inline static void
    update_data(InternalValueType & val, PyObject * data)
    {
        BANYAN_PYOBJECT_DECREF(val.second);
        val.second = data;
    }

    static InternalValueType
    key_data_to_internal_value_inc(PyObject * key, PyObject * data)
    {
        Key_Type k = _KeyFactory<Key_Type>::convert(key);
        BANYAN_PYOBJECT_INCREF(key);
        BANYAN_PYOBJECT_INCREF(data);
        return std::make_pair(std::make_pair(k, key), data);
    }        
};    

#endif // #ifndef _TREE_IMP_NON_PYOBJECT_KEY_VALUE_TYPE_DICT_BASE_HPP

