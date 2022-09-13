/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/

#include <math.h>

void correlate(int ny, int nx, const float *data, float *result) {
    // double corr;
    double diff;
    double diff2_i;
    double diff2_j;
    double* avgs = new double[ny];
    for (int i=0 ; i < ny ; ++i){
        avgs[i] = 0;
        for (int j=0 ; j < nx ; ++j) {
            avgs[i] += data[j + i*nx];
        }
        avgs[i] /= nx;
    }
    for (int j=0 ; j < ny ; ++j){
        for (int i=0 ; i < j ; ++i) {
            // corr = 0;
            diff = 0;
            diff2_i = 0;
            diff2_j = 0;
            for (int k=0; k < nx ; ++k) {
                diff += (data[k + i*nx] - avgs[i])*(data[k + j*nx] - avgs[j]);

                diff2_i += (data[k + i*nx] - avgs[i])*(data[k + i*nx] - avgs[i]);
                diff2_j += (data[k + j*nx] - avgs[j])*(data[k + j*nx] - avgs[j]);
            }
            result[j + i*ny] = float(diff/sqrt(diff2_i*diff2_j));
        }
    }

    for (int i=0 ; i < ny ; ++i){
        result[i + i*ny] = 1;
    }
    delete [] avgs;
    // free(avgs);
}
