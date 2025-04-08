#!/bin/bash
#PBS -q lecture-mig
#PBS -l select=2:mpiprocs=1
#PBS -l walltime=00:01:00
#PBS -W group_list=gt00
#PBS -j oe

cd $PBS_O_WORKDIR
module load nvidia nv-hpcx

mkdir -p images

size=512
total_step=5000
img_step=50
nprocs=2
mpirun -n $nprocs ./run $size $size $nprocs $total_step $img_step

module load python
python3 ../create_images.py $size $total_step $img_step

rm result.grd
