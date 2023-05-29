#include <Python.h>
#include "libmypy.h"
#include "structmember.h"
#include <stddef.h>
typedef struct {
  PyObject_HEAD
  int64_t value;
}PyInt64Object;

static struct PyMemberDef Custom_members[] = {
    
    {"value", T_INT, offsetof(PyInt64Object, value), 0, "custom number"},
    {NULL}  /* Sentinel */
};

static PyTypeObject CustomType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "custom.Custom",
    .tp_doc = PyDoc_STR("Custom objects"),
    .tp_basicsize = sizeof(PyInt64Object),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_members = Custom_members,
};

PyObject * hello(PyObject * self) {
	return PyUnicode_FromFormat("Hello C extension!");
}

PyObject * heyman(PyObject *self, PyObject *args) {
	char *name;

	if(!PyArg_ParseTuple(args, "s", &name))
		return NULL;

	return PyUnicode_FromFormat("Hay %s!", name);
}

PyObject * add(PyObject *self, PyObject *args) {
	int num1, num2;


	if(!PyArg_ParseTuple(args, "ii", &num1, &num2))
		return NULL;

        PyInt64Object *value;
        if (PyType_Ready(&CustomType) < 0){
          return NULL;
        }

        value = PyObject_New(PyInt64Object,&CustomType);
        value->value = 4;

        PyObject *vals = PyTuple_New(5);

        int64_t vals_arr[]={1,2,3,4,5};
        int i = 0;
        for (i=0; i<5;i++) {
          PyTuple_SET_ITEM(vals, i, Py_BuildValue("i",vals_arr[i]));
        }

        int samples = 5;
        PyObject* python_val = PyList_New(samples);
        for (i=0;i<samples;i++) {
          PyObject* python_int = Py_BuildValue("i", vals_arr[i]);
          PyList_SetItem(python_val, i, python_int);
        }
        
        //	return Py_BuildValue("i", num1+num2);
        return python_val;
}
