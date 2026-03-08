# EC7207 – HPC Project | Group 03
## Parallel Image Kernel Processing (Serial + OpenMP)

**Members:**
- EG/2020/3989 – Jayasinghe U. R.     → Grayscale Filter
- EG/2020/4029 – Kulathilaka W. A. S. P. → Gaussian Blur
- EG/2020/4064 – Malinda H. W. J.     → Sobel + Laplacian

---

## Folder Structure

```
HPC-Project-Group03/
├── stb_image.h               
├── stb_image_write.h         
├── compile_run.sh            
├── images/
│   └── testing_images/
│       └── input.jpg         
│
├── serial_version/
│   ├── gray_scale_filter.c   
│   ├── gaussian_blur.c       
│   ├── sobel_filter.c        
│   ├── laplacian_filter.c    
│   └── output/
│       ├── grayscale/
│       ├── gaussian/
│       ├── sobel/
│       └── laplacian/
│
└── openmp_version/
    ├── gray_scale_filter.c   
    ├── gaussian_blur.c       
    ├── sobel_filter.c        
    ├── laplacian_filter.c    
    └── output/
        ├── grayscale/
        ├── gaussian/
        ├── sobel/
        └── laplacian/
```

---

### Compile and run (all at once)
```bash
bash compile_run.sh
```

### OR compile individually:
```bash
# Serial
cd serial_version
gcc gray_scale_filter.c -o gray_serial -lm && ./gray_serial

# OpenMP
cd openmp_version
gcc -fopenmp gray_scale_filter.c -o gray_omp -lm && ./gray_omp
```

---

## Filters Explained

| Filter | What it does | Kernel size |
|---|---|---|
| Grayscale | RGB → single brightness value | Formula (no kernel) |
| Gaussian Blur | Smooths/blurs using weighted average | 5×5 |
| Sobel Edge Detection | Finds edges using gradient in X and Y | 3×3 |
| Laplacian Filter | Finds all edges using second derivative | 3×3 |

---
