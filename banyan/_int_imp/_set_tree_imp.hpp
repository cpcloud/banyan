#ifndef _BANYAN_SET_TREE_IMP_HPP
#define _BANYAN_SET_TREE_IMP_HPP

#include <Python.h>

#include "_int_int/_set_tree_imp_base.hpp"
#include "_tree_imp.hpp"
#include "_dbg.hpp"

template<
    class Alg_Tag,
    typename Key_Type,
    class MetadataTag,
    class LT>
class _SetTreeImp : 
    public _TreeImp<
        Alg_Tag,
        Key_Type,
        true,
        MetadataTag,
        LT>
{
public:
    explicit
    _SetTreeImp(PyObject * fast_seq, PyObject * metadata, PyObject * lt);

    virtual 
    ~_SetTreeImp();
    
    virtual PyObject *
    discard(PyObject * key);

    virtual PyObject *
    insert(PyObject * key);

    virtual PyObject *
    ext_union(PyObject * o, int type);

    virtual PyObject *
    ext_cmp(PyObject * o, int type);

    virtual void *
    next(void * cur, PyObject * stop, int type, PyObject * & cur_val);

    virtual void *
    prev(void * cur, PyObject * start, int type, PyObject * & cur_val);
    
#ifdef BANYAN_DEBUG
    void
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG    

protected:
    typedef   
        _TreeImp<
            Alg_Tag,
            Key_Type,
            true,
            MetadataTag,
            LT>     
        BaseT;         
        
    typedef
        typename BaseT::TreeT           
        TreeT;
};

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::_SetTreeImp(PyObject * fast_seq, PyObject * metadata, PyObject * lt) :
    BaseT(fast_seq, metadata, lt)
{
    // Do nothing.
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::~_SetTreeImp()
{
    // Do nothing.
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::insert(PyObject * key)
{
    std::pair<typename TreeT::Iterator, bool> ins;
    
    try {
        ins = BaseT::tree.insert(BaseT::key_to_internal_key(key));
    }
    catch(...) {
        PyErr_SetObject(PyExc_KeyError, key);        
        throw;
    }
    
    if (ins.second) 
        BANYAN_PYOBJECT_INCREF(key);

    Py_RETURN_NONE;                
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::ext_union(PyObject * fast_seq, int type)
{
    typedef
        std::vector<typename BaseT::InternalKeyType, PyMemMallocAllocator<typename BaseT::InternalKeyType> >
        VecT;

    VecT sorted = BaseT::sort_inc_unique_seq(fast_seq);

    VecT union_res;    
    switch (type) {
    case 0:
        std::set_union(
            BaseT::tree.begin(), BaseT::tree.end(),
            sorted.begin(), sorted.end(),
            back_inserter(union_res),
            BaseT::tree.less_than());
        break;            
    case 1:        
        std::set_intersection(
            BaseT::tree.begin(), BaseT::tree.end(),
            sorted.begin(), sorted.end(),
            back_inserter(union_res),
            BaseT::tree.less_than());
        break;            
    case 2:        
        std::set_difference(
            BaseT::tree.begin(), BaseT::tree.end(),
            sorted.begin(), sorted.end(),
            back_inserter(union_res),
            BaseT::tree.less_than());
        break;            
    case 3:        
        std::set_symmetric_difference(
            BaseT::tree.begin(), BaseT::tree.end(),
            sorted.begin(), sorted.end(),
            back_inserter(union_res),
            BaseT::tree.less_than());
        break;            
    default:
        DBG_ASSERT(0);
    };            

    PyObject * const res = PyTuple_New(union_res.size());
    if (res == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    
    for (size_t i = 0; i < union_res.size(); ++i) {
        PyObject * const k = BaseT::internal_value_to_key_inc(union_res[i]);
        PyTuple_SET_ITEM(res, i, k);
    }            
    
    for (size_t i = 0; i < sorted.size(); ++i)
        BaseT::dec_internal_value(sorted[i]);    

    return res;
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::ext_cmp(PyObject * fast_seq, int type)
{
    typedef
        std::vector<typename BaseT::InternalKeyType, PyMemMallocAllocator<typename BaseT::InternalKeyType> >
        VecT;
        
    VecT sorted = BaseT::sort_inc_unique_seq(fast_seq);

    bool res = false;        
    switch (type) {
    case 0:
        res = std::includes(
            sorted.begin(), sorted.end(),
            BaseT::tree.begin(), BaseT::tree.end(),
            BaseT::tree.less_than());
        break;            
    case 1:        
        res = std::includes(
            BaseT::tree.begin(), BaseT::tree.end(),
            sorted.begin(), sorted.end(),
            BaseT::tree.less_than());
        break;            
    case 2:        
        res = 
            std::distance(sorted.begin(), sorted.end()) == 
                std::distance(BaseT::tree.begin(), BaseT::tree.end()) && 
            std::equal(
                sorted.begin(), sorted.end(),
                BaseT::tree.begin(),
                LTEq<typename TreeT::LTT>(BaseT::tree.less_than()));
        break;            
    case 3:        
        res = disjoint(
            BaseT::tree.begin(), BaseT::tree.end(),
            sorted.begin(), sorted.end(),
            BaseT::tree.less_than());
        break;            
    default:
        DBG_ASSERT(0);
    };            
    
    if (res)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;        
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::discard(PyObject * key)
{
    try {    
        typename BaseT::InternalValueType val = 
            BaseT::tree.erase(BaseT::key_to_internal_key(key));
        BaseT::dec_internal_value(val);
    }
    catch(const std::logic_error &) {
        // Do nothing.
    }

    Py_RETURN_NONE;
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
void *
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::next(
    void * mem, PyObject * stop, int type, PyObject * & cur_val)
{
    DBG_ASSERT(mem != NULL);       
    DBG_ASSERT(type == 0);
    
    cur_val = BaseT::internal_value_to_key_inc(BaseT::mem_value(mem));
    
    if (stop == NULL)
        return BaseT::mem_next(mem, NULL);
        
    const typename BaseT::InternalKeyType stop_k = BaseT::key_to_internal_key(stop);                        
    return BaseT::mem_next(mem, &stop_k);
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
void *
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::prev(
    void * mem, PyObject * start, int type, PyObject * & cur_val)
{
    DBG_ASSERT(mem != NULL);    
    DBG_ASSERT(type == 0);
    
    cur_val = BaseT::internal_value_to_key_inc(BaseT::mem_value(mem));
    
    if (start == NULL)
        return BaseT::mem_prev(mem, NULL);
        
    const typename BaseT::InternalKeyType start_k = BaseT::key_to_internal_key(start);                        
    return BaseT::mem_prev(mem, &start_k);
}

#ifdef BANYAN_DEBUG
template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
void
_SetTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::assert_valid() const
{
    BaseT::tree.assert_valid();       
}
#endif // #ifdef BANYAN_DEBUG    

#endif // #ifndef _BANYAN_SET_TREE_IMP_HPP

