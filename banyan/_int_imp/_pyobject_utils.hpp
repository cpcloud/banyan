#ifndef _PYOBJECT_UTILS_HPP
#define _PYOBJECT_UTILS_HPP

#include <Python.h>

#include <functional>
#include <memory>
#include <string>

#include "_dbg.hpp"
#include "_pymem_malloc_allocator.hpp"

#ifdef BANYAN_DEBUG
void 
_banyan_dbg_py_dummy_incref(PyObject * p, const char file[], size_t line);
void 
_banyan_dbg_py_incref(PyObject * p, const char file[], size_t line);
void 
_banyan_dbg_py_decref(PyObject * p, const char file[], size_t line);
PyObject * 
_banyan_dbg_verify_pyobject(PyObject * p, const char file[], size_t line);
PyObject *
_banyan_dbg_verify_exception(PyObject *p, const char file[], size_t line);
int 
_banyan_dbg_verify_exception(int status, const char file[], size_t line);
#define BANYAN_PYOBJECT_DUMMY_INCREF(p) _banyan_dbg_py_dummy_incref(p, __FILE__, __LINE__)
#define BANYAN_PYOBJECT_INCREF(p) _banyan_dbg_py_incref(p, __FILE__, __LINE__)
#define BANYAN_PYOBJECT_DECREF(p) _banyan_dbg_py_decref(p, __FILE__, __LINE__)
#define BANYAN_PYOBJECT_VERIFY(p) _banyan_dbg_verify_pyobject(p, __FILE__, __LINE__)
#define BANYAN_VERIFY_EXCEPTION(p) _banyan_dbg_verify_exception(p, __FILE__, __LINE__)
#else // #ifdef BANYAN_DEBUG
#define BANYAN_PYOBJECT_DUMMY_INCREF(p)
#define BANYAN_PYOBJECT_INCREF(p) Py_INCREF(p)
#define BANYAN_PYOBJECT_DECREF(p) Py_DECREF(p)
#define BANYAN_PYOBJECT_VERIFY(p) p
#define BANYAN_VERIFY_EXCEPTION(p) p
#endif // #ifdef BANYAN_DEBUG

void
_py_warn(PyObject * type, const std::string & msg);

class _PyObjectStdLT : 
    public std::binary_function<PyObject * , PyObject * , bool> 
{
public:
    inline bool 
    operator()(const PyObject * lhs, const PyObject * rhs) const;

#ifdef BANYAN_DEBUG
    void 
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG
};

inline bool 
_PyObjectStdLT::operator()(
    const PyObject * lhs, const PyObject * rhs) const
{
    return PyObject_RichCompareBool(
        BANYAN_PYOBJECT_VERIFY(const_cast<PyObject *>(lhs)),
        BANYAN_PYOBJECT_VERIFY(const_cast<PyObject *>(rhs)),
        Py_LT);
}  

class _PyObjectCmpCBLT : 
    public std::binary_function<PyObject * , PyObject * , bool> 
{
public:
    explicit _PyObjectCmpCBLT(PyObject * cb);    
    _PyObjectCmpCBLT(const _PyObjectCmpCBLT & other);
    
    ~_PyObjectCmpCBLT();
    
    bool 
    operator()(const PyObject * lhs, const PyObject * rhs) const;
    
    int 
    traverse(visitproc visit, void * arg);
    
#ifdef BANYAN_DEBUG
    void 
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG

private:
    _PyObjectCmpCBLT &
    operator=(const _PyObjectCmpCBLT & other);    

private:
    PyObject * cb;    
};  

class _PyObjectKeyCBLT : 
    public std::binary_function<PyObject * , PyObject * , bool> 
{
public:
    explicit _PyObjectKeyCBLT(PyObject * cb);
    _PyObjectKeyCBLT(const _PyObjectKeyCBLT & other);    
    ~_PyObjectKeyCBLT();
    
    _PyObjectKeyCBLT &
    operator=(const _PyObjectKeyCBLT & other);
    
    bool 
    operator()(const PyObject * lhs, const PyObject * rhs) const;
    
    int 
    traverse(visitproc visit, void * arg);

#ifdef BANYAN_DEBUG
    void 
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG

private:
    PyObject * cb;    
};  

class _PyObjectCBMetadata 
{
public:
    explicit _PyObjectCBMetadata(PyObject * cbs);        
    _PyObjectCBMetadata(const _PyObjectCBMetadata & other);    
    ~_PyObjectCBMetadata();
    
    _PyObjectCBMetadata &
    operator=(const _PyObjectCBMetadata & other);
        
    void 
    update(PyObject * value, const _PyObjectCBMetadata * l, const _PyObjectCBMetadata * r);

