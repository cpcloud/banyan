#ifndef _BANYAN_DICT_TREE_IMP_HPP
#define _BANYAN_DICT_TREE_IMP_HPP

#include <Python.h>

#include "_int_int/_dict_tree_imp_base.hpp"
#include "_tree_imp.hpp"
#include "_dbg.hpp"

template<
    class Alg_Tag,
    typename Key_Type,
    class MetadataTag,
    class LT>
class _DictTreeImp : 
    public _TreeImp<
        Alg_Tag,
        Key_Type,
        false,
        MetadataTag,
        LT>
{
public:
    explicit
    _DictTreeImp(PyObject * fast_seq, PyObject * metadata, PyObject * lt);

    virtual 
    ~_DictTreeImp();
    
    virtual PyObject *
    pop(PyObject * key, PyObject * data);

    virtual PyObject *
    find(PyObject * key);

    virtual PyObject *
    find_slice(PyObject * start, PyObject * stop);

    virtual PyObject *
    get(PyObject * key, PyObject * data);

    virtual PyObject *
    insert(PyObject * key, PyObject * data, bool overwrite);
    
    virtual int
    update_slice_data(PyObject * start, PyObject * stop, PyObject * data);

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
            false,
            MetadataTag,
            LT>     
        BaseT;            

    typedef
        typename BaseT::TreeT           
        TreeT;
};

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::_DictTreeImp(PyObject * fast_seq, PyObject * metadata, PyObject * lt) :
    BaseT(fast_seq, metadata, lt)
{
    // Do nothing.
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::~_DictTreeImp()
{
    // Do nothing.
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::pop(PyObject * key, PyObject * data)
{
    try {    
        typename BaseT::InternalValueType val = 
            BaseT::tree.erase(BaseT::key_to_internal_key(key));
        data = BaseT::internal_value_to_data_inc(val);
        BaseT::dec_internal_value(val);
        return data;
    }
    catch(const std::logic_error &) {
        if (data == NULL) {
            PyErr_SetObject(PyExc_KeyError, key);        
            return NULL;
        }     
        BANYAN_PYOBJECT_INCREF(data);
        return data;                
    }

    return NULL;
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::find(PyObject * key)
{
    typename TreeT::Iterator found = BaseT::tree.find(BaseT::key_to_internal_key(key));

    if (found == BaseT::end()) {
        PyErr_SetObject(PyExc_KeyError, key);
        return NULL;
    }

    return BaseT::internal_value_to_data_inc(*found);
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::find_slice(PyObject * start, PyObject * stop)
{
    std::pair<typename TreeT::Iterator, typename TreeT::Iterator> start_stop = 
        BaseT::start_stop_its(start, stop);
        
    PyObject * val = PyTuple_New(std::distance(start_stop.first, start_stop.second));
    if (val == NULL) {
        PyErr_NoMemory();
        return NULL;
    }    
    for (typename TreeT::Iterator it = start_stop.first; it != start_stop.second; ++it) {
        PyObject * const data = BaseT::internal_value_to_data_inc(*it);
        PyTuple_SET_ITEM(val, std::distance(start_stop.first, it), data);
    }        
    return val;
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::get(PyObject * key, PyObject * data)
{
    typename TreeT::Iterator found = BaseT::tree.find(BaseT::key_to_internal_key(key));

    if (found == BaseT::end()) {
        BANYAN_PYOBJECT_INCREF(data);
        return data;
    }

    return BaseT::internal_value_to_data_inc(*found);
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
PyObject *
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::insert(PyObject * key, PyObject * data, bool overwrite)
{
    typename BaseT::InternalValueType val = 
        BaseT::key_data_to_internal_value_inc(key, data);

    std::pair<typename TreeT::Iterator, bool> ins = 
        BaseT::tree.insert(val);
        
    if (ins.second) {
        BANYAN_PYOBJECT_INCREF(data);
        return data;
    }
        
    if (overwrite) {
        BANYAN_PYOBJECT_INCREF(data);
        BaseT::dec_internal_value(*ins.first);
        *ins.first = val;
        return data;
    }
    
    data = BaseT::internal_value_to_data_inc(*ins.first);        
    BaseT::dec_internal_value(val);        
    return data;
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
int
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::update_slice_data(
    PyObject * start, PyObject * stop, PyObject * data_fast_seq)
{
    std::pair<typename TreeT::Iterator, typename TreeT::Iterator> its = BaseT::start_stop_its(start, stop);    
    typename TreeT::Iterator b = its.first, e = its.second;

    if (std::distance(b, e) != PySequence_Fast_GET_SIZE(data_fast_seq)) {
        PyErr_SetObject(PyExc_ValueError, data_fast_seq);
        return BANYAN_VERIFY_EXCEPTION(-1);    
    }        
    
    for (size_t i = 0; i < static_cast<size_t>(PySequence_Fast_GET_SIZE(data_fast_seq)); ++i, ++b) {
        BANYAN_PYOBJECT_INCREF(PySequence_Fast_GET_ITEM(data_fast_seq, i));
        BaseT::update_data(*b, PySequence_Fast_GET_ITEM(data_fast_seq, i));
    }        
                    
    return 0;
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
void *
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::next(
    void * mem, PyObject * stop, int type, PyObject * & cur_val)
{
    DBG_ASSERT(mem != NULL);       
    
    switch(type) {
    case 0:
        cur_val = BaseT::internal_value_to_key_inc(
            BaseT::mem_value(mem));
        break;
    case 1:
        cur_val = BaseT::internal_value_to_data_inc(
            BaseT::mem_value(mem));
        break;
    case 2:
        cur_val = BaseT::internal_value_to_value_inc(
            BaseT::mem_value(mem));
        break;
    default:
        DBG_ASSERT(false);
    };    
    
    if (stop == NULL)
        return BaseT::mem_next(mem, NULL);
        
    const typename BaseT::InternalKeyType stop_k = BaseT::key_to_internal_key(stop);                        
    return BaseT::mem_next(mem, &stop_k);
}

template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
void *
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::prev(
    void * mem, PyObject * start, int type, PyObject * & cur_val)
{
    DBG_ASSERT(mem != NULL);
    
    switch(type) {
    case 0:
        cur_val = BaseT::internal_value_to_key_inc(
            BaseT::mem_value(mem));
        break;
    case 1:
        cur_val = BaseT::internal_value_to_data_inc(
            BaseT::mem_value(mem));
        break;
    case 2:
        cur_val = BaseT::internal_value_to_value_inc(
            BaseT::mem_value(mem));
        break;
    default:
        DBG_ASSERT(false);
    };
    
    if (start == NULL)
        return BaseT::mem_prev(mem, NULL);
        
    const typename BaseT::InternalKeyType start_k = BaseT::key_to_internal_key(start);                        
    return BaseT::mem_prev(mem, &start_k);
}

#ifdef BANYAN_DEBUG
template<class Alg_Tag, typename Key_Type, class MetadataTag, class LT>
void
_DictTreeImp<Alg_Tag, Key_Type, MetadataTag, LT>::assert_valid() const
{
    BaseT::tree.assert_valid();       
}
#endif // #ifdef BANYAN_DEBUG    

#endif // #ifndef _BANYAN_DICT_TREE_IMP_HPP

