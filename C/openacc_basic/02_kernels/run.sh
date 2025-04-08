#!/bin/bash
#PBS -q lecture-g
#PBS -l select=1
#PBS -l walltime=00:01:00
#PBS -W group_list=gt00
#PBS -j oe

cd $PBS_O_WORKDIR
module load nvidia 

# GPUカーネルが並列化されず、極端に遅いため、
# このジョブは制限時間として設定した1分で終わらず、制限超過での異常終了となる

./run
