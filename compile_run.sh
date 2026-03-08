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

gcc gray_scale_filter.c   -o gray_serial    -lm
gcc gaussian_blur.c       -o gaussian_serial -lm
gcc sobel_filter.c        -o sobel_serial    -lm
gcc laplacian_filter.c    -o laplacian_serial -lm

echo "Serial compile done."

echo ""
echo ">>> Running SERIAL versions..."
S_GRAY=$(./gray_serial       | grep "time:" | grep -oP '[0-9]+\.[0-9]+')
S_GAUSS=$(./gaussian_serial  | grep "time:" | grep -oP '[0-9]+\.[0-9]+')
S_SOBEL=$(./sobel_serial     | grep "time:" | grep -oP '[0-9]+\.[0-9]+')
S_LAPL=$(./laplacian_serial  | grep "time:" | grep -oP '[0-9]+\.[0-9]+')

# Print outputs without timing (already captured)
./gray_serial
./gaussian_serial
./sobel_serial
./laplacian_serial

cd ..

# ---- OPENMP VERSION ----
echo ""
echo ">>> Compiling OPENMP versions..."
cd openmp_version

gcc -fopenmp gray_scale_filter.c   -o gray_omp     -lm
gcc -fopenmp gaussian_blur.c       -o gaussian_omp -lm
gcc -fopenmp sobel_filter.c        -o sobel_omp    -lm
gcc -fopenmp laplacian_filter.c    -o laplacian_omp -lm

echo "OpenMP compile done."

echo ""
echo ">>> Running OPENMP versions..."
P_GRAY=$(./gray_omp       | grep "time:" | grep -oP '[0-9]+\.[0-9]+')
P_GAUSS=$(./gaussian_omp  | grep "time:" | grep -oP '[0-9]+\.[0-9]+')
P_SOBEL=$(./sobel_omp     | grep "time:" | grep -oP '[0-9]+\.[0-9]+')
P_LAPL=$(./laplacian_omp  | grep "time:" | grep -oP '[0-9]+\.[0-9]+')

./gray_omp
./gaussian_omp
./sobel_omp
./laplacian_omp

cd ..

# ---- SUMMARY TABLE ----
speedup() {
    echo "$1 $2" | awk '{if ($2>0) printf "%.2f", $1/$2; else print "N/A"}'
}

echo ""
echo "=========================================="
echo "  All done! Check output folders."
echo "=========================================="
echo ""
echo "┌─────────────────────────┬────────────────┬────────────────┬──────────┐"
echo "│ Filter                  │ Serial (s)     │ OpenMP (s)     │ Speedup  │"
echo "├─────────────────────────┼────────────────┼────────────────┼──────────┤"
printf "│ %-23s │ %-14s │ %-14s │ %-8s │\n" "Grayscale"       "$S_GRAY"  "$P_GRAY"  "$(speedup $S_GRAY  $P_GRAY)x"
printf "│ %-23s │ %-14s │ %-14s │ %-8s │\n" "Gaussian Blur"   "$S_GAUSS" "$P_GAUSS" "$(speedup $S_GAUSS $P_GAUSS)x"
printf "│ %-23s │ %-14s │ %-14s │ %-8s │\n" "Sobel Edge Det." "$S_SOBEL" "$P_SOBEL" "$(speedup $S_SOBEL $P_SOBEL)x"
printf "│ %-23s │ %-14s │ %-14s │ %-8s │\n" "Laplacian"       "$S_LAPL"  "$P_LAPL"  "$(speedup $S_LAPL  $P_LAPL)x"
echo "└─────────────────────────┴────────────────┴────────────────┴──────────┘"
