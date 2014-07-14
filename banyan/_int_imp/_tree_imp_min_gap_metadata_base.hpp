#ifndef _TREE_IMP_MIN_GAP_METADATA_BASE_HPP
#define _TREE_IMP_MIN_GAP_METADATA_BASE_HPP

#include <Python.h>

#include <climits>
#include <cfloat>

#include "_dbg.hpp"
#include "_dsa/_min_gap_updator.hpp"

struct _MinGapMetadataTag{};

template<typename Key_Type>
struct __MinGapMetadata
{
    __MinGapMetadata() 
    {
        PyErr_SetString(PyExc_TypeError, "MinGapUpdator incompatible with type");
        throw std::logic_error("MinGapUpdator incompatible with type");    
    }

    template<typename T, class C>
    void 
    update(const T &, const C *, const C *)
    {
        DBG_VERIFY(false);
    }
    
    PyObject *
    repr_inc() const
    {
        DBG_VERIFY(false);
        return NULL;
    }
    
    PyObject *
    val_inc() const
    {
        DBG_VERIFY(false);
        return NULL;
    }

    int 
    traverse(visitproc, void *)
    {
        DBG_VERIFY(false);
        return 0;
    }

    Key_Type min_gap;
};

template<>
struct __MinGapMetadata<long> :
    public _MinGapMetadata<long> 
{
    typedef __MinGapMetadata<long> ThisT;

    void 
    update(const std::pair<long, PyObject *> & key, const ThisT * l, const ThisT * r)
    {
        _MinGapMetadata<long>::update(key.first, l, r);
    }
    
    PyObject *
    repr_inc() const
    {
        if (min_gap < 0)
            Py_RETURN_NONE;
            
#if PY_MAJOR_VERSION >= 3
        return PyLong_FromLong(min_gap);
#else // #if PY_MAJOR_VERSION >= 3
        return PyInt_FromLong(min_gap);            
#endif // #if PY_MAJOR_VERSION >= 3                    
    }
    
    PyObject *
    val_inc() const
    {
        if (min_gap < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Min-gap undefined");          
            return NULL;
        }            
            
#if PY_MAJOR_VERSION >= 3
        return PyLong_FromLong(min_gap);
#else // #if PY_MAJOR_VERSION >= 3
        return PyInt_FromLong(min_gap);            
#endif // #if PY_MAJOR_VERSION >= 3                    
    }

    int 
    traverse(visitproc, void *)
    {
        return 0;
    }
};

template<>
struct __MinGapMetadata<double> :
    public _MinGapMetadata<double> 
{
    typedef __MinGapMetadata<double> ThisT;

    void 
    update(const std::pair<double, PyObject *> & key, const ThisT * l, const ThisT * r)
    {
        _MinGapMetadata<double>::update(key.first, l, r);
    }

    PyObject *
    repr_inc() const
    {
        if (min_gap < 0)
            Py_RETURN_NONE;
            
        return PyFloat_FromDouble(min_gap);            
    }
    
    PyObject *
    val_inc() const
    {
        if (min_gap < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Min-gap undefined");          
            return NULL;
        }            
            
        return PyFloat_FromDouble(min_gap);            
    }

    int 
    traverse(visitproc, void *)
    {
        return 0;
    }
};

template<>
struct __MinGapMetadata<PyObject *>
{
    typedef __MinGapMetadata<PyObject *> ThisT;
    
    __MinGapMetadata() :
        min_(NULL),
        max_(NULL),
        min_gap(NULL)
    {
        // Do nothing.
    }        
    
    __MinGapMetadata(const ThisT & other)  :
        min_(other.min_),
        max_(other.max_),
        min_gap(other.min_gap)
    {
        if (min_ != NULL) 
            BANYAN_PYOBJECT_INCREF(min_);
        if (max_ != NULL) 
            BANYAN_PYOBJECT_INCREF(max_);
        if (min_gap != NULL) 
            BANYAN_PYOBJECT_INCREF(min_gap);
    }
    
    __MinGapMetadata & 
    operator=(const ThisT & other) 
    {
        if (this != &other)
        {
            if (min_ != NULL) 
                BANYAN_PYOBJECT_DECREF(min_);
            if (max_ != NULL) 
                BANYAN_PYOBJECT_DECREF(max_);
            if (min_gap != NULL) 
                BANYAN_PYOBJECT_DECREF(min_gap);
                
            min_ = other.min_;
            max_ = other.max_;
            min_gap = other.min_gap;
            
            if (min_ != NULL) 
                BANYAN_PYOBJECT_INCREF(min_);
            if (max_ != NULL) 
                BANYAN_PYOBJECT_INCREF(max_);
            if (min_gap != NULL) 
                BANYAN_PYOBJECT_INCREF(min_gap);
        }
        
        return *this;
    }
    
    virtual ~__MinGapMetadata()
    {
        if (min_ != NULL) 
            BANYAN_PYOBJECT_DECREF(min_);
        if (max_ != NULL) 
            BANYAN_PYOBJECT_DECREF(max_);
        if (min_gap != NULL) 
            BANYAN_PYOBJECT_DECREF(min_gap);
    }

