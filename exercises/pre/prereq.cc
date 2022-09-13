struct Result {
    float avg[3];
};

/*
This is the function you need to implement. Quick reference:
- x coordinates: 0 <= x < nx
- y coordinates: 0 <= y < ny
- horizontal position: 0 <= x0 < x1 <= nx
- vertical position: 0 <= y0 < y1 <= ny
- color components: 0 <= c < 3
- input: data[c + 3 * x + 3 * nx * y]
- output: avg[c]
*/
Result calculate(int ny, int nx, const float *data, int y0, int x0, int y1, int x1) {
    double red = 0;
    double green = 0;
    double blue = 0;
    unsigned int count = (x1-x0)*(y1-y0);
    for (int j=y0 ; j < y1 ; ++j) {
        for (int i=x0 ; i < x1 ; ++i) {
            red += data[3 * i + 3 * nx * j];
            green += data[3 * i + 3 * nx * j + 1];
            blue += data[3 * i + 3 * nx * j + 2];
        }
    }
    Result result{{float(red/count), float(green/count), float(blue/count)}};
    return result;
}
