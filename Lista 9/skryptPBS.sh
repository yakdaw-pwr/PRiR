#!/bin/bash
#PBS -N MPI-DesBreaker
#PBS -l walltime=01:00:00
#PBS -j oe
#PBS -l select=2:ncpus=6:mpiprocs=12
cd ~/pbs
mpicc /home/32b-g1/Lista9/main.c -o MPI-DesBreaker -lcrypt
mpirun -n 1 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8nNMN5joJMI6 8
mpirun -n 2 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8nNMN5joJMI6 8
mpirun -n 8 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8nNMN5joJMI6 8

mpirun -n 1 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a80P6qKwQIhTQ 8
mpirun -n 2 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a80P6qKwQIhTQ 8
mpirun -n 8 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a80P6qKwQIhTQ 8

mpirun -n 1 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8bgkm3M5nUXQ 8
mpirun -n 2 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8bgkm3M5nUXQ 8
mpirun -n 8 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8bgkm3M5nUXQ 8

mpirun -n 8 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8jMSX/IQv.Pw 8
mpirun -n 8 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8.NqxJsLzjfM 8
mpirun -n 8 --mca btl tcp,self --mca pml ob1 MPI-DesBreaker a8bgkm3M5nUXQ 8