    void 
    update(PyObject * key, const ThisT * l, const ThisT * r)
    {
        if (min_ != NULL) 
            BANYAN_PYOBJECT_DECREF(min_);
        if (max_ != NULL) 
            BANYAN_PYOBJECT_DECREF(max_);
        if (min_gap != NULL) 
            BANYAN_PYOBJECT_DECREF(min_gap);
        
        min_ = max_ = key;
        min_gap = NULL;
        
        if (l != NULL) {
            min_ = l->min_;
            
            min_gap = child_min_gap_inc(key, l->max_, l->min_gap);
            DBG_ASSERT(min_gap != NULL);
        }
        if (r != NULL) {
            max_ = r->max_;
            
            PyObject * const right_min_gap = child_min_gap_inc(key, r->min_, r->min_gap);
            
            if (min_gap == NULL)
                min_gap = right_min_gap;
            else if (min_gap != NULL && PyObject_RichCompareBool(right_min_gap, min_gap, Py_LT)) {
                BANYAN_PYOBJECT_DECREF(min_gap);
                min_gap = right_min_gap;
            }
            else
                BANYAN_PYOBJECT_DECREF(right_min_gap);                            
            DBG_ASSERT(min_gap != NULL);
        }
        
        DBG_VERIFY((l == NULL && r == NULL) || min_gap != NULL);
        
        BANYAN_PYOBJECT_INCREF(min_);
        BANYAN_PYOBJECT_INCREF(max_);
    }
    
    PyObject * 
    child_min_gap_inc(PyObject * key, PyObject * child_ext, PyObject * child_min_gap) 
    {
        PyObject * const gap = PyNumber_Subtract(key, child_ext);
        if (gap == NULL) {
            PyErr_SetString(PyExc_TypeError, "Failed to subtract");
            throw std::logic_error("Failed to subtract");
        }
        BANYAN_PYOBJECT_DUMMY_INCREF(gap);
        PyObject * const abs_gap = PyNumber_Absolute(gap);
        if (abs_gap == NULL) {
            PyErr_SetString(PyExc_TypeError, "Failed to take absolute value");
            throw std::logic_error("Failed to take absolute value");
        }
        BANYAN_PYOBJECT_DUMMY_INCREF(abs_gap);
        BANYAN_PYOBJECT_DECREF(gap);     
        
        if (child_min_gap == NULL || PyObject_RichCompareBool(abs_gap, child_min_gap, Py_LE))
            return abs_gap;
 
        BANYAN_PYOBJECT_DECREF(abs_gap);
        BANYAN_PYOBJECT_INCREF(child_min_gap);
        return child_min_gap;
    }

    int 
    traverse(visitproc visit, void * arg)
    {
        if (min_ != NULL)
            Py_VISIT(min_);
        if (max_ != NULL)
            Py_VISIT(max_);
        if (min_gap != NULL)            
            Py_VISIT(min_gap);
        return 0;            
    }

    PyObject *
    repr_inc() const
    {
        if (min_gap == NULL)
            Py_RETURN_NONE;
            
        BANYAN_PYOBJECT_INCREF(min_gap);
        return min_gap;            
    }
    
    PyObject *
    val_inc() const
    {
        if (min_gap == NULL) { 
            PyErr_SetString(PyExc_RuntimeError, "Min-gap undefined");          
            
            return NULL;
        }            
            
        BANYAN_PYOBJECT_INCREF(min_gap);
        return min_gap;            
    }

    PyObject * min_, * max_, * min_gap;
};

template<
    class Alg_Tag,
    typename Key_Type,
    bool Set,
    class LT>
struct _TreeImpMetadataBase<
        Alg_Tag,
        Key_Type,
        Set,
        _MinGapMetadataTag,
        LT> :
    public _TreeImpValueTypeBase<
        Alg_Tag,
        Key_Type,
        Set,
        __MinGapMetadata<Key_Type>,
        LT>    
{
public: 
    explicit
    _TreeImpMetadataBase(PyObject * seq, PyObject * metadata, const LT & lt) :
        BaseT(seq, __MinGapMetadata<Key_Type>(), lt)
    {
        DBG_ASSERT(seq != NULL);
        DBG_ASSERT(metadata == NULL);
    }   

    virtual PyObject *
    iter_metadata(void * it)
    {                
        return BaseT::iter_internal_metadata(it).repr_inc();
    }

    virtual PyObject *
    min_gap_updator_min_gap()
    {    
        void * const root = BaseT::root_iter();
        if (root == NULL) {
            PyErr_SetString(PyExc_RuntimeError, "Min-gap undefined");          
            return NULL;
        }            

        PyObject * const ret = BaseT::iter_internal_metadata(root).val_inc();    
        
        BaseT::delete_node_iter(root);
        
        return ret;
    }

    int
    traverse(visitproc visit, void * arg)
    {
        int v = BaseT::traverse(visit, arg);
        if (v)
            return v;
    
        v = BaseT::tree.meta().traverse(visit, arg);        
        if (v)
            return v;
            
        // Tmp Ami
        /*            
        for (typename TreeT::Iterator it = BaseT::tree.begin(); it != BaseT::end(); ++it) {
            Py_VISIT(it->first.second);            
            Py_VISIT(it->second);
        }*/
                
        return 0;            
    }

protected:
    typedef
        _TreeImpValueTypeBase<
            Alg_Tag,
            Key_Type,
            Set,
            __MinGapMetadata<Key_Type>,
            LT>    
        BaseT;
        
    typedef typename BaseT::TreeT TreeT;        
};

#endif // #ifndef _TREE_IMP_MIN_GAP_METADATA_BASE_HPP
    
