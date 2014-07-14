#ifndef _TREE_IMP_PYOBJECT_KEY_VALUE_TYPE_SET_BASE_HPP
#define _TREE_IMP_PYOBJECT_KEY_VALUE_TYPE_SET_BASE_HPP

#include <Python.h>

#include <algorithm>
#include <exception>

#include "_dsa/_key_extractor.hpp"
#include "_pyobject_utils.hpp"
#include "_tree_imp_alg_base.hpp"
#include "_dbg.hpp"

template<
    class LT>
class _PyObjectUniqueSorterIncer<
    LT,
    true>
{
public:
    _PyObjectUniqueSorterIncer(PyObject * fast_seq, const LT & lt) 
    {           
        if (fast_seq == Py_None)
            return;
    
        vals.reserve(PySequence_Fast_GET_SIZE(fast_seq));            
        std::copy(
            PySequence_Fast_ITEMS(fast_seq), 
            PySequence_Fast_ITEMS(fast_seq) + PySequence_Fast_GET_SIZE(fast_seq),
            std::back_inserter(vals));        
            
        std::sort(vals.begin(), vals.end(), lt);
        vals.erase(unique(vals.begin(), vals.end(), not2(lt)), vals.end());

        for (PyObject * * o = sorted_begin(); o != sorted_end(); ++o)
            BANYAN_PYOBJECT_INCREF(*o);            
    }
    
    PyObject * *
    sorted_begin() 
    {
        return vals.empty()? NULL: &vals[0];
    }
    
    PyObject * *
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
    typedef std::vector<PyObject *, PyMemMallocAllocator<PyObject *> > VecT;    
    
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
        true,
        Metadata,
        LT> : 
    private _PyObjectUniqueSorterIncer<LT, true>,
    public _TreeImpAlgBase<
        Alg_Tag,
        PyObject *,
        true,
        _KeyExtractor<PyObject *>,
        Metadata,
        LT>
{
protected:
    typedef 
        PyObject *
        InternalKeyType;

    typedef 
        PyObject *
        InternalValueType;

    typedef
        _PyObjectUniqueSorterIncer<LT, true>        
        SortingBaseT;

    typedef        
        _TreeImpAlgBase<
            Alg_Tag,
            PyObject *,
            true,
            _KeyExtractor<PyObject *>,
            Metadata,
            LT>
        BaseT;            
        
    typedef typename BaseT::TreeT TreeT;        

protected:        
    explicit 
    _TreeImpValueTypeBase(PyObject * fast_seq, const Metadata & metadata, const LT & lt) : 
        SortingBaseT(fast_seq, lt),
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
        return val;
    }

    inline static PyObject *
    internal_value_to_key_inc(InternalValueType val)
    {
        BANYAN_PYOBJECT_INCREF(val);
        return val;
    }

    inline static PyObject *
    internal_value_to_value_inc(InternalValueType val)
    {
        BANYAN_PYOBJECT_INCREF(val);
        return val;
    }

    std::vector<PyObject *, PyMemMallocAllocator<PyObject *> > 
    sort_inc_unique_seq(PyObject * fast_seq)
    {
        SortingBaseT so(fast_seq, BaseT::tree.less_than());
        return std::vector<PyObject *, PyMemMallocAllocator<PyObject *> >(
            so.sorted_begin(), so.sorted_end());
    }

#ifdef BANYAN_DEBUG
    static void 
    assert_valid(const InternalValueType & val)
    {
        BANYAN_PYOBJECT_VERIFY(val);
    }
#endif // #ifdef BANYAN_DEBUG
};    

#endif // #ifndef _TREE_IMP_PYOBJECT_KEY_VALUE_TYPE_SET_BASE_HPP

