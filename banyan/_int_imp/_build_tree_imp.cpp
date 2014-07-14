#include <Python.h>

#include "rank_metadata.hpp"
#include "min_gap_metadata.hpp"
#include "overlapping_intervals_metadata.hpp"
#include "_int_int/_tree_imp_base.hpp"
#include "_int_imp/_build_tree_imp.hpp"
#include "_int_imp/_pyobject_utils.hpp"
#include "_int_imp/_set_tree_imp.hpp"
#include "_int_imp/_dict_tree_imp.hpp"

using namespace std;

// #define BANYAN_MIN_ALG_BUILD
// #define BANYAN_MIN_MAPPING_BUILD
// #define BANYAN_NO_UPDATORS_BUILD

template<
    class Alg_Tag,
    typename Key_Type, 
    bool Set,
    class MetadataTag,
    class LT>
struct _KnownMappingBuilder
{
    // Nothing.
};

template<
    class Alg_Tag,
    typename Key_Type, 
    class MetadataTag,
    class LT>
struct _KnownMappingBuilder<
    Alg_Tag,
    Key_Type,
    false,
    MetadataTag,
    LT>
{
    static _TreeImpBase *
    build_imp(PyObject * seq, PyObject * metadata, PyObject * lt)
    {    
        return new _DictTreeImp<
            Alg_Tag, 
            Key_Type, 
            MetadataTag, 
            LT>(seq, metadata, lt);
    }
};

template<
    class Alg_Tag,
    typename Key_Type,
    class MetadataTag,
    class LT>
struct _KnownMappingBuilder<
    Alg_Tag,
    Key_Type,
    true,
    MetadataTag,
    LT>
{
    static _TreeImpBase *
    build_imp(PyObject * seq, PyObject * metadata, PyObject * lt)
    {
        return new _SetTreeImp<
            Alg_Tag, 
            Key_Type, 
            MetadataTag, 
            LT>(seq, metadata, lt);
    }
};

template<
    class Alg_Tag,
    class Key_Type,
    class MetadataTag,
    class LT>   
struct _KnownMetadataBuilder
{
    static _TreeImpBase *
    build_imp(PyObject * seq, int mapping, PyObject * lt)
    {
#ifndef BANYAN_MIN_MAPPING_BUILD
        if (mapping == 0)
            return _KnownMappingBuilder<Alg_Tag, Key_Type, false, MetadataTag, LT>::build_imp(
                seq, NULL, lt);
#endif // #ifndef BANYAN_MIN_MAPPING_BUILD                
        if (mapping == 1)            
            return _KnownMappingBuilder<Alg_Tag, Key_Type, true, MetadataTag, LT>::build_imp(
                seq, NULL, lt);
        DBG_VERIFY(false);
        return (_TreeImpBase *)NULL;
    }
};

template<
    class Alg_Tag,
    class Key_Type,
    class LT>   
struct _KnownMetadataBuilder<
    Alg_Tag,
    Key_Type,
    _PyObjectCBMetadataTag,
    LT>
{
    static _TreeImpBase *
    build_imp(PyObject * seq, int mapping, PyObject * metadata, PyObject * lt)
    {
#ifndef BANYAN_MIN_MAPPING_BUILD
        if (mapping == 0)
            return _KnownMappingBuilder<Alg_Tag, Key_Type, false, _PyObjectCBMetadataTag, LT>::build_imp(
                seq, metadata, lt);
#endif // #ifndef BANYAN_MIN_MAPPING_BUILD
        if (mapping == 1)            
            return _KnownMappingBuilder<Alg_Tag, Key_Type, true, _PyObjectCBMetadataTag, LT>::build_imp(
                seq, metadata, lt);
        DBG_VERIFY(false);
        return (_TreeImpBase *)NULL;
    }
};

template<
    class Alg_Tag,
    class Key_Type,
    class LT>   
