#include <Python.h>

#include <map>

#include "_pyobject_utils.hpp"

using namespace std;

#ifdef BANYAN_DEBUG
typedef map<PyObject *, size_t> ref_counts;

ref_counts counts;

PyObject * 
_banyan_dbg_verify_pyobject(PyObject * p, const char file[], size_t line)
{
    DBG_ASSERT(p != NULL);
    if (Py_REFCNT(p) <= 0) {
        WARN("Illegal ref " << p << " " << Py_REFCNT(p) << " " << file << " " << line);
        DBG_VERIFY(false);
    }
    return p;
}

void 
_banyan_dbg_py_dummy_incref(PyObject * p, const char file[], size_t line)
{
    // TRACE("dummy incref " << p << " " << file << " " << line);

    DBG_ASSERT(p != NULL);

    _banyan_dbg_verify_pyobject(p, file, line);
    
    ++counts.insert(make_pair(p, 0)).first->second;
}

void 
_banyan_dbg_py_incref(PyObject * p, const char file[], size_t line)
{
    // TRACE("incref " << p << " " << file << " " << line);
    
    if (p == NULL) {
        WARN("bad inc " << file << "::" << line << " " << p);
        DBG_VERIFY(false);
    }
            
    ++counts.insert(make_pair(p, 0)).first->second;
    
    Py_INCREF(p);

    _banyan_dbg_verify_pyobject(p, file, line);
}

void 
_banyan_dbg_py_decref(PyObject * p, const char file[], size_t line)
{
    // TRACE("decref " << p << " " << file << " " << line);

    DBG_ASSERT(p != NULL);
    _banyan_dbg_verify_pyobject(p, file, line);

    ref_counts::iterator it = counts.find(p);
    if (it == counts.end()) {
        WARN("bad dec " << file << "::" << line << " " << p);
        DBG_VERIFY(false);
    }
    if (--it->second == 0)
        counts.erase(p);

     Py_DECREF(p);
}

PyObject *
_banyan_dbg_verify_exception(PyObject * p, const char file[], size_t line)
{
    DBG_ASSERT(p != NULL || PyErr_Occurred() != NULL);
    if (p != NULL)
        _banyan_dbg_verify_pyobject(p, file, line);
    return p;
}

int
_banyan_dbg_verify_exception(int status, const char file[], size_t line)
{
    DBG_ASSERT(status == 0 || PyErr_Occurred() != NULL);
    return status;
}
#endif // #ifdef BANYAN_DEBUG

void
_py_warn(PyObject * type, const std::string & msg)
{
    const int warn_res = PyErr_WarnEx(type, msg.c_str(), 2);            
        
    if (warn_res == -1)      
        throw runtime_error("warning threw");              
}

#ifdef BANYAN_DEBUG
void
_PyObjectStdLT::assert_valid() const
{
    // Do nothing.
}
#endif // #ifdef BANYAN_DEBUG

_PyObjectCmpCBLT::_PyObjectCmpCBLT(PyObject * cb_) :
    cb(cb_)
{
    DBG_ASSERT(cb != NULL && PyCallable_Check(cb));
    BANYAN_PYOBJECT_INCREF(cb);
}        

_PyObjectCmpCBLT::_PyObjectCmpCBLT(const _PyObjectCmpCBLT & other) :
    cb(other.cb)
{
    DBG_ASSERT(cb != NULL && PyCallable_Check(cb));
    BANYAN_PYOBJECT_INCREF(cb);
}

_PyObjectCmpCBLT::~_PyObjectCmpCBLT()
{
    BANYAN_PYOBJECT_DECREF(cb);
}

int _PyObjectCmpCBLT::traverse(visitproc visit, void * arg)
{
    Py_VISIT(cb);
    return 0;
}

#ifdef BANYAN_DEBUG
void
_PyObjectCmpCBLT::assert_valid() const
{
    BANYAN_PYOBJECT_VERIFY(cb);
}
#endif // #ifdef BANYAN_DEBUG

