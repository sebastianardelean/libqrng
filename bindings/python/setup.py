#!/usr/bin/env python3

from distutils.core import setup, Extension

setup(
	name = "qrng",
	version = "1.0",
	ext_modules = [Extension("qrng", ["bind.c", "qrngmodule.c"])]
	);
