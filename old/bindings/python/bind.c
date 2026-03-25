/****************************************************************************
 * libqrng - library for interacting with IDQ's Quantis Appliance           *
 *                                                                          *
 * Copyright (C) 2023  Sebastian Mihai Ardelean                             *
 *                                                                          *
 * This program is free software: you can redistribute it and/or modify     *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation, either version 3 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 ****************************************************************************/

/**
 * @file bind.c
 * @author Sebastian Mihai Ardelean <sebastian.ardelean@cs.upt.ro>
 * @date 24 May 2023
 * @brief API for interacting with IDQ's Quantis Appliance
 */
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
          "qrand_rand_int",
            (PyCFunction)qrand_rand_int,
            METH_VARARGS,
            randomintfunc_docs
        },
        {
          "qrand_rand_float",
          (PyCFunction)qrand_rand_float,
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