bool 
_PyObjectCmpCBLT::operator()(const PyObject * lhs, const PyObject * rhs) const
{
    DBG_ASSERT(cb != NULL && PyCallable_Check(cb));
    PyObject * const ret = PyObject_CallFunctionObjArgs(cb, lhs, rhs, NULL);
    if (ret == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to call bool cmp");    
        throw std::runtime_error("Failed to create .");
    }        
    BANYAN_PYOBJECT_DUMMY_INCREF(ret);
    if (ret == NULL || 
#if PY_MAJOR_VERSION >= 3
        !PyLong_Check(ret)) {
#else // #if PY_MAJOR_VERSION >= 3
        !PyInt_Check(ret)) {
#endif // #if PY_MAJOR_VERSION >= 3
        WARN(ret);
        throw std::runtime_error("Expected int result");
    }
    const int ret_ =  
#if PY_MAJOR_VERSION >= 3
        PyLong_AsLong(ret);
#else // #if PY_MAJOR_VERSION >= 3
        PyInt_AsLong(ret);
#endif // #if PY_MAJOR_VERSION >= 3
    BANYAN_PYOBJECT_DECREF(ret);
    return ret_ < 0;
}

_PyObjectKeyCBLT::_PyObjectKeyCBLT(PyObject * cb_) :
    cb(cb_)
{
    DBG_ASSERT(cb != NULL && PyCallable_Check(cb));
    BANYAN_PYOBJECT_INCREF(cb);
}        

_PyObjectKeyCBLT::_PyObjectKeyCBLT(const _PyObjectKeyCBLT & other) :
    cb(other.cb)
{
    DBG_ASSERT(cb != NULL && PyCallable_Check(cb));
    BANYAN_PYOBJECT_INCREF(cb);
}

_PyObjectKeyCBLT::~_PyObjectKeyCBLT()
{
    BANYAN_PYOBJECT_DECREF(cb);
}

bool 
_PyObjectKeyCBLT::operator()(const PyObject * lhs, const PyObject * rhs) const
{
    DBG_ASSERT(cb != NULL && PyCallable_Check(cb));

    PyObject * const lhs_key = PyObject_CallFunctionObjArgs(cb, lhs, NULL);
    if (lhs_key == NULL) {
        PyErr_SetObject(PyExc_RuntimeError, const_cast<PyObject *>(lhs));    
        throw std::runtime_error("Failed to calculate left key.");
    }    
    BANYAN_PYOBJECT_DUMMY_INCREF(lhs_key);
    
    PyObject * const rhs_key = PyObject_CallFunctionObjArgs(cb, rhs, NULL);
    if (rhs_key == NULL) {
        PyErr_SetObject(PyExc_RuntimeError, const_cast<PyObject *>(rhs));    
        throw std::runtime_error("Failed to calculate right key.");
    }    
    BANYAN_PYOBJECT_DUMMY_INCREF(rhs_key);
    
    const bool lt = PyObject_RichCompareBool(
        BANYAN_PYOBJECT_VERIFY(const_cast<PyObject *>(lhs_key)),
        BANYAN_PYOBJECT_VERIFY(const_cast<PyObject *>(rhs_key)),
        Py_LT);
        
    BANYAN_PYOBJECT_DECREF(lhs_key);
    BANYAN_PYOBJECT_DECREF(rhs_key);
    
    return lt;
}

int 
_PyObjectKeyCBLT::traverse(visitproc visit, void * arg)
{
    Py_VISIT(cb);
    return 0;
}

#ifdef BANYAN_DEBUG
void
_PyObjectKeyCBLT::assert_valid() const
{
    BANYAN_PYOBJECT_VERIFY(cb);
}
#endif // #ifdef BANYAN_DEBUG

_PyObjectCBMetadata::_PyObjectCBMetadata(PyObject * cbs_) :
    cbs(cbs_)
{
    BANYAN_PYOBJECT_INCREF(cbs);
    get_meta();
}

