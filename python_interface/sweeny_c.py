#!/usr/bin/env python
"""
Define the ctypes interface to the Monte Carlo C implementation
"""
import ctypes
import numpy as np
LIB_PATH="../obj/lib_sweeny_mc.so"

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
    """
    Setup/Initialize a sweeny simulation

    Parameters
    ----------
    q: float
        Cluster-weight, (>0).
    l: int
        Number of vertices per dimension, (>2)
    beta: float
        Inverse temperature, (>0)
    coupl: float
        Coupling constant, (>0)/Ferromagnetic
    cutoff: int
        Number of sweeps for equilibration, (>0)
    tslength: int
        Number of sweeps of actual simulation. This also corresponds
        to the number of observable samples, (>0)
    rngseed: int
        Seed for the pseudo-random-number generator, (>0)
    impl: str
        Type of implementation. 'ibfs', 'sbfs', 'uf' and 'dc' 
        correspond to Interleaved/Sequential BFS, Union-Find and
        Dynamic Connectivity,respectively
    a: array_like (dtype=np.uint32)
        Array for time-series of number of bonds
    b: array_like (dtype=np.uint32)
        Array for time-series of number of cluster
    c: array_like (dtype=np.uint32)
        Array for time-series of size of giant component
    d: array_like (dtype=np.uint64)
        Array for time-series of second cluster size moment


    Returns
    -------
    Returns 0 when failed, otherwise a positive integer (1,2)
    """
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
    """
    Start the prepared/configured simulation. Note if run multiple times
    the previous state is used as the "initial" configuration of the 
    simulation (markov process). 
    """
    return symc.sweeny_simulate()

def sy_destroy():
    """
    Free all resources of the simulation (except the time-series)
    """
    symc.sweeny_destroy()
