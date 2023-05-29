#ifndef Py_QRNGMODULE_H
#define Py_QRNGMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

PyObject* init(PyObject *, PyObject *);
PyObject* random_int(PyObject*, PyObject *);
  
#ifdef __cplusplus
}
#endif

#endif /* !defined(Py_QRNGMODULE_H) */
