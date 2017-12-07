#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name="test_fork_hanging", version="1.0",
      ext_modules=[Extension("test_fork_hanging", ["test_fork_hanging.c"])])
