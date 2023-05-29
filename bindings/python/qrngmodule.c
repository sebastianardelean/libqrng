#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdint.h>
#include <stdlib.h>
#include "qrngmodule.h"
#include "../qrng.h"



PyObject* init(PyObject *self, PyObject *args)
{
  int retval = 0;
  char *device_domain_address;
  if (!PyArg_ParseTuple(args, "s", &device_domain_address)) {
    return NULL;
  }
  retval = qrng_open(device_domain_address);
  return PyBuildValue("i", retval);
  
  
}


PyObject* random_int(PyObject *,PyObject *)
{
  int64_t min = 0;
  int64_t max = 0;
  size_t samples = 0;
  size_t i = 0;
  int64_t *data;
  if (!PyArg_ParseTuple(args, "iii", &min, &max, &samples)) {
    return NULL;
  }

  data = malloc(samples * sizeof(int64_t));
  if (!data) {
    return NULL;
  }

  qrng_random_int64(min, max, samples, data);

  PyObject *pylist_results = PyList_New(samples);
  for (i = 0; i < samples; i++) {
    PyObject *py_value = Py_BuildValue("i", data[i]);
    PyList_SetItem(pylist_results, i, py_value);
  }

  free(data);
  return pylist_results;
  
}
