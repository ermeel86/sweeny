##Efficient simulation of the random-cluster model
*Current version: 0.0.1*
### Description
Sweeny's algorithm is a Markov Chain Monte Carlo algorithm for the random-cluster model (RCM).

The RCM captures models such as Bond-Percolation, Ising model, Potts model or Uniform Spanning Trees.

The computationally demanding problem of Sweeny's algorithm is the 
problem of obtaining connectivity-information while the graph is modified by
edge insertions/deletions. 

This project is a collection of 4 different implementations of Sweeny's algorithm for the RCM
in 2d on the square lattice. 

All, except the Dynamic Connectivity implementation, have average run-times which are powers 
of the system size (at the critical point of the simulated model - That is where usually 
cluster-algorithm are used in Statistical Physics)

All four implementations include sampling facilities for time-series of the following 
observables:

* Number of active edges (internal energy)
* Number of clusters/components
* Size of largest component (order parameter)
* Sum of squared-cluster sizes (Susceptibility

### Dynamic Connectivity implementation (DC)
Implemented using a Dynamic Connectivity algorithm to provide amortized runtime bounds
which are poly-logarithms in the number of vertices. 

### Breadth-First-Search implementation (BFS)
Implementation based on breadth-first searches. Connectivity information are obtained
by traversing the graph. There are two variants: 

* Sequential (SBFS): Starting at one of both vertices
* Interleaved/Alternating (IBFS): To alternating BFS-steps at both vertices (More efficient)

### Union-Find implementation
Implementation based on a combination of an Union-Find algorithm and breadth-first searches.

### Example
```bash
./sweeny_{dc,uf,sbfs,ibfs} -l 128 -q 2 -b 0.88 -j 1 -c 1000 -m 1000 -s 14 -v
```

### Dependencies
To compile the source code you need:

* GNU Scientific Library (GSL) version >= 1.1
* Hierachical Data Format 5 (HDF5) version >= 1.8.

### IPython Notebook to compare to (available) exact results
An IPython Notebook provides a naïve (zero order) test of
all algorithms by comparing the active-bond time-series to 
exact results available for the 2d Ising model.

### TODO

* Clean up code; Proper indentation
* Write detailed documentation
* Change from <linux/types.h> data types to <stdint.h> types for better portability
* Write Python interface for easy usage
* Try some other balanced binary search trees
* Extend to other Graphs like cubic lattice
* Find alternative approaches to the connectivity problem

### References
* Mark Sweeny's original paper: http://prb.aps.org/abstract/PRB/v27/i7/p4445_1
* Preprint describing this work and in-depth analysis: http://arxiv.org/abs/1307.6647 (Please refer to this work when using this source code/program)
* Dynamic Connectivity algorithm used in this work: http://dl.acm.org/citation.cfm?id=502095
* Splay trees which underlie the whole implementation: http://dl.acm.org/citation.cfm?id=3835 
