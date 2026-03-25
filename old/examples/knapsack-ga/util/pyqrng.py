############################################################################
# libqrng - library for interacting with IDQ's Quantis Appliance           #
#                                                                          #
# Copyright (C) 2023  Sebastian Mihai Ardelean                             #
#                                                                          #
# This program is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as published by     #
# the Free Software Foundation, either version 3 of the License, or        #
# (at your option) any later version.                                      #
#                                                                          #
# This program is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of           #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
# GNU General Public License for more details.                             #
#                                                                          #
# You should have received a copy of the GNU General Public License        #
# along with this program.  If not, see <https://www.gnu.org/licenses/>.   #
############################################################################

#############################################################################
# @file pyqrng.py                                                           #
# @author Sebastian Mihai Ardelean <sebastian.ardelean@cs.upt.ro>           #
# @date 24 May 2023                                                         #
# @brief API for interacting with IDQ's Quantis Appliance                   #
#############################################################################
import qrng
from collections import deque
import numpy as np


def randint(l,h,n,pool_size=1000):
    if not hasattr(randint, "ivalues"):
        randint.ivalues = {}
    
#    print("Before init")
    key = str(l)+str(h)
    if key in randint.ivalues:
        #found the key, let's check the values
        values = randint.ivalues[key]
        if len(values) == 0 or len(values)<n:       
            qrng.qrand_init("random.cs.upt.ro")
            values = qrng.qrand_rand_int(l,h,(n+pool_size))
            qrng.qrand_close()
            randint.ivalues[key]+=values
        else:
            pass
    else:
        qrng.qrand_init("random.cs.upt.ro")
        randint.ivalues[key]=qrng.qrand_rand_int(l,h,(n+pool_size))
        qrng.qrand_close()

    outvals = randint.ivalues[key][:n]
    del randint.ivalues[key][:n]
    return np.array(outvals, dtype=np.uint32)

def uniform(l,h,n,pool_size = 1000):
    if not hasattr(uniform, "fvalues"):
        uniform.fvalues = {}
    key = str(l)+str(h)
    if key in uniform.fvalues:
        #found the key
        values = uniform.fvalues[key]
        if len(values) == 0 or len(values)<n:
            qrng.qrand_init("random.cs.upt.ro")
            values = qrng.qrand_rand_float(l,h,(n+pool_size))
            qrng.qrand_close()
            uniform.fvalues[key]+=values
        else:
            pass
    else:
        qrng.qrand_init("random.cs.upt.ro")
        uniform.fvalues[key]=qrng.qrand_rand_float(l,h,(n+pool_size))
        qrng.qrand_close()

    outvals = uniform.fvalues[key][:n]
    del uniform.fvalues[key][:n]
    return np.array(outvals, dtype=float)
