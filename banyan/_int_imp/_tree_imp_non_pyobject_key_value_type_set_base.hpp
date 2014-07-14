#ifndef _TREE_IMP_NON_PYOBJECT_KEY_VALUE_TYPE_SET_BASE_HPP
#define _TREE_IMP_NON_PYOBJECT_KEY_VALUE_TYPE_SET_BASE_HPP

#include <Python.h>

#include <algorithm>
#include <exception>

#include "_dsa/_functional.hpp"
#include "_dsa/_key_extractor.hpp"
#include "_pyobject_utils.hpp"
#include "_tree_imp_value_type_base.hpp"
#include "_dbg.hpp"

template<
    typename Key_Type>
class _NonPyObjectUniqueSorterIncer<
    Key_Type,
    true>    
{
public:
    typedef std::pair<Key_Type, PyObject *> ValueType;

public:
    _NonPyObjectUniqueSorterIncer(PyObject * fast_seq) 
    {           
        if (fast_seq == Py_None)
            return;
    
        vals.reserve(PySequence_Fast_GET_SIZE(fast_seq));            
        for (size_t i = 0; i < static_cast<size_t>(PySequence_Fast_GET_SIZE(fast_seq)); ++i) {    
            PyObject * const val = PySequence_Fast_GET_ITEM(fast_seq, i);    
            BANYAN_PYOBJECT_INCREF(val);
            vals.push_back(std::make_pair(_KeyFactory<Key_Type>::convert(val), val));
        }            
            
        _FirstLT<std::less<Key_Type> > lt;            
        std::sort(vals.begin(), vals.end(), lt);
        vals.erase(unique(vals.begin(), vals.end(), not2(lt)), vals.end());
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
    typedef std::vector<ValueType, PyMemMallocAllocator<ValueType> > VecT;    
    
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
        true,
        Metadata,
        std::less<Key_Type> > : 
    private _NonPyObjectUniqueSorterIncer<Key_Type, true>,
    public _TreeImpAlgBase<
        Alg_Tag,
        std::pair<Key_Type, PyObject *>,
        true,
        _KeyExtractor<
            std::pair<Key_Type, PyObject *> >,
        Metadata,   
        _FirstLT<std::less<Key_Type> > >
{
protected:
    typedef 
        std::pair<Key_Type, PyObject *>
        InternalKeyType;

    typedef 
        InternalKeyType
        InternalValueType;

    typedef
        _NonPyObjectUniqueSorterIncer<Key_Type, true>        
        SortingBaseT;

    typedef        
        _TreeImpAlgBase<
            Alg_Tag,
            InternalKeyType,
            true,
            _KeyExtractor<
                std::pair<Key_Type, PyObject *> >,
            Metadata,   
            _FirstLT<std::less<Key_Type> > >
        BaseT;            
        
    typedef typename BaseT::TreeT TreeT;        

protected:        
    explicit 
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
        for (typename TreeT::Iterator it = BaseT::tree.begin(); it != BaseT::end(); ++it)
            Py_VISIT(it->second);

        return 0;
    }

    inline static void
    dec_internal_value(InternalValueType & val)
    {
        BANYAN_PYOBJECT_DECREF(val.second);
    }

    inline static int
    visit_internal_value(InternalValueType & val, visitproc visit, void * arg)
    {
        Py_VISIT(val.second);
        return 0;
    }

    inline static InternalKeyType
    key_to_internal_key(PyObject * key) 
    {
        return std::make_pair(_KeyFactory<Key_Type>::convert(key), key);
    }    

    inline static PyObject *
    value_to_key(PyObject * val)
    {
        return val;
    }

    inline static PyObject *
    internal_value_to_key_inc(const InternalValueType & val)
    {
        BANYAN_PYOBJECT_INCREF(val.second);
        return val.second;
    }

    inline static InternalKeyType
    internal_value_to_internal_key(const InternalValueType & val)
    {
        return val;
    }

    inline static PyObject *
    internal_value_to_value_inc(const InternalValueType & val)
    {
        BANYAN_PYOBJECT_INCREF(val.second);
        return val.second;
    }

    std::vector<InternalKeyType, PyMemMallocAllocator<InternalKeyType> > 
    sort_inc_unique_seq(PyObject * fast_seq)
    {
        SortingBaseT so(fast_seq);
        return std::vector<InternalKeyType, PyMemMallocAllocator<InternalKeyType> >(
            so.sorted_begin(), so.sorted_end());
    }

#ifdef BANYAN_DEBUG
    static void 
    assert_valid(const InternalValueType & val)
    {
        BANYAN_PYOBJECT_VERIFY(val.second);
    }
#endif // #ifdef BANYAN_DEBUG
};    

#endif // #ifndef _TREE_IMP_NON_PYOBJECT_KEY_VALUE_TYPE_SET_BASE_HPP

