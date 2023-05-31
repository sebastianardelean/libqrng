#!/usr/bin/env python3

import qrng
import json

print(qrng.qrand_init("random.cs.upt.ro"))
print(qrng.qrand_rand_int(0, 6,10))
print(qrng.qrand_rand_float(0.0, 1.0, 10))
print(qrng.qrand_close());