void
_PyObjectCBMetadata::get_meta()
{
    DBG_ASSERT(cbs != NULL && PyTuple_Check(cbs) && PyTuple_Size(cbs) == 2);        
    DBG_ASSERT(PyCallable_Check(PyTuple_GET_ITEM(cbs, 0)));
    DBG_ASSERT(PyCallable_Check(PyTuple_GET_ITEM(cbs, 1)));
        
    PyObject * const create = PyTuple_GET_ITEM(cbs, 0);
    DBG_ASSERT(create != NULL && PyCallable_Check(create));
    meta_ = PyObject_CallFunctionObjArgs(create, NULL);
    if (meta_ == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create meta");    
        throw std::runtime_error("Failed to create .");
    }        
    BANYAN_PYOBJECT_DUMMY_INCREF(meta_);
}

_PyObjectCBMetadata::_PyObjectCBMetadata(const _PyObjectCBMetadata & other) :
    cbs(other.cbs)
{
    BANYAN_PYOBJECT_INCREF(cbs);
    get_meta();
}

_PyObjectCBMetadata &
_PyObjectCBMetadata::operator=(const _PyObjectCBMetadata & other)
{
    if (this != &other) {
        BANYAN_PYOBJECT_INCREF(other.cbs);
        BANYAN_PYOBJECT_DECREF(cbs);
        cbs = other.cbs;    
        BANYAN_PYOBJECT_DECREF(meta_);
        get_meta();
    }
    
    return *this;
}

_PyObjectCBMetadata::~_PyObjectCBMetadata()
{
    DBG_VERIFY(cbs != NULL);
    BANYAN_PYOBJECT_DECREF(cbs);
    DBG_VERIFY(meta_ != NULL);
    BANYAN_PYOBJECT_DECREF(meta_);
}

void 
_PyObjectCBMetadata::update(
    PyObject * value, const _PyObjectCBMetadata * l, const _PyObjectCBMetadata * r)
{
    DBG_ASSERT(value != NULL);

    DBG_ASSERT(cbs != NULL && PyTuple_Check(cbs) && PyTuple_Size(cbs) == 2);        
    DBG_ASSERT(PyCallable_Check(PyTuple_GET_ITEM(cbs, 0)));
    DBG_ASSERT(PyCallable_Check(PyTuple_GET_ITEM(cbs, 1)));
        
    PyObject * const update = PyTuple_GET_ITEM(cbs, 1);
    DBG_ASSERT(update != NULL && PyCallable_Check(update));
    PyObject * const ret = PyObject_CallFunctionObjArgs(
        update, 
        meta_, 
        value, 
        l == NULL? Py_None: l->meta_,
        r == NULL? Py_None: r->meta_,
        NULL);
    if (ret == NULL) {
        PyErr_SetString(PyExc_TypeError, "Failed to update metadata");    
        throw std::runtime_error("Failed to update metadata");
    }    
    BANYAN_PYOBJECT_DUMMY_INCREF(ret);
    BANYAN_PYOBJECT_DECREF(ret);
}

int 
_PyObjectCBMetadata::traverse(visitproc visit, void * arg)
{
    Py_VISIT(cbs);
    Py_VISIT(meta_);
    return 0;
}

PyObject * 
_PyObjectCBMetadata::meta()
{
    return meta_;
}

#ifdef BANYAN_DEBUG
void
_PyObjectCBMetadata::assert_valid() const
{
    BANYAN_PYOBJECT_VERIFY(cbs);    
    
    BANYAN_PYOBJECT_VERIFY(meta_);
}
#endif // #ifdef BANYAN_DEBUG

#ifdef BANYAN_DEBUG
void
_PyObjectCBMetadata::trace(std::ostream & ) const
{
    // Do nothing.
}
#endif // #ifdef BANYAN_DEBUG

_CachedKeyPyObject::_CachedKeyPyObject() :
    orig(NULL),
    cached_key(NULL)
{
    // Do nothing.
}

_CachedKeyPyObject::_CachedKeyPyObject(PyObject * orig_, PyObject * key_fn) :
    orig(orig_),
    cached_key(PyObject_CallFunctionObjArgs(key_fn, orig, NULL))
{
    if (cached_key == NULL) {
        PyErr_SetObject(PyExc_RuntimeError, orig);    
        throw std::runtime_error("Failed to calculate key.");
    }    
    BANYAN_PYOBJECT_DUMMY_INCREF(cached_key);
    BANYAN_PYOBJECT_INCREF(orig);    
}
    
