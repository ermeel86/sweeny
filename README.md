# Sweeny_DC
#### Dynamic Connectivity implementation of Sweeny's algorithm
##### Description
This is an implementation of Sweeny's algorithm for the simulation of the Random Cluster model
on a two-dimensional square lattice with periodic boundary conditions. 
It is implemented using a Dynamic Connectivity algorithm to provide amortized runtime bounds
which are poly-logarithms in the number of vertices. 
The programm provides a simplistic command-line interface to set parameter like the cluster-weight,
inverse temperature, equilibration, number of steps or coupling constant. 
A simulation creates a time-series of observable samples (Monte Carlo). Right now 
a 4 observables are implemented:
* Number of active edges (internal energy)
* Number of clusters/components
* Size of largest component (order parameter)
* Sum of squared-cluster sizes (Susceptibility

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
