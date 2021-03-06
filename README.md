# SKaMPI 6


## What is this?

Originally, SKaMPI is a benchmark for MPI libraries, developed and
maintained at the University of Karlsruhe, Germany.

This version was extended to support OpenSHMEM libraries.

## Documentation

The full documentation can be found in the doc subdirectory.  There
are an extensive manual and a very short guide for impatient users in
Postscript and Pdf formats.

## Quickstart

```
  git clone https://github.com/coti/SKaMPI.git
  cd skampi
  ./configure
  make
  mpirun -np 2 ./skampi -i ski/skampi_pt2pt.ski
```


Main configuration flags:
* `--disable-mpi` : by default, MPI is enabled. This will disable the compilation of the MPI support, measurements and infrastructure. OpenSHMEM must be enabled.
* `--enable-openshmem` : enable OpenSHMEM. This will compile the OpenSHMEM  measurement functions and, if MPI is disabled, the OpenSHMEM infrastructure.
* `--enable-papi` : use PAPI for measurements
* `--prefix` : installation path.

