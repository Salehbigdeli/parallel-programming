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
#include<cstdlib>

typedef double double8_t __attribute__ ((vector_size (8 * sizeof(double))));


static double8_t* double8_alloc(std::size_t n) {
    void* tmp = 0;
    if (posix_memalign(&tmp, sizeof(double8_t), sizeof(double8_t) * n)) {
        throw std::bad_alloc();
    }
    return (double8_t*)tmp;
}


void correlate(int ny, int nx, const float *data, float *result) {

    constexpr int parts = 8;
    int buckets = (nx + parts - 1) / parts;
    int patitionable_nx = buckets*parts;

    double8_t* vdata_minus_rows_average = double8_alloc(ny*buckets);
    double8_t* vdata_minus_rows_average_replica = double8_alloc(ny*buckets);
    double* auto_correlation = new double[ny];
    


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
            int p = j/parts;
            int z = j%parts;
            double t = data[i*nx+j] - row_mean;
            vdata_minus_rows_average[i*buckets+p][z] = t;
            vdata_minus_rows_average_replica[i*buckets+p][z] = t;
            auto_correlation[i] += t*t;
        }
    }

    
    if (patitionable_nx != nx){

        #pragma omp parallel for
        for (int i=0 ; i < ny ; ++i){
            for (int j=nx ; j < patitionable_nx ; ++j) {
                int t = j/parts;
                int p = j%parts;
                vdata_minus_rows_average[i*buckets+t][p] = 0;
            }
        }
    }

    #pragma omp parallel for
    for (int j=0 ; j < ny ; ++j){
        for (int i=0 ; i < j ; ++i) {

            double diff = 0;
            double8_t diffs = {0};
            for (int k=0; k < patitionable_nx/parts ; ++k) {
                // if (t+k*parts >= nx) break; // prevents optimizations !!!
                double8_t row_i_part = vdata_minus_rows_average[k + i*buckets];
                double8_t row_j_part = vdata_minus_rows_average_replica[k + j*buckets];
                diffs += row_i_part * row_j_part;
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
    std::free(vdata_minus_rows_average);
    delete [] auto_correlation;
    std::free(vdata_minus_rows_average_replica);
}
