#!/bin/bash
#PBS -q lecture-g
#PBS -l select=1
#PBS -l walltime=00:01:00
#PBS -W group_list=gt00
#PBS -j oe

cd $PBS_O_WORKDIR
module load nvidia 

./run

echo
echo "----- Collecting performance information using NVCOMPILER_ACC_TIME -----"
export NVCOMPILER_ACC_TIME=1
./run

unset NVCOMPILER_ACC_TIME

echo "----- Collecting performance information using Nsight Systems -----"
nsys profile -f true -t cuda,nvtx,openacc,mpi -o report ./run
