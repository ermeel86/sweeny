#!/usr/bin/env python
"""
Define the ctypes interface to the Monte Carlo C implementation
"""
import ctypes
import numpy as np
LIB_PATH="./sweeny_mc.so"

symc = ctypes.CDLL(LIB_PATH)
# specify parameter types
symc.sweeny_setup.argtypes = [ctypes.c_uint, ctypes.c_double, ctypes.c_uint,
        ctypes.c_double, ctypes.c_double, ctypes.c_uint, ctypes.c_uint,ctypes.c_uint,
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p]
symc.sweeny_destroy.argtypes = None
symc.sweeny_simulate.argtypes = None

# specify return types
#symc.sweeny_setup.restype = ctypes
#symc.sweeny_destroy.restype = None
#symc.sweeny_simulate.restype = ctypes.c_char

def sy_setup(impl_idx,q,l,beta,coupl,cutoff,rngseed,a,b,c,d):
    return symc.sweeny_setup(ctypes.c_uint(int(impl_idx)),
            ctypes.c_double(float(q)),
            ctypes.c_uint(int(l)),
            ctypes.c_double(float(beta)),
            ctypes.c_double(float(coupl)),
            ctypes.c_uint(int(cutoff)),
            ctypes.c_uint(a.ctypes.shape[0]),
            ctypes.c_uint(int(rngseed)),
            a.ctypes.data,
            b.ctypes.data,
            c.ctypes.data,
            d.ctypes.data)




def sy_simulate():
    return symc.sweeny_simulate()

def sy_destroy():
    symc.sweeny_destroy()