struct _KnownLTBuilder
{
    static _TreeImpBase *
    build_imp(PyObject * seq, int mapping, PyObject * metadata, PyObject * lt)
    {
        if (metadata == Py_None)
            return _KnownMetadataBuilder<Alg_Tag, Key_Type, _NullMetadataTag, LT>::build_imp(
                seq, mapping, lt);                
        DBG_ASSERT(metadata != NULL && PyTuple_Check(metadata) && PyTuple_Size(metadata) == 2);        

#ifndef BANYAN_NO_UPDATORS_BUILD
        PyObject * const meta = PyTuple_GET_ITEM(metadata, 0);
        DBG_ASSERT(PyCallable_Check(meta));
        DBG_ASSERT(PyType_Check(meta));
        PyObject * const inst = PyObject_CallFunctionObjArgs(meta, NULL);        
        if (inst == NULL) {
            PyErr_NoMemory();
            return NULL;
        }        
        BANYAN_PYOBJECT_DUMMY_INCREF(inst);            
        if (PyObject_TypeCheck(inst, &RankMetadataType)) {
            BANYAN_PYOBJECT_DECREF(inst);
            return _KnownMetadataBuilder<Alg_Tag, Key_Type, _RankMetadataTag, LT>::build_imp(
                seq, mapping, lt);                
        }
        if (PyObject_TypeCheck(inst, &MinGapMetadataType)) {
            BANYAN_PYOBJECT_DECREF(inst);
            return _KnownMetadataBuilder<Alg_Tag, Key_Type, _MinGapMetadataTag, LT>::build_imp(
                seq, mapping, lt);                
        }
        if (PyObject_TypeCheck(inst, &OverlappingIntervalsMetadataType)) {
            BANYAN_PYOBJECT_DECREF(inst);
            return _KnownMetadataBuilder<Alg_Tag, Key_Type, _IntervalMaxMetadataTag, LT>::build_imp(
                seq, mapping, lt);                
        }

        BANYAN_PYOBJECT_DECREF(inst);
#endif // #ifndef BANYAN_NO_UPDATORS_BUILD

        return _KnownMetadataBuilder<Alg_Tag, Key_Type, _PyObjectCBMetadataTag, LT>::build_imp(
            seq, mapping, metadata, lt);                
    }            
};

template<
    class Alg_Tag,
    class Key_Type>
struct _Knownkey_typeBuilder
{
    static _TreeImpBase *
    build_imp(PyObject * seq, int mapping, PyObject * metadata)
    {
        if (metadata == Py_None)
            return _KnownMetadataBuilder<Alg_Tag, Key_Type, _NullMetadataTag, std::less<Key_Type> >::build_imp(
                seq, mapping, NULL);
        DBG_ASSERT(metadata != NULL && PyTuple_Check(metadata) && PyTuple_Size(metadata) == 2);        

#ifndef BANYAN_NO_UPDATORS_BUILD
        PyObject * const meta = PyTuple_GET_ITEM(metadata, 0);
        DBG_ASSERT(PyCallable_Check(meta));
        DBG_ASSERT(PyType_Check(meta));
        PyObject * const inst = PyObject_CallFunctionObjArgs(meta, NULL);        
        if (inst == NULL) {
            PyErr_NoMemory();
            return NULL;
        }        
        BANYAN_PYOBJECT_DUMMY_INCREF(inst);            
        if (PyObject_TypeCheck(inst, &RankMetadataType)) {
            BANYAN_PYOBJECT_DECREF(inst);
            return _KnownMetadataBuilder<Alg_Tag, Key_Type, _RankMetadataTag, std::less<Key_Type> >::build_imp(
                seq, mapping, NULL);                
        }
        if (PyObject_TypeCheck(inst, &MinGapMetadataType)) {
            BANYAN_PYOBJECT_DECREF(inst);
            return _KnownMetadataBuilder<Alg_Tag, Key_Type, _MinGapMetadataTag, std::less<Key_Type> >::build_imp(
                seq, mapping, NULL);                
        }
        if (PyObject_TypeCheck(inst, &OverlappingIntervalsMetadataType)) {
            BANYAN_PYOBJECT_DECREF(inst);
            return _KnownMetadataBuilder<Alg_Tag, Key_Type, _IntervalMaxMetadataTag, std::less<Key_Type> >::build_imp(
                seq, mapping, NULL);                
        }

        BANYAN_PYOBJECT_DECREF(inst);
#endif // #ifndef BANYAN_NO_UPDATORS_BUILD

        return _KnownMetadataBuilder<Alg_Tag, Key_Type, _PyObjectCBMetadataTag, std::less<Key_Type> >::build_imp(
            seq, mapping, metadata, NULL);                
    }
};

