#!/usr/bin/env python3
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
# @file setup.py                                                            #
# @author Sebastian Mihai Ardelean <sebastian.ardelean@cs.upt.ro>           #
# @date 24 May 2023                                                         #
# @brief API for interacting with IDQ's Quantis Appliance                   #
#############################################################################
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
