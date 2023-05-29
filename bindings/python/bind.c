#include "qrngmodule.h"

char initfunc_docs[] = "Init function.";
char randomintfunc_docs[] = "Generate random int numbers.";
char closefunc_docs[] = "Close function";
char randomdoublefunc_docs[] = "Generate random float numbers.";
PyMethodDef  qrng_funcs[] = {
        {
          "qrand_close",
          (PyCFunction)qrand_close,
          METH_NOARGS,
          closefunc_docs
        },
	{
          "qrand_init",
          (PyCFunction)qrand_init,
          METH_VARARGS,
          initfunc_docs
        },
	{
          "qrand_random_int",
            (PyCFunction)qrand_random_int,
            METH_VARARGS,
            randomintfunc_docs
        },
        {
          "qrand_random_float",
          (PyCFunction)qrand_random_float,
          METH_VARARGS,
          randomdoublefunc_docs
        },
	{
          NULL
        }
};

char qrngmod_docs[] = "Qrng module.";

PyModuleDef qrng_mod = {
	PyModuleDef_HEAD_INIT,
	"qrng",
	qrngmod_docs,
	-1,
	qrng_funcs,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_qrng(void) {
	return PyModule_Create(&qrng_mod);
}



