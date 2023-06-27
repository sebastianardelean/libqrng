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
