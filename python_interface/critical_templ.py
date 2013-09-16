#!/usr/bin/env python
from numpy import sqrt,log
from sys import argv

if len(argv) == 2:
    q = float(argv[1])
    c = 1.
    print log(1 + sqrt(q))/c
elif len(argv) == 3:
    q = float(argv[1])
    c = float(argv[2])
    print log(1 + sqrt(q))/c
