#
# Copyright 2008,2009 Free Software Foundation, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# The presence of this file turns this directory into a Python package

"""
This is the GNU Radio CYBERETHER module. Place your Python package
description here (python/__init__.py).
"""

import os

# import pybind11 generated symbols into the cyberether namespace
try:
    # this might fail if the module is python-only
    from . import cyberether_python as _cyberether_python
    from .cyberether_python import *
except ModuleNotFoundError:
    _cyberether_python = None

# import any pure python here


# Open the CyberEther window and block until it is closed.
def present(tb=None):
    if _cyberether_python is None:
        raise RuntimeError("cyberether C++ extension is not available")

    if tb is not None:
        tb.start()
    try:
        _cyberether_python.present()
    finally:
        if tb is not None:
            tb.stop()
            tb.wait()
