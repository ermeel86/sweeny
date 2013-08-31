from __future__ import division
cimport foobar
#cimport def_sweeny_c
from foobar import hello_world
#from def_sweeny_c import sweeny_setup,sweeny_simulate,sweeny_destroy
#import numpy as np
#cimport numpy as np
import ctypes
cimport cython

def hello():
    hello_world()
#def sy_setup(char impl_idx, double q, unsigned int l, double beta,
#        double coupl, unsigned int cutoff,unsigned int rngseed, 
#        np.ndarray[np.uint32_t,ndim=1] a,
#        np.ndarray[np.uint32_t,ndim=1] b,
#        np.ndarray[np.uint32_t,ndim=1] c,
#        np.ndarray[np.uint64_t,ndim=1] d):
#    return sweeny_setup()
#   # return <char> sweeny_setup(impl_idx, q,l,beta,coupl,cutoff,<unsigned int> a.shape[0],
#   #          rngseed,<char *>&a[0],<char *>&b[0],<char *>&c[0],<char *> &d[0])
#
#def sy_simulate():
#    return <char >sweeny_simulate()
#
#def sy_destroy():
#    sweeny_destroy()
