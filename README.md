fmincg-c
========

Conjugate gradient implementation in C. This is a port of fmincg.m in Octave. There are two implementations of fmincg in C included here.
The one in cpu/ is suitable for execution on the CPU. The second in openCL/ is suited to execution as an openCL kernel. 

When using openCL, often the cost function and gradient evaluation is performed on an openCL target such as the GPU. If the fmincg minimizer is run on the host CPU,
the latency of the data trasfer between the host(CPU) and the device(GPU) may become a bottleneck. So it is desirable to run the minimizer as an openCL kernel. However, openCL kernels cannot call other openCL kernels.
The implementation of fmincg in openCL/ is written in such a way that the state of the fmincg minimizer is kept entirely in memory and can be resumed after cost function computation.

Dependencies
-------------

- GCC toolchain
- OpenCL driver
- PyOpenCL

The last two are needed only for the openCL implementation.

Examples
--------

See cpu/example.c and openCL/example.py for examples on how to use the minimizer.


