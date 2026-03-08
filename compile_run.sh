#!/bin/bash
# ============================================================
#  COMPILE AND RUN SCRIPT  – Group 03
#  Run this from the project root directory:  bash compile_run.sh
# ============================================================

echo "=========================================="
echo "  EC7207 HPC Project – Group 03"
echo "  Compiling Serial + OpenMP versions"
echo "=========================================="

# ---- SERIAL VERSION ----
echo ""
echo ">>> Compiling SERIAL versions..."
cd serial_version

gcc sobel_filter.c        -o sobel_serial    -lm
gcc laplacian_filter.c    -o laplacian_serial -lm

echo "Serial compile done."

echo ""
echo ">>> Running SERIAL versions..."

./sobel_serial
./laplacian_serial

cd ..

# ---- OPENMP VERSION ----
echo ""
echo ">>> Compiling OPENMP versions..."
cd openmp_version

gcc -fopenmp sobel_filter.c        -o sobel_omp    -lm
gcc -fopenmp laplacian_filter.c    -o laplacian_omp -lm

echo "OpenMP compile done."

echo ""
echo ">>> Running OPENMP versions..."
./sobel_omp
./laplacian_omp

cd ..

echo ""
echo "=========================================="
echo "  All done! Check output folders."
echo "=========================================="