    int 
    traverse(visitproc visit, void * arg);
    
#ifdef BANYAN_DEBUG
    void 
    assert_valid() const;

    void
    trace(std::ostream & ) const;
#endif // #ifdef BANYAN_DEBUG        
    
    PyObject *
    meta();
    
private:
    void
    get_meta();    

private:
    PyObject * cbs;    
    
    PyObject * meta_;
};  

struct _TupleKeyExtractor :
    public std::unary_function<PyObject *, PyObject *>
{
    typedef PyObject * KeyType;
    typedef PyObject * ValueType;

    inline static KeyType 
    extract(const PyObject * p)
    {
        DBG_ASSERT(is_valid_tuple(p));
        return BANYAN_PYOBJECT_VERIFY(
            PyTuple_GET_ITEM(BANYAN_PYOBJECT_VERIFY(const_cast<PyObject *>(p)), 0));
    }
    
    static bool 
    is_valid_tuple(const PyObject * p)
    {
        if (!PyTuple_Check(BANYAN_PYOBJECT_VERIFY(const_cast<PyObject *>(p))))
            return false;
        return PyTuple_Size(const_cast<PyObject *>(p)) == 2;
    }
};

template<
    class LT>
struct TupleLT :
    public std::binary_function<PyObject *, PyObject *, bool>
{
    TupleLT(const LT & lt_) : 
        lt(lt_)
    {
        // Do nothing.
    }        
    
    inline bool
    operator()(const PyObject * lhs, const PyObject * rhs) const
    {
         return lt(_TupleKeyExtractor::extract(lhs), _TupleKeyExtractor::extract(rhs));
    }
    
#ifdef BANYAN_DEBUG
    void 
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG

private:
    LT lt;    
};    

typedef
    std::basic_string<
        char,
        std::char_traits<char>,
        PyMemMallocAllocator<char> >
    String;                        

typedef
    std::basic_string<
        Py_UNICODE,
        std::char_traits<Py_UNICODE>,
        PyMemMallocAllocator<Py_UNICODE> >
    UnicodeString;                        

template<typename T>
struct _KeyFactory
{
    // Nothing.
};

template<>
struct _KeyFactory<
    double>
{
    static inline double 
    convert(PyObject * p)
    {    
        const double d = PyFloat_AsDouble(p);
        
        if (PyErr_Occurred() && d == -1) {
            PyErr_SetObject(PyExc_TypeError, p);
            throw std::logic_error("PyFloat_AsDouble failed");
        }
        
        return d;
    }

    static inline PyObject *
    convert_inc(double d)
    {
        PyObject * const p = PyFloat_FromDouble(d);
        if (p == NULL)
            throw std::bad_alloc();
        return p;            
    }    
};

struct _CachedKeyPyObject 
{
    _CachedKeyPyObject();
    _CachedKeyPyObject(PyObject * orig, PyObject * cached_key);
    _CachedKeyPyObject(const _CachedKeyPyObject & other);
    
    ~_CachedKeyPyObject();
    
    _CachedKeyPyObject &
    operator=(const _CachedKeyPyObject & other);
    
    void dec();

    int
    traverse(visitproc visit, void * arg);
    
    inline PyObject *
    key_inc() const
    {
        DBG_ASSERT(orig != NULL);
        BANYAN_PYOBJECT_INCREF(orig);
        return orig;
    }

#ifdef BANYAN_DEBUG
    void 
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG

    PyObject * orig, * cached_key;
};

struct _CachedKeyPyObjectCacheGeneratorLT : 
    public std::unary_function<PyObject *, _CachedKeyPyObject>,
    public std::binary_function<_CachedKeyPyObject, _CachedKeyPyObject, bool>    
{
    _CachedKeyPyObjectCacheGeneratorLT(PyObject * key_fn);   
    _CachedKeyPyObjectCacheGeneratorLT(const _CachedKeyPyObjectCacheGeneratorLT & other);
    ~_CachedKeyPyObjectCacheGeneratorLT();
    
    _CachedKeyPyObjectCacheGeneratorLT &
    operator=(const _CachedKeyPyObjectCacheGeneratorLT & other);

    _CachedKeyPyObject 
    operator()(PyObject * orig) const;

    bool 
    operator()(const _CachedKeyPyObject & lhs, const _CachedKeyPyObject & rhs) const;
    
    int
    traverse(visitproc visit, void * arg);

#ifdef BANYAN_DEBUG
    void 
    assert_valid() const;
#endif // #ifdef BANYAN_DEBUG

    PyObject * key_fn;
};


