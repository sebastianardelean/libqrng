#!/usr/bin/env python3
from distutils.core import setup, Extension

libraries = ['qrng', 'curl']
library_dirs = ['/usr/lib', '/usr/local/lib']



setup(
	name = "qrng",
	version = "1.0",
        description = "Python interface for the qrng library",
        author = "Sebastian Mihai Ardelean",
        author_email = "sebastian.ardelean@cs.upt.ro",
	ext_modules = [Extension("qrng",
                                 ["bind.c", "qrngmodule.c"],
                                 libraries=libraries,
                                 library_dirs=library_dirs,
                                 extra_link_args=['-lqrng', '-lcurl'])
                       ]
	);
