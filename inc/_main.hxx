#pragma once
#include "_utility.hxx"
#ifdef OPENMP
#include "_openmp.hxx"
#endif
#ifdef CUDA
#include "_cuda.hxx"
#endif
#ifdef MPI
#include "_mpi.hxx"
#endif