template<>
struct _KeyFactory<
    long>
{
    static inline long
    convert(PyObject * p)
    {
#if PY_MAJOR_VERSION >= 3
        const long i = PyLong_AsLong(p);
#else // #if PY_MAJOR_VERSION >= 3
        const long i = PyInt_AsLong(p);
#endif // #if PY_MAJOR_VERSION >= 3
        
        if (PyErr_Occurred() && i == -1) {
            PyErr_SetObject(PyExc_TypeError, p);
            throw std::logic_error("PyInt_AsLong failed");
        }
        
        return i;
    }

    static inline PyObject *
    convert_inc(long i)
    {
#if PY_MAJOR_VERSION >= 3
        PyObject * const p = PyLong_FromLong(i);
#else // #if PY_MAJOR_VERSION >= 3
        PyObject * const p = PyInt_FromLong(i);
#endif // #if PY_MAJOR_VERSION >= 3
        if (p == NULL)
            throw std::bad_alloc();
        return p;            
    }    
};

template<typename First_Type, typename Second_Type>
struct _KeyFactory<
    std::pair<First_Type, Second_Type> >
{
    static inline std::pair<First_Type, Second_Type>
    convert(PyObject * p)
    {
        // Tmp Ami - check in interval shit if length is checked.
        if (!PySequence_Check(p) || PySequence_Length(p) != 2) {
            PyErr_SetObject(PyExc_TypeError, p);
            throw std::logic_error("PyInt_AsLong failed");
        }
        
        return std::make_pair(
            _KeyFactory<First_Type>::convert(PySequence_GetItem(p, 0)),
            _KeyFactory<Second_Type>::convert(PySequence_GetItem(p, 1)));
    }

    static inline PyObject *
    convert_inc(const std::pair<First_Type, Second_Type> i)
    {
        PyObject * const p = PyTuplePack(
            2,
            _KeyFactory<First_Type>::convert_inc(i.first),
            _KeyFactory<Second_Type>::convert_inc(i.second));
        if (p == NULL)
            throw std::bad_alloc();
        return p;            
    }    
};

#if PY_MAJOR_VERSION >= 3
template<>
struct _KeyFactory<
    String>
{
    static String
    convert(PyObject * p)
    {
        PyObject * const bytes = PyByteArray_FromObject(p);
        if (bytes == NULL) {
            PyErr_SetObject(PyExc_TypeError, p);
            throw std::logic_error("PyByteArray_FromObject failed");
        }
         
        const char * const c = PyByteArray_AsString(bytes);
        DBG_VERIFY(c != NULL);

        const Py_ssize_t size = PyByteArray_Size(bytes);
        
        return String(c, c + size);
    }

    static inline PyObject *
    convert_inc(const String & s)
    {
        PyObject * const p = PyByteArray_FromStringAndSize(&s[0], s.size());
        if (p == NULL)
            throw std::bad_alloc();
        return p;            
    }    
};
#else // #if PY_MAJOR_VERSION >= 3
template<>
struct _KeyFactory<
    String>
{
    static String
    convert(PyObject * p)
    {
        if (!PyString_Check(p)) {
            PyErr_SetObject(PyExc_TypeError, p);
            throw std::logic_error("PyString_Check");
        }
         
        char * c;
        Py_ssize_t size;
        DBG_VERIFY(PyString_AsStringAndSize(p, &c, &size) != -1);        
        
        return String(c, c + size);
    }

    static inline PyObject *
    convert_inc(const String & s)
    {
        PyObject * const p = PyString_FromStringAndSize(&s[0], s.size());
        if (p == NULL)
            throw std::bad_alloc();
        return p;            
    }    
};
#endif // #if PY_MAJOR_VERSION >= 3

template<>
struct _KeyFactory<
    UnicodeString>
{
    static UnicodeString
    convert(PyObject * p)
    {
        if (!PyUnicode_Check(p)) {
            PyErr_SetObject(PyExc_TypeError, p);
            throw std::logic_error("PyUnicode_AS_UNICODE failed");
        }
        
        const Py_UNICODE * const c = PyUnicode_AS_UNICODE(p);
        const Py_ssize_t size = PyUnicode_GET_SIZE(p);
        
        return UnicodeString(c, c + size);
    }

    static inline PyObject *
    convert_inc(const UnicodeString & s)
    {
        PyObject * const p = PyUnicode_FromUnicode(&s[0], s.size());
        if (p == NULL)
            throw std::bad_alloc();
        return p;            
    }    
};

template<>
struct _KeyFactory<
    PyObject *>
{
    static inline PyObject *
    convert(PyObject * p)
    {
        return p;
    }

    static inline PyObject *
    convert_inc(PyObject * const p)
    {
        BANYAN_PYOBJECT_INCREF(p);
        return p;
    }    
};

#endif // #ifndef _PYOBJECT_UTILS_HPP