template<
    class Alg_Tag>
struct _Knownkey_typeBuilder<
    Alg_Tag,
    PyObject *>
{
    static _TreeImpBase *
    build_imp(PyObject * seq, int mapping, PyObject * metadata, PyObject * key, PyObject * cmp)
    {
        DBG_ASSERT(key == Py_None || cmp == Py_None);
#if PY_MAJOR_VERSION >= 3
        // Tmp Ami - make a static warning function.
        if (cmp != Py_None) 
            _py_warn(PyExc_DeprecationWarning, "cmp function deprecated in favour of key function");
#endif // #if PY_MAJOR_VERSION >= 3

        // Tmp Ami - who checks if callable? Should not be in assert.
        if (key != Py_None) 
            return _KnownLTBuilder<Alg_Tag, PyObject *, _PyObjectKeyCBLT>::build_imp(
                seq, mapping, metadata, key);
        if (cmp != Py_None)
            return _KnownLTBuilder<Alg_Tag, PyObject *, _PyObjectCmpCBLT>::build_imp(
                seq, mapping, metadata, cmp);
        return _KnownLTBuilder<Alg_Tag, PyObject *, _PyObjectStdLT>::build_imp(
            seq, mapping, metadata, NULL);
    }
};

static bool _is_long_type(PyObject * type)
{
    DBG_ASSERT(PyType_Check(type));
    PyObject * const inst = PyObject_CallFunctionObjArgs(type, NULL);
    BANYAN_PYOBJECT_DUMMY_INCREF(inst);
    if (inst == NULL) {
        BANYAN_PYOBJECT_DECREF(inst);
        return false;
    }
    
#if PY_MAJOR_VERSION >= 3
    if (PyLong_CheckExact(inst)) {
#else // #if PY_MAJOR_VERSION >= 3
    if (PyInt_CheckExact(inst)) {
#endif // #if PY_MAJOR_VERSION >= 3
        BANYAN_PYOBJECT_DECREF(inst);
        return true;
    }
    BANYAN_PYOBJECT_DECREF(inst);
    return false;
}

static bool _is_double_type(PyObject * type)
{
    DBG_ASSERT(PyType_Check(type));
    PyObject * const inst = PyObject_CallFunctionObjArgs(type, NULL);
    BANYAN_PYOBJECT_DUMMY_INCREF(inst);
    if (inst == NULL) {
        BANYAN_PYOBJECT_DECREF(inst);
        return false;
    }
    
    if (PyFloat_CheckExact(inst)) {
        BANYAN_PYOBJECT_DECREF(inst);
        return true;
    }
    BANYAN_PYOBJECT_DECREF(inst);
    return false;
}

static bool _is_string_type(PyObject * type)
{
    DBG_ASSERT(PyType_Check(type));
    PyObject * const inst = PyObject_CallFunctionObjArgs(type, NULL);
    BANYAN_PYOBJECT_DUMMY_INCREF(inst);
    if (inst == NULL) {
        BANYAN_PYOBJECT_DECREF(inst);
        return false;
    }
    
#if PY_MAJOR_VERSION >= 3
    if (PyByteArray_CheckExact(inst)) {
#else // #if PY_MAJOR_VERSION >= 3
    if (PyString_CheckExact(inst)) {       
#endif // #if PY_MAJOR_VERSION >= 3
        BANYAN_PYOBJECT_DECREF(inst);
        return true;
    }
    BANYAN_PYOBJECT_DECREF(inst);
    return false;
}

static bool _is_unicode_string_type(PyObject * type)
{
    // Tmp Ami - think of these asserts.
    DBG_ASSERT(PyType_Check(type));
    PyObject * const inst = PyObject_CallFunctionObjArgs(type, NULL);
    BANYAN_PYOBJECT_DUMMY_INCREF(inst);
    if (inst == NULL) {
        BANYAN_PYOBJECT_DECREF(inst);
        return false;
    }
    
    if (PyUnicode_CheckExact(inst)) {
        BANYAN_PYOBJECT_DECREF(inst);
        return true;
    }
    BANYAN_PYOBJECT_DECREF(inst);
    return false;
}

template<
    class Alg_Tag>
struct _KnownAlgBuilder
{
    static _TreeImpBase *
    build_imp(
        PyObject * seq,
        PyObject * key_type, 
        int mapping, 
        PyObject * metadata,
        PyObject * key, 
        PyObject * cmp)
    {
        if ((key != Py_None || cmp != Py_None) && key_type != Py_None) {
            _py_warn(
                PyExc_Warning, 
                "Key-type optimization cannot be performed with key function or comparison function");            
            key_type = Py_None;
        }            

        if (key_type == Py_None) 
            return _Knownkey_typeBuilder<Alg_Tag, PyObject *>::build_imp(seq, mapping, metadata, key, cmp);

        DBG_ASSERT(key == Py_None);            
        DBG_ASSERT(cmp == Py_None);            

        if (PyTuple_Check(key_type)) {
            if (PyTuple_Size(key_type) != 2) {
                _py_warn(PyExc_Warning, "Unimplemented key-type optimization");            
                return _Knownkey_typeBuilder<Alg_Tag, PyObject *>::build_imp(seq, mapping, metadata, key, cmp);
            }

            if (
                    _is_long_type(PyTuple_GET_ITEM(key_type, 0)) && 
                    _is_long_type(PyTuple_GET_ITEM(key_type, 1)))                                 
                return _Knownkey_typeBuilder<Alg_Tag, std::pair<long, long> >::build_imp(seq, mapping, metadata);
            if (
                    _is_double_type(PyTuple_GET_ITEM(key_type, 0)) && 
                    _is_double_type(PyTuple_GET_ITEM(key_type, 1)))                                 
                return _Knownkey_typeBuilder<Alg_Tag, std::pair<double, double> >::build_imp(seq, mapping, metadata);

            _py_warn(PyExc_Warning, "Unimplemented key-type optimization");            
            return _Knownkey_typeBuilder<Alg_Tag, PyObject *>::build_imp(seq, mapping, metadata, key, cmp);
        }
        
        if (_is_long_type(key_type))
            return _Knownkey_typeBuilder<Alg_Tag, long>::build_imp(seq, mapping, metadata);
        if (_is_double_type(key_type))
            return _Knownkey_typeBuilder<Alg_Tag, double>::build_imp(seq, mapping, metadata);
        if (_is_string_type(key_type))
            return _Knownkey_typeBuilder<Alg_Tag, String>::build_imp(seq, mapping, metadata);
        if (_is_unicode_string_type(key_type))
            return _Knownkey_typeBuilder<Alg_Tag, UnicodeString>::build_imp(seq, mapping, metadata);

        _py_warn(PyExc_Warning, "Unimplemented key-type optimization");            
        return _Knownkey_typeBuilder<Alg_Tag, PyObject *>::build_imp(seq, mapping, metadata, key, cmp);
    }    
};

_TreeImpBase *
_build_tree_imp(
    int alg, 
    PyObject * seq,
    PyObject * key_type, 
    int mapping,
    PyObject * metadata, 
    PyObject * key,
    PyObject * cmp)
{
    if (alg == 0)
        return _KnownAlgBuilder<_RBTreeTag>::build_imp(seq, key_type, mapping, metadata, key, cmp);
#ifndef BANYAN_MIN_ALG_BUILD
    if (alg == 1)
        return _KnownAlgBuilder<_SplayTreeTag>::build_imp(seq, key_type, mapping, metadata, key, cmp);
    if (alg == 2)
        return _KnownAlgBuilder<_OVTreeTag>::build_imp(seq, key_type, mapping, metadata, key, cmp); 
#endif // #ifndef BANYAN_MIN_ALG_BUILD
    DBG_VERIFY(false);        
    return (_TreeImpBase *)NULL;
}


