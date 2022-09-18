/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
#include <math.h>
#include <new>

typedef float float8_t __attribute__ ((vector_size (8 * sizeof(float))));


static float8_t* float8_alloc(std::size_t n) {
    void* tmp = 0;
    if (posix_memalign(&tmp, sizeof(float8_t), sizeof(float8_t) * n)) {
        throw std::bad_alloc();
    }
    return (float8_t*)tmp;
}


void correlate(int ny, int nx, const float *data, float *result) {

    constexpr int parts = 8;
    int buckets = (nx + parts - 1) / parts;
    int patitionable_nx = buckets*parts;

    double* data_minus_rows_average = new double[ny*patitionable_nx];
    double* data_minus_rows_average_replica = new double[ny*patitionable_nx];
    double* auto_correlation = new double[ny];
    
    #pragma omp parallel for
    for (int i=0 ; i < ny ; ++i){
        auto_correlation[i] = 0;
    } 

    #pragma omp parallel for
    for (int i=0 ; i < ny ; ++i){
        double row_mean = 0;
        for (int j=0 ; j < nx ; ++j) {
            row_mean += data[i*nx+j];
        }
        row_mean /= nx;
        for (int j=0 ; j < nx ; ++j) {
            double t = data[i*nx+j] - row_mean;
            data_minus_rows_average[i*patitionable_nx+j] = t;
            data_minus_rows_average_replica[i*patitionable_nx+j] = t;
            auto_correlation[i] += t*t;
        }
    }

    
    if (patitionable_nx != nx){

        #pragma omp parallel for
        for (int i=0 ; i < ny ; ++i){
            for (int j=nx ; j < patitionable_nx ; ++j) {
                data_minus_rows_average[i*patitionable_nx+j] = 0;
            }
        }
    }

    #pragma omp parallel for
    for (int j=0 ; j < ny ; ++j){
        for (int i=0 ; i < j ; ++i) {

            double diff = 0;
            double diffs[parts] = {0};
            for (int k=0; k < patitionable_nx/parts ; ++k) {
                for(int t=0; t < parts ; ++t){
                    // if (t+k*parts >= nx) break; // prevents optimizations !!!
                    double row_i_part = data_minus_rows_average[t+k*parts + i*patitionable_nx];
                    double row_j_part = data_minus_rows_average_replica[t+k*parts + j*patitionable_nx];
                    diffs[t] += row_i_part * row_j_part;
                }
                
            }
            for(int t=0; t <  parts; ++t){
                diff += diffs[t];
            }
            result[j + i*ny] = float(diff/sqrt(auto_correlation[i]*auto_correlation[j]));
        }
    }

    #pragma omp parallel for
    for (int i=0 ; i < ny ; ++i){
        result[i + i*ny] = 1;
    }
    delete [] data_minus_rows_average;
    delete [] auto_correlation;
    delete [] data_minus_rows_average_replica;
}
