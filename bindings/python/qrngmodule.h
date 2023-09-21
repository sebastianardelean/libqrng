#ifndef Py_QRNGMODULE_H
#define Py_QRNGMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

PyObject* qrand_init(PyObject *, PyObject *);
PyObject* qrand_close(PyObject *);
PyObject* qrand_rand_int(PyObject*, PyObject *);
PyObject* qrand_rand_float(PyObject *, PyObject *);


  
#ifdef __cplusplus
}
#endif

#endif /* !defined(Py_QRNGMODULE_H) */
