#ifndef _BANYAN_TREE_IMP_HPP
#define _BANYAN_TREE_IMP_HPP

#include <Python.h>

#include "_dsa/_functional.hpp"
#include "_tree_imp_lt_base.hpp"
#include "_dbg.hpp"

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class MetadataTag,
    class LT>
class _TreeImp : 
    public _TreeImpLTBase<
        Alg_Tag,
        Key_Type,
        Set,
        MetadataTag,
        LT>
{
public:
    explicit
    _TreeImp(PyObject * fast_seq, PyObject * metadata, PyObject * lt);

    virtual 
    ~_TreeImp();
    
    virtual int
    traverse(visitproc visit, void * arg);    
    
    virtual int
    contains(PyObject * key);

    virtual PyObject *
    erase(PyObject * key);

    virtual PyObject *
    erase_return(PyObject * key);

    virtual PyObject *
    clear();

    virtual PyObject *
    pop();

    virtual void *
    begin(PyObject * start, PyObject * stop);

    virtual void *
    rbegin(PyObject * start, PyObject * stop);

    virtual PyObject *
    iter_key(void * it);

    virtual PyObject *
    lt_keys(PyObject * lhs_key, PyObject * rhs_key);

    virtual PyObject *
    erase_slice(PyObject * start, PyObject * stop);

    virtual Py_ssize_t
    size();

#ifdef BANYAN_DEBUG
    void
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG    

protected:    
    typedef 
        _TreeImpLTBase<
            Alg_Tag,
            Key_Type,
            Set,
            MetadataTag,
            LT>
        BaseT;
        
    typedef typename BaseT::TreeT TreeT;        
    
    typedef typename BaseT::InternalKeyType InternalKeyType;
    typedef typename BaseT::InternalValueType InternalValueType;
    
private:
    typedef
        _TreeImp<
            Alg_Tag,
            Key_Type,
            Set,
            MetadataTag,
            LT>
        ThisT;        
        
protected:
    std::pair<typename TreeT::Iterator, typename TreeT::Iterator>
    start_stop_its(PyObject * start, PyObject * stop);
};

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::_TreeImp(PyObject * fast_seq, PyObject * metadata, PyObject * lt) :
    BaseT(fast_seq, metadata, lt)
{
    // Do nothing.
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::~_TreeImp()
{
    clear();
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
int
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::traverse(visitproc visit, void * arg)
{
    const int base_traverse = BaseT::traverse(visit, arg);
    if (base_traverse != 0)
        return base_traverse;
            
    return 0;
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
std::pair<
    typename _TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::TreeT::Iterator, 
    typename _TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::TreeT::Iterator>
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::start_stop_its(PyObject * start, PyObject * stop)
{
    if (start == Py_None && stop == Py_None) 
        return std::make_pair(BaseT::tree.begin(), BaseT::tree.end());
        
    if (start == Py_None && stop != Py_None) {
        typename TreeT::Iterator b, e;
        b = e = BaseT::tree.begin();
        const InternalKeyType stop_k = BaseT::key_to_internal_key(stop);
        while (true) {
            if (e == BaseT::tree.end() || 
                    !BaseT::tree.less_than()(
                        TreeT::KeyExtractorT::extract(*e),
                        stop_k))
                break;    
            ++e;
        }
        return std::make_pair(b, e);
    }                    
    
    DBG_VERIFY(start != Py_None);
    
    typename TreeT::Iterator b;
    b = BaseT::tree.lower_bound(BaseT::key_to_internal_key(start));
    
    typename TreeT::Iterator e;
    if (stop == Py_None)
        e = BaseT::tree.end();
    else {
        e = b;
        while (true) {
            if (e == BaseT::tree.end() || !BaseT::tree.less_than()(
                    TreeT::KeyExtractorT::extract(*e),
                    BaseT::key_to_internal_key(stop)))
                break;    
            ++e;
        }
    }            
    
    return std::make_pair(b, e);
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
int
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::contains(PyObject * key)
{
    return BaseT::tree.find(BaseT::key_to_internal_key(key)) != BaseT::end();
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
PyObject *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::erase(PyObject * key)
{
    try {    
        InternalValueType val = BaseT::tree.erase(BaseT::key_to_internal_key(key));
        
        BaseT::dec_internal_value(val);

        Py_RETURN_NONE;
    }
    catch(const std::logic_error &) {
        PyErr_SetObject(PyExc_KeyError, key);        
        throw;
    }
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
PyObject *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::erase_return(PyObject * key)
{    
    try {    
        InternalValueType val = BaseT::tree.erase(BaseT::key_to_internal_key(key));
        PyObject * const p = BaseT::internal_value_to_value_inc(val);
        BaseT::dec_internal_value(val);
        return p;
    }
    catch(const std::logic_error &) {
        PyErr_SetObject(PyExc_KeyError, key);        
        throw;
    }
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
PyObject *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::erase_slice(PyObject * start, PyObject * stop)
{
    std::pair<typename TreeT::Iterator, typename TreeT::Iterator> its = start_stop_its(start, stop);    
    typename TreeT::Iterator b = its.first, e = its.second;
    typename TreeT::Iterator begin_ = BaseT::tree.begin();
    
    if (b == begin_ && e == BaseT::tree.end()) {
        clear();
        
        Py_RETURN_NONE;
    }
    
    if (b == BaseT::tree.end()) {
        DBG_ASSERT(e == BaseT::tree.end());

        Py_RETURN_NONE;
    }
    
    const size_t orig_size = BaseT::tree.size();        
        
    if (b == begin_ && e != BaseT::tree.end()) {
        TreeT larger(NULL, NULL, BaseT::tree.meta(), BaseT::tree.less_than());
        BaseT::tree.split(TreeT::KeyExtractorT::extract(*e), larger);  

        size_t num_erased = 0;        
        for (typename TreeT::Iterator it = BaseT::tree.begin(); it != BaseT::tree.end(); ++it) {
            ++num_erased;
            BaseT::dec_internal_value(*it);            
        }         
        
        BaseT::tree.swap(larger);   
        BaseT::tree.restore_size(orig_size - num_erased);

        DBG_ONLY(BaseT::tree.assert_valid();)

        Py_RETURN_NONE;
    }
    
    if (b != begin_ && e == BaseT::tree.end()) {
        TreeT larger(NULL, NULL, BaseT::tree.meta(), BaseT::tree.less_than());
        BaseT::tree.split(TreeT::KeyExtractorT::extract(*b), larger);  
    
        size_t num_erased = 0;        
        for (typename TreeT::Iterator it = larger.begin(); it != larger.end(); ++it) {
            ++num_erased;
            BaseT::dec_internal_value(*it);            
        }         
        
        BaseT::tree.restore_size(orig_size - num_erased);

        DBG_ONLY(BaseT::tree.assert_valid();)

        Py_RETURN_NONE;
    }
    
    const typename BaseT::KeyType start_key = TreeT::KeyExtractorT::extract(*b);
    const typename BaseT::KeyType stop_key = TreeT::KeyExtractorT::extract(*e);
    
    TreeT mid(NULL, NULL, BaseT::tree.meta(), BaseT::tree.less_than());
    BaseT::tree.split(start_key, mid);  

    TreeT right(NULL, NULL, BaseT::tree.meta(), BaseT::tree.less_than());
    // Tmp Ami - suspicious
    if (stop != Py_None)
        mid.split(stop_key, right);

    size_t num_erased = 0;        
    for (typename TreeT::Iterator it = mid.begin(); it != mid.end(); ++it) {
        ++num_erased;
        BaseT::dec_internal_value(*it);            
    }            
    
    BaseT::tree.join(right);     
    BaseT::tree.restore_size(orig_size - num_erased);

    DBG_ONLY(BaseT::tree.assert_valid();)
         
    Py_RETURN_NONE;
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
PyObject *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::clear()
{
    for (typename TreeT::Iterator it = BaseT::tree.begin(); it != BaseT::end(); ++it) 
        BaseT::dec_internal_value(*it);        

    BaseT::tree.clear();
    
    Py_RETURN_NONE;
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
PyObject *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::pop()
{
    if (BaseT::tree.size() == 0) {
        PyErr_SetString(PyExc_KeyError, "Attempting to pop an empty tree");
        
        return NULL;
    }
    
    typename BaseT::InternalValueType popped = BaseT::tree.pop();
    PyObject * const ret = BaseT::internal_value_to_value_inc(popped);
    return ret;
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
void *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::begin(PyObject * start, PyObject * stop)
{
    if (start == NULL && stop == NULL) 
        return BaseT::mem_begin(NULL, NULL);
            
    if (start == NULL && stop != NULL) {
        const InternalKeyType stop_k = BaseT::key_to_internal_key(stop);                        
        return BaseT::mem_begin(NULL, &stop_k);
    }        
    
    DBG_VERIFY(start != NULL);
    const InternalKeyType start_k = BaseT::key_to_internal_key(start);                        

    if (stop == NULL)
        return BaseT::mem_begin(&start_k, NULL);
        
    const InternalKeyType stop_k = BaseT::key_to_internal_key(stop);                        
    return BaseT::mem_begin(&start_k, &stop_k);
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
void *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::rbegin(PyObject * start, PyObject * stop)
{
    if (start == NULL && stop == NULL) 
        return BaseT::mem_rbegin(NULL, NULL);
            
    if (start == NULL && stop != NULL) {
        const InternalKeyType stop_k = BaseT::key_to_internal_key(stop);                        
        return BaseT::mem_rbegin(NULL, &stop_k);
    }        
    
    DBG_VERIFY(start != NULL);
    const InternalKeyType start_k = BaseT::key_to_internal_key(start);                        

    if (stop == NULL)
        return BaseT::mem_rbegin(&start_k, NULL);
        
    const InternalKeyType stop_k = BaseT::key_to_internal_key(stop);                        
    return BaseT::mem_rbegin(&start_k, &stop_k);
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
PyObject *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::iter_key(void * it)
{
    DBG_ASSERT(it != NULL);
    PyObject * const p = BaseT::internal_value_to_key_inc(BaseT::iter_value(it));
    return p;
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
PyObject *
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::lt_keys(PyObject * lhs_key, PyObject * rhs_key)
{
    if (BaseT::tree.less_than()(
            BaseT::key_to_internal_key(lhs_key), 
            BaseT::key_to_internal_key(rhs_key)))
        Py_RETURN_TRUE;
    
    Py_RETURN_FALSE;        
}

template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
Py_ssize_t
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::size()
{
    return BaseT::tree.size();
}

#ifdef BANYAN_DEBUG
template<class Alg_Tag, typename Key_Type, bool Set, class MetadataTag, class LT>
void
_TreeImp<Alg_Tag, Key_Type, Set, MetadataTag, LT>::assert_valid() const
{
    BaseT::tree.assert_valid();       
    DBG_ASSERT(BaseT::tree.size() != static_cast<size_t>(-1));
    typename BaseT::TreeT::Iterator it = const_cast<TreeT &>(BaseT::tree).begin();
    while (it != const_cast<TreeT &>(BaseT::tree).end()) 
        BaseT::assert_valid(*it++);

    // Tmp Ami
    // const_cast<TreeT &>(BaseT::tree).less_than().assert_valid();
    // Tmp Ami
    // const_cast<TreeT &>(BaseT::tree).meta().assert_valid();
}
#endif // #ifdef BANYAN_DEBUG    

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set>
class _TreeImp<
        Alg_Tag,
        Key_Type,
        Set,
        _PyObjectCBMetadataTag,
        std::less<Key_Type> > :
    public _TreeImp<
        Alg_Tag,
        PyObject *,
        Set, 
        _PyObjectCBMetadataTag,
        _PyObjectStdLT>        
{
public:
    typedef
        _TreeImp<
            Alg_Tag,
            PyObject *,
            Set, 
            _PyObjectCBMetadataTag,
            _PyObjectStdLT>
        BaseT;
                    
public:
    explicit
    _TreeImp(PyObject * fast_seq, PyObject * metadata, PyObject * lt) :
        BaseT(fast_seq, metadata, NULL)
    {
        _py_warn(   
            PyExc_Warning, 
            "Key-type optimization unimplemented with callback metadata.");                            
    }
};    

#endif // #ifndef _BANYAN_TREE_IMP_HPP

