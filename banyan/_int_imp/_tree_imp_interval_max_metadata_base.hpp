#ifndef _TREE_IMP_INTERVAL_MAX_METADATA_BASE_HPP
#define _TREE_IMP_INTERVAL_MAX_METADATA_BASE_HPP

#include <Python.h>

#include "_dbg.hpp"

struct _IntervalMaxMetadataTag{};

template<typename Type>
struct _IntervalMaxMetadata
{
    typedef
        std::pair<Type, Type>
        IntervalT;

    void 
    update(
        const std::pair<IntervalT, PyObject *> & key, 
        const _IntervalMaxMetadata<Type> * l, const _IntervalMaxMetadata<Type> * r)
    {
        max_ = key.first.second;
        
        if (l != NULL) 
            max_ = std::max<Type>(max_, l->max_);
        if (r != NULL) 
            max_ = std::max<Type>(max_, r->max_);
    }

    int 
    traverse(visitproc, void *)
    {
        return 0;            
    }

    Type max_;
};

struct _PyObjectIntervalMaxMetadata
{
    _PyObjectIntervalMaxMetadata() :
        max_(NULL)
    {
        // Do nothing.
    }        
    
    _PyObjectIntervalMaxMetadata(const _PyObjectIntervalMaxMetadata & other)  :
        max_(other.max_)
    {
        if (max_ != NULL) 
            BANYAN_PYOBJECT_INCREF(max_);
    }
    
    _PyObjectIntervalMaxMetadata & 
    operator=(const _PyObjectIntervalMaxMetadata & other) 
    {
        if (this != &other)
        {
            if (max_ != NULL) 
                BANYAN_PYOBJECT_DECREF(max_);
                
            max_ = other.max_;
            
            if (max_ != NULL) 
                BANYAN_PYOBJECT_INCREF(max_);
        }
        
        return *this;
    }
    
    virtual ~_PyObjectIntervalMaxMetadata()
    {
        if (max_ != NULL) 
            BANYAN_PYOBJECT_DECREF(max_);
    }

    void 
    update(PyObject * key, const _PyObjectIntervalMaxMetadata * l, const _PyObjectIntervalMaxMetadata * r)
    {
        if (max_ != NULL) 
            BANYAN_PYOBJECT_DECREF(max_);
            
        if (!PySequence_Check(key) || 
                (max_ = PySequence_Fast_GET_ITEM(key, 1)) == NULL) {
            PyErr_SetObject(PyExc_TypeError, key);            
            throw std::logic_error("Can't take [1]");
        }
                
        if (l != NULL) {
            if (PyObject_RichCompareBool(max_, l->max_, Py_LT))
                max_ = l->max_;
        }                
        if (r != NULL) {
            if (PyObject_RichCompareBool(max_, r->max_, Py_LT))
                max_ = r->max_;
        }                
        
        BANYAN_PYOBJECT_INCREF(max_);
    }

    int 
    traverse(visitproc visit, void * arg)
    {
        if (max_ != NULL)
            Py_VISIT(max_);
        return 0;            
    }

    PyObject * max_;
};

template<
    class Alg_Tag,
    typename Type,
    bool Set,
    class LT>
struct _TreeImpMetadataBase<
        Alg_Tag,
        Type,
        Set,
        _IntervalMaxMetadataTag,
        LT> :
    public _TreeImpValueTypeBase<
        Alg_Tag,
        Type,
        Set,
        _NullMetadata,
        LT>    
{
public: 
    explicit
    _TreeImpMetadataBase(PyObject * seq, PyObject * metadata, const LT & lt) :
        BaseT(seq, _NullMetadata(), lt)
    {
        PyErr_SetString(PyExc_TypeError, "Key type incompatible with updator");
        throw std::logic_error("Key type incompatible with updator");
    }   

    virtual PyObject *
    iter_metadata(void * it)
    {                
        DBG_VERIFY(false);
        return NULL;
    }

    virtual PyObject *
    interval_max_updator_overlapping(PyObject * b, PyObject * e)
    {
        DBG_VERIFY(false);
        return NULL;
    }

    virtual PyObject *
    interval_max_updator_overlapping(PyObject * p)
    {    
        DBG_VERIFY(false);
        return NULL;
    }
    
protected:    
    typedef
        _TreeImpValueTypeBase<
            Alg_Tag,
            Type,
            Set,
            _NullMetadata,
            LT>    
        BaseT;
};

template<
    class Alg_Tag,
    typename Type,
    bool Set>
