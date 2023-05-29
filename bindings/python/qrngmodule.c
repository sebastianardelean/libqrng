#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdint.h>
#include <stdlib.h>
#include <qrng.h>
#include <stdio.h>
#include "qrngmodule.h"



PyObject* qrand_init(PyObject *self, PyObject *args)
{

  int retval = 0;
  char *device_domain_address;
  if (!PyArg_ParseTuple(args, "s", &device_domain_address)) {
    return NULL;
  }
  retval = qrng_open(device_domain_address);
  return Py_BuildValue("i", retval);
  
  
}

PyObject* qrand_close(PyObject *self )
{
  qrng_close();
  return Py_BuildValue("i", 0);
}


PyObject* qrand_rand_int(PyObject *self, PyObject *args)
{
  int64_t min = 0;
  int64_t max = 0;
  size_t samples = 0;
  size_t i = 0;
  int64_t *data;
  if (!PyArg_ParseTuple(args, "LLK", &min, &max, &samples)) {
    return NULL;
  }

  data = malloc(samples * sizeof(int64_t));
  if (!data) {
    return NULL;
  }

  if (qrng_random_int64(min, max, samples, data)) {
    return NULL;
  }
  
  PyObject *pylist_results = PyList_New(samples);
  for (i = 0; i < samples; i++) {
    PyObject *py_value = Py_BuildValue("L", data[i]);
    PyList_SetItem(pylist_results, i, py_value);
  }

  free(data);
  return pylist_results;
  
  
  return NULL;
}


PyObject* qrand_rand_float(PyObject *self, PyObject *args)
{
  double min = 0.0;
  double max = 1.0;
  size_t samples = 0;
  size_t i =0;
  double *data;
    if (!PyArg_ParseTuple(args, "ddK", &min, &max, &samples)) {
    return NULL;
  }

  data = malloc(samples * sizeof(double));
  if (!data) {
    return NULL;
  }

  if (qrng_random_double(min, max, samples, data)) {
    return NULL;
  }
  
  PyObject *pylist_results = PyList_New(samples);
  for (i = 0; i < samples; i++) {
    PyObject *py_value = Py_BuildValue("d", data[i]);
    PyList_SetItem(pylist_results, i, py_value);
  }

  free(data);
  return pylist_results;
  
  
  return NULL;
}



