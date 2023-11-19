# Multithread Genetic Algorithm with OpenCL

This repository contains a C++ genetic algorithm code that I developed a year after graduating on 23/02/2017. The code features a highly generic interface to adapt to various optimization problems. An example included solves the boolean knapsack problem. Although comments are in Portuguese, the code itself is in English, making it intuitive to understand. 

The code supports both MultiThreading and OpenCL, enabling execution on GPU, including NVIDIA and AMD, or any hardware accelerator with C++ support.

Additionally, in the ```operators``` folder, there are pre-programmed functions for crossover, mutation, and selection, each in their respective files with matching names.

The file ```parameters.h``` contains all additional configurations necessary for the genetic algorithm to function.
