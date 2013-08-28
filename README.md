# Sweeny
#### Description
Various more or less efficient implementations of Sweeny's algorithm for the simulation of the Random Cluster model
on a two-dimensional square lattice with periodic boundary conditions. 
#### Dynamic Connectivity implementation
Implemented using a Dynamic Connectivity algorithm to provide amortized runtime bounds
which are poly-logarithms in the number of vertices. 
The programm provides a simplistic command-line interface to set parameter like the cluster-weight,
inverse temperature, equilibration, number of steps or coupling constant. 
A simulation creates a time-series of observable samples (Monte Carlo). Right now 
a 4 observables are implemented:
* Number of active edges (internal energy)
* Number of clusters/components
* Size of largest component (order parameter)
* Sum of squared-cluster sizes (Susceptibility
#### Breadth-First-Search implementation
Implementation based on breadth-first searches. Connectivity information are obtained
by traversing the graph. There are two variants: 
* Sequential: Starting at one of both vertices
* Interleaved/Alternating: To alternating BFS-steps at both vertices (More efficient)
#### Union-Find implementation
Implementation based on a combination of an Union-Find algorithm and breadth-first searches.
##### Dependencies
To compile the source code you need:
* GNU Scientific Library (GSL) version >= 1.1
* Hierachical Data Format 5 (HDF5) version >= 1.8.

##### Example
```bash
./sweeny_dc -l 128 -q 2 -b 0.88 -j 1 -c 1000 -m 1000 -s 14
```

##### TODO
* Write Python interface
* Write detailed documentation
* Try some other balanced binary search trees
* Extend to other Graphs like cubic lattice

#### References
* Mark Sweeny's original paper: http://prb.aps.org/abstract/PRB/v27/i7/p4445_1
* Preprint desciribing this work and in-depth analysis: http://arxiv.org/abs/1307.6647 (Please refer to this work when using this source code/program)
* Dynamic Connectivity algorithm used in this work: http://dl.acm.org/citation.cfm?id=502095
* Splay trees which underly the whole implementation: http://dl.acm.org/citation.cfm?id=3835 