struct _TreeImpMetadataBase<
        Alg_Tag,
        std::pair<Type, Type>,
        Set,
        _IntervalMaxMetadataTag,
        std::less<std::pair<Type, Type> > > :
    public _TreeImpValueTypeBase<
        Alg_Tag,
        std::pair<Type, Type>,
        Set,
        _IntervalMaxMetadata<Type>,
        std::less<std::pair<Type, Type> > >    
{
public: 
    explicit
    _TreeImpMetadataBase(PyObject * seq, PyObject * metadata, const std::less<std::pair<Type, Type> > &) :
        BaseT(seq, _IntervalMaxMetadata<Type>(), std::less<std::pair<Type, Type> >())
    {
        DBG_ASSERT(seq != NULL);
        DBG_ASSERT(metadata == NULL);
    }   

    virtual PyObject *
    iter_metadata(void * it)
    {                
        return _KeyFactory<Type>::convert_inc(BaseT::iter_internal_metadata(it).max_);
    }

    virtual PyObject *
    interval_max_updator_overlapping(PyObject * b, PyObject * e)
    {
        PyObject * const l = PyList_New(0);
        if (l == NULL) {
            PyErr_NoMemory();
            throw std::bad_alloc();
        }
        
        if (BaseT::tree.size() == 0)
            return l;

        interval_max_updator_overlapping(
            _KeyFactory<Type>::convert(b), 
            _KeyFactory<Type>::convert(e), 
            BaseT::root_iter(), 
            l);
        
        return l;
    }

    virtual PyObject *
    interval_max_updator_overlapping(PyObject * p)
    {    
        PyObject * const l = PyList_New(0);
        if (l == NULL) {
            PyErr_NoMemory();
            throw std::bad_alloc();
        }
        
        if (BaseT::tree.size() == 0)
            return l;
        
        interval_max_updator_overlapping(
            _KeyFactory<Type>::convert(p), 
            BaseT::root_iter(), 
            l);
        
        return l;
    }
    
protected:
    typedef
        _TreeImpValueTypeBase<
            Alg_Tag,
            std::pair<Type, Type>,
            Set,
            _IntervalMaxMetadata<Type>,
            std::less<std::pair<Type, Type> > >    
        BaseT;
        
    typedef typename BaseT::TreeT TreeT;        

private:    
    void
    interval_max_updator_overlapping(Type b_, Type e_, void * node, PyObject * l)
    {
        const std::pair<Type, Type> interval = 
            BaseT::internal_value_to_internal_key(BaseT::iter_value(node)).first;        
        const Type b = interval.first;
        const Type e = interval.second;
        
        void * const left_node = BaseT::left_iter(node);
        if (left_node != NULL) {            
            if (b_ <= BaseT::iter_internal_metadata(left_node).max_) 
                interval_max_updator_overlapping(b_, e_, left_node, l);
            else                
                BaseT::delete_node_iter(left_node);       
        }            
                
        if (b <= e_ && b_ <= e) 
           if (PyList_Append(l, BaseT::internal_value_to_key_inc(BaseT::iter_value(node))) == -1) {
                PyErr_NoMemory();
                std::bad_alloc();
            }
            
        void * const right_node = BaseT::right_iter(node);
        if (right_node != NULL) {            
            if (b_ <= BaseT::iter_internal_metadata(right_node).max_) 
                interval_max_updator_overlapping(b_, e_, right_node, l);
            else                
                BaseT::delete_node_iter(right_node);       
        }            
        
        BaseT::delete_node_iter(node);       
    }

    void
    interval_max_updator_overlapping(Type p, void * node, PyObject * l)
    {    
        const std::pair<Type, Type> interval = 
            BaseT::internal_value_to_internal_key(BaseT::iter_value(node)).first;        
        const Type b = interval.first;
        const Type e = interval.second;
        
        void * const left_node = BaseT::left_iter(node);
        if (left_node != NULL) {            
            if (p <= BaseT::iter_internal_metadata(left_node).max_) 
                interval_max_updator_overlapping(p, left_node, l);
            else
                BaseT::delete_node_iter(left_node);       
        }            
                
        if (b <= p && p <= e) 
            if (PyList_Append(l, BaseT::internal_value_to_key_inc(BaseT::iter_value(node))) == -1) {
                PyErr_NoMemory();
                std::bad_alloc();
            }
            
        void * const right_node = BaseT::right_iter(node);
        if (right_node != NULL) {
            if (p <= BaseT::iter_internal_metadata(right_node).max_) 
                interval_max_updator_overlapping(p, right_node, l);
            else
                BaseT::delete_node_iter(right_node);       
        }            
        
        BaseT::delete_node_iter(node);       
    }
};

template<
    class Alg_Tag,
    bool Set,
    class LT>
