#define MODULE_VERSION "1.0"

#include <Python.h>
#include <structmember.h>

#include <cstdio>
#include <cstdlib>

#include "tree.hpp"
#include "frozen_set_tree.hpp"
#include "set_tree.hpp"
#include "frozen_dict_tree.hpp"
#include "dict_tree.hpp"
#include "tree_view.hpp"
#include "node.hpp"
#include "rank_metadata.hpp"
#include "min_gap_metadata.hpp"
#include "overlapping_intervals_metadata.hpp"
#include "_int_imp/_pyobject_utils.hpp"

using namespace std;

struct module_state
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyMethodDef banyan_c_methods[] =
{
    { NULL, NULL }
};

#if PY_MAJOR_VERSION >= 3

static int banyan_c_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int banyan_c_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "banyan_c",
    NULL,
    sizeof(struct module_state),
    banyan_c_methods,
    NULL,
    banyan_c_traverse,
    banyan_c_clear,
    NULL
};

#define INITERROR return NULL

extern "C" PyObject *
PyInit_banyan_c(void)

#else
#define INITERROR return

extern "C" void
initbanyan_c(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule("banyan_c", banyan_c_methods);
#endif

    if (module == NULL)
        INITERROR;
    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException((char *)"banyan_c.Error", NULL, NULL);
    if (st->error == NULL) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    
    if (PyType_Ready(&TreeType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&TreeType);
    PyModule_AddObject(module, "Tree", (PyObject *)&TreeType);

    FrozenSetTreeType.tp_base = &TreeType;
    if (PyType_Ready(&FrozenSetTreeType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&FrozenSetTreeType);
    PyModule_AddObject(module, "FrozenSetTree", (PyObject *)&FrozenSetTreeType);

    SetTreeType.tp_base = &FrozenSetTreeType;
    if (PyType_Ready(&SetTreeType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&SetTreeType);
    PyModule_AddObject(module, "SetTree", (PyObject *)&SetTreeType);

    FrozenDictTreeType.tp_base = &TreeType;
    if (PyType_Ready(&FrozenDictTreeType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&FrozenDictTreeType);
    PyModule_AddObject(module, "FrozenDictTree", (PyObject *)&FrozenDictTreeType);

    DictTreeType.tp_base = &FrozenDictTreeType;
    if (PyType_Ready(&DictTreeType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&DictTreeType);
    PyModule_AddObject(module, "DictTree", (PyObject *)&DictTreeType);

    if (PyType_Ready(&TreeViewType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&TreeViewType);
    PyModule_AddObject(module, "TreeView", (PyObject *)&TreeViewType);

    if (PyType_Ready(&NodeType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&NodeType);
    PyModule_AddObject(module, "Node", (PyObject *)&NodeType);

    if (PyType_Ready(&RankMetadataType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&RankMetadataType);
    PyModule_AddObject(module, "RankMetadata", (PyObject *)&RankMetadataType);

    if (PyType_Ready(&MinGapMetadataType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&MinGapMetadataType);
    PyModule_AddObject(module, "MinGapMetadata", (PyObject *)&MinGapMetadataType);

    if (PyType_Ready(&OverlappingIntervalsMetadataType) < 0) {
        BANYAN_PYOBJECT_DECREF(module);
        INITERROR;
    }
    BANYAN_PYOBJECT_INCREF((PyObject *)&OverlappingIntervalsMetadataType);
    PyModule_AddObject(module, "OverlappingIntervalsMetadata", (PyObject *)&OverlappingIntervalsMetadataType);

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
