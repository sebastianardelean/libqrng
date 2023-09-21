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
 * @file qrngmodule.h
 * @author Sebastian Mihai Ardelean <sebastian.ardelean@cs.upt.ro>
 * @date 24 May 2023
 * @brief API for interacting with IDQ's Quantis Appliance
 */
#ifndef Py_QRNGMODULE_H
#define Py_QRNGMODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

/**
 * @brief Initialization function
 * This function must be called to initialize libqrng.
 * @param self argument points to the module object for module-level functions; for a method it would point to the object instance.
 * @param args argument will be a pointer to a Python tuple object containing the arguments. IDQ's Quantis Appliance domain address must be specified in this parameter.
 * @return Function returns 0 on SUCCESS, and -1 if libqrng fails.
 * @note On failure, the function performs clean-up.
 */
PyObject* qrand_init(PyObject *self, PyObject *args);

/**
 * @brief Close function.
 * This function must be called to clean-up libqrng.
 * @param self argument points to the module object for module-level functions; for a method it would point to the object instance.
 * @return Always returns 0, SUCCESS.
 */  
PyObject* qrand_close(PyObject *self);

/**
 * @brief Generate random @int@ values.
 * This function generates random @int@ values in the specified interval.
 * @param self argument points to the module object for module-level functions; for a method it would point to the object instance.
 * @param args argument will be a pointer to a Python tuple object containing the arguments. Min, max and number of samples must be specified in this parameter.
 * @return Function returns 0 on SUCCESS, and -1 if libqrnq fails.
 */
PyObject* qrand_rand_int(PyObject *self, PyObject *args);

/**
 * @brief Generate random @float@ values.
 * This function generates random @float@ values in the specified interval.
 * @param self argument points to the module object for module-level functions; for a method it would point to the object instance.
 * @param args argument will be a pointer to a Python tuple object containing the arguments. Min, max and number of samples must be specified in this parameter.
 * @return Function returns 0 on SUCCESS, and -1 if libqrnq fails.
 */  
PyObject* qrand_rand_float(PyObject *self, PyObject *args);


  
#ifdef __cplusplus
}
#endif

#endif /* !defined(Py_QRNGMODULE_H) */