struct _TreeImpMetadataBase<
        Alg_Tag,
        PyObject *,
        Set,
        _IntervalMaxMetadataTag,
        LT> :
    public _TreeImpValueTypeBase<
        Alg_Tag,
        PyObject *,
        Set,
        _PyObjectIntervalMaxMetadata,
        LT>    
{
public: 
    explicit
    _TreeImpMetadataBase(PyObject * seq, PyObject * metadata, const LT & lt) :
        BaseT(seq, _PyObjectIntervalMaxMetadata(), lt)
    {
        DBG_ASSERT(seq != NULL);
        DBG_ASSERT(metadata == NULL);
    }   

    virtual PyObject *
    iter_metadata(void * it)
    {                
        PyObject * const max_ = BaseT::iter_internal_metadata(it).max_;

        BANYAN_PYOBJECT_INCREF(max_);
        
        return max_;
    }

    virtual PyObject *
    interval_max_updator_overlapping(PyObject * b, PyObject * e)
    {
        PyObject * const l = PyList_New(0);
        if (l == NULL) {
            PyErr_NoMemory();
            throw std::bad_alloc();
        }
        
        if (BaseT::tree.size() == 0)
            return l;

        interval_max_updator_overlapping(b, e, BaseT::root_iter(), l);
        
        return l;
    }

    virtual PyObject *
    interval_max_updator_overlapping(PyObject * p)
    {    
        PyObject * const l = PyList_New(0);
        if (l == NULL) {
            PyErr_NoMemory();
            throw std::bad_alloc();
        }
        
        if (BaseT::tree.size() == 0)
            return l;
        
        interval_max_updator_overlapping(p, BaseT::root_iter(), l);
        
        return l;
    }
    
protected:
    typedef
        _TreeImpValueTypeBase<
            Alg_Tag,
            PyObject *,
            Set,
            _PyObjectIntervalMaxMetadata,
            LT>    
        BaseT;
        
    typedef typename BaseT::TreeT TreeT;        

private:    
    void
    interval_max_updator_overlapping(PyObject * b_, PyObject * e_, void * node, PyObject * l)
    {
        PyObject * const interval = BaseT::internal_value_to_key_inc(BaseT::iter_value(node));        
        PyObject * const b = PySequence_Fast_GET_ITEM(interval, 0);
        PyObject * const e = PySequence_Fast_GET_ITEM(interval, 1);
        DBG_ASSERT(b != NULL && e != NULL);
        
        void * const left_node = BaseT::left_iter(node);
        if (left_node != NULL) {            
            if (PyObject_RichCompareBool(b_, BaseT::iter_internal_metadata(left_node).max_, Py_LE)) 
                interval_max_updator_overlapping(b_, e_, left_node, l);
            else                
                BaseT::delete_node_iter(left_node);       
        }            
                
        if (PyObject_RichCompareBool(b, e_, Py_LE) && PyObject_RichCompareBool(b_, e, Py_LE)) {
            if (PyList_Append(l, interval) == -1) {
                PyErr_NoMemory();
                std::bad_alloc();
            }
        }            
        else
            BANYAN_PYOBJECT_DECREF(interval);      
            
        void * const right_node = BaseT::right_iter(node);
        if (right_node != NULL) {            
            if (PyObject_RichCompareBool(b_, BaseT::iter_internal_metadata(right_node).max_, Py_LE)) 
                interval_max_updator_overlapping(b_, e_, right_node, l);
            else                
                BaseT::delete_node_iter(right_node);       
        }            
        
        BaseT::delete_node_iter(node);       
    }

    void
    interval_max_updator_overlapping(PyObject * p, void * node, PyObject * l)
    {    
        PyObject * const interval = BaseT::internal_value_to_key_inc(BaseT::iter_value(node));        
        PyObject * const b = PySequence_Fast_GET_ITEM(interval, 0);
        PyObject * const e = PySequence_Fast_GET_ITEM(interval, 1);
        
        void * const left_node = BaseT::left_iter(node);
        if (left_node != NULL) {            
            if (PyObject_RichCompareBool(p, BaseT::iter_internal_metadata(left_node).max_, Py_LE)) 
                interval_max_updator_overlapping(p, left_node, l);
            else
                BaseT::delete_node_iter(left_node);       
        }            
                
        if (PyObject_RichCompareBool(b, p, Py_LE) && PyObject_RichCompareBool(p, e, Py_LE)) {
            if (PyList_Append(l, interval) == -1) {
                PyErr_NoMemory();
                std::bad_alloc();
            }
        }            
        else
            BANYAN_PYOBJECT_DECREF(interval);
                    
        void * const right_node = BaseT::right_iter(node);
        if (right_node != NULL) {            
            if (PyObject_RichCompareBool(p, BaseT::iter_internal_metadata(right_node).max_, Py_LE)) 
                interval_max_updator_overlapping(p, right_node, l);
            else
                BaseT::delete_node_iter(right_node);       
        }            
        
        BaseT::delete_node_iter(node);       
    }
};

#endif // #ifndef _TREE_IMP_INTERVAL_MAX_METADATA_BASE_HPP
    