_CachedKeyPyObject::_CachedKeyPyObject(const _CachedKeyPyObject & other) :
    orig(other.orig),
    cached_key(other.cached_key)
{
    if (orig != NULL) {
        BANYAN_PYOBJECT_INCREF(orig);    
        BANYAN_PYOBJECT_INCREF(cached_key);
    }        
}    

_CachedKeyPyObject &
_CachedKeyPyObject::operator=(const _CachedKeyPyObject & other)
{
    if (this != &other) {
        if (orig != NULL) {
            BANYAN_PYOBJECT_DECREF(orig);    
            BANYAN_PYOBJECT_DECREF(cached_key);
        }
                
        orig = other.orig;
        cached_key = other.cached_key;

        if (orig != NULL) {
            BANYAN_PYOBJECT_INCREF(orig);    
            BANYAN_PYOBJECT_INCREF(cached_key);    
        }            
    }
    
    return *this;
}

void
_CachedKeyPyObject::dec()
{    
    BANYAN_PYOBJECT_DECREF(orig);    
    orig = NULL;
    BANYAN_PYOBJECT_DECREF(cached_key);
    cached_key = NULL;
}

int
_CachedKeyPyObject::traverse(visitproc visit, void * arg)
{
    if (orig != NULL) {
        Py_VISIT(orig);        
        Py_VISIT(cached_key);        
    }        
    return 0;        
}

_CachedKeyPyObject::~_CachedKeyPyObject()
{    
    if (orig != NULL)
        BANYAN_PYOBJECT_DECREF(orig);    
    if (cached_key != NULL)        
        BANYAN_PYOBJECT_DECREF(cached_key);
}

#ifdef BANYAN_DEBUG
void 
_CachedKeyPyObject::assert_valid() const
{
    DBG_ASSERT(orig != NULL);
    BANYAN_PYOBJECT_VERIFY(orig);    
    DBG_ASSERT(cached_key != NULL);
    BANYAN_PYOBJECT_VERIFY(cached_key);
}
#endif // #ifdef BANYAN_DEBUG

_CachedKeyPyObjectCacheGeneratorLT::_CachedKeyPyObjectCacheGeneratorLT(PyObject * key_fn_) :
    key_fn(key_fn_)
{
    BANYAN_PYOBJECT_INCREF(key_fn);
}
    
_CachedKeyPyObjectCacheGeneratorLT::_CachedKeyPyObjectCacheGeneratorLT(
        const _CachedKeyPyObjectCacheGeneratorLT & other) :
    key_fn(other.key_fn)
{
    BANYAN_PYOBJECT_INCREF(key_fn);
}
    
_CachedKeyPyObjectCacheGeneratorLT::~_CachedKeyPyObjectCacheGeneratorLT()
{
    BANYAN_PYOBJECT_DECREF(key_fn);
}

_CachedKeyPyObjectCacheGeneratorLT &
_CachedKeyPyObjectCacheGeneratorLT::operator=(const _CachedKeyPyObjectCacheGeneratorLT & other)
{
    if (this != &other) {
        BANYAN_PYOBJECT_DECREF(key_fn);
        key_fn = other.key_fn;
        BANYAN_PYOBJECT_INCREF(key_fn);
    }        
    
    return *this;
}

int
_CachedKeyPyObjectCacheGeneratorLT::traverse(visitproc visit, void * arg)
{
    Py_VISIT(key_fn);    
    return 0;
}

bool 
_CachedKeyPyObjectCacheGeneratorLT::operator()(
    const _CachedKeyPyObject & lhs, const _CachedKeyPyObject & rhs) const
{
    return PyObject_RichCompareBool(lhs.cached_key, rhs.cached_key, Py_LT);
}

_CachedKeyPyObject
_CachedKeyPyObjectCacheGeneratorLT::operator()(PyObject * orig) const
{
    return _CachedKeyPyObject(orig, BANYAN_PYOBJECT_VERIFY(key_fn));
}

